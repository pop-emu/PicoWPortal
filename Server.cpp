#include "Server.hpp"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include <string>

err_t tcp_server_accept(void* arg, struct tcp_pcb* pcb, err_t err)
{
    Server* server = (Server*)arg;

    return server->Accept(pcb, err);
}

err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    return ((Server*)arg)->Recv(tpcb, p, err);
}

err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    return ((Server*)arg)->Sent(tpcb, len);
}

static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
    return ((Server*)arg)->Poll(tpcb);
}

void tcp_server_err(void *arg, err_t err) {
    ((Server*)arg)->Err(err);
}

Server::Server()
{
    if(cyw43_arch_init())
    {
        printf("WiFi init failed");
        exit(-1);
    }

    cyw43_arch_enable_sta_mode();

    cyw43_wifi_pm(&cyw43_state, CYW43_PERFORMANCE_PM);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

	if(cyw43_arch_wifi_connect_timeout_ms("ZiggoFA834C1", "Ck7jtupfzpaB", CYW43_AUTH_WPA2_AES_PSK, 30000))
	{
		printf("Failed to connect");
		exit(-1);
	}

	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

	char* ip = ip4addr_ntoa(netif_ip4_addr(netif_list));

	printf("My ip is %s", ip);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);

    tcp_bind(pcb, NULL, 80);

    server_pcb = tcp_listen_with_backlog(pcb, 1);

    tcp_arg(server_pcb, this);
    tcp_accept(server_pcb, tcp_server_accept);
}

void Server::Poll()
{
    cyw43_arch_poll();
}

err_t Server::Accept(struct tcp_pcb* pcb, err_t err)
{
    if(pcb == nullptr || err != ERR_OK)
    {
        return ERR_VAL;
    }
    this->client_pcb = pcb;

    tcp_arg(client_pcb, this);

    tcp_sent(client_pcb, tcp_server_sent);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_poll(client_pcb, tcp_server_poll, 10);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

err_t Server::Recv(struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if(p->tot_len > 0)
    {
        const uint16_t left = 2048 - recv_count;
        recv_count += pbuf_copy_partial(p, buffer_recv + recv_count, p->tot_len > left ? left : p->tot_len, 0);
        tcp_recved(tpcb, p->tot_len);
    }

    std::string data = "<html><body><h1>HTTP 200 Status Code Sample with HTTP Request Methods</h1></body></html>";

    err = ERR_OK;
    err = tcp_write(client_pcb, data.c_str(), data.length(), TCP_WRITE_FLAG_COPY);

    pbuf_free(p);

    return err;
}

err_t Server::Sent(struct tcp_pcb *tpcb, u16_t len)
{
    return CloseClient();
}

err_t Server::Poll(struct tcp_pcb* tpcb)
{
    return CloseClient();
}

void Server::Err(err_t err)
{
    if (err != ERR_ABRT) {
        CloseClient();
    }
}

err_t Server::CloseClient()
{
    err_t err = ERR_OK;
    if(client_pcb != nullptr)
    {
        memset(buffer_sent, 0, 2048);
        memset(buffer_recv, 0, 2048);
        recv_count = 0;

        tcp_arg(client_pcb, nullptr);

        tcp_sent(client_pcb, nullptr);
        tcp_recv(client_pcb, nullptr);
        tcp_poll(client_pcb, nullptr, 0);
        tcp_err(client_pcb, nullptr);

        err = tcp_close(client_pcb);

        if(err != ERR_OK)
        {
            tcp_abort(client_pcb);
            err = ERR_ABRT;
        }
        client_pcb = nullptr;
    }

    return err;
}