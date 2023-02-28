#include "Server.hpp"

tcp_pcb* Server::server_pcb;
tcp_pcb* Server::client_pcb;
unsigned int Server::recv_count;
unsigned char Server::buffer_sent[2048];
unsigned char Server::buffer_recv[2048];

void Server::Initialize()
{
    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return;
    }

    //cyw43_arch_enable_ap_mode("Portal of Power", "Test_Password", CYW43_AUTH_WPA2_AES_PSK);

    cyw43_arch_enable_sta_mode();

    cyw43_wifi_pm(&cyw43_state, CYW43_PERFORMANCE_PM);

    if(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
	{
		printf("Failed to connect");
	}

    char* ip = ip4addr_ntoa(netif_ip4_addr(netif_default));

	printf("My ip is %s", ip);

    const char* hostname = netif_get_hostname(netif_default);

    printf("My hostname is %s", hostname);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);

    tcp_bind(pcb, NULL, 80);

    server_pcb = tcp_listen_with_backlog(pcb, 1);

    tcp_accept(server_pcb, Server::Accept);

    // turn led on to signal the network has been initialized
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
}

void Server::Poll()
{
    cyw43_arch_poll();
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

err_t Server::Accept(void* arg, struct tcp_pcb* pcb, err_t err)
{
    if(pcb == nullptr || err != ERR_OK)
    {
        return ERR_VAL;
    }
    client_pcb = pcb;

    tcp_sent(client_pcb, Server::Sent);
    tcp_recv(client_pcb, Server::Recieved);
    tcp_poll(client_pcb, Server::Poll, 10);
    tcp_err(client_pcb, Server::Error);

    return ERR_OK;
}

err_t Server::Poll(void *arg, struct tcp_pcb *tpcb)
{
    return Server::CloseClient();
}

err_t Server::Sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    return CloseClient();
}

err_t Server::Recieved(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if(p->tot_len > 0)
    {
        const uint16_t left = 2048 - recv_count;
        recv_count += pbuf_copy_partial(p, buffer_recv + recv_count, p->tot_len > left ? left : p->tot_len, 0);
        tcp_recved(tpcb, p->tot_len);
    }

    pbuf_free(p);

    HttpRequest request = ParseHttpRequest((char *)buffer_recv);

    if(request.method.rfind("GET", 0) == 0)
    {
        if(request.path.rfind("/api/v1/info", 0) == 0)
        {
            std::string json =  "{"
                                    "\"name\":\"PicoWPortal\","
                                    "\"version\":\"0.0.1\""
                                "}";

            return RespondJSON(json);
        }
        if(request.path.rfind("/api/v1/status", 0) == 0)
        {
            std::string figures = "";

            for(unsigned char i = 0; i < 16; i++)
            {
                Figure figure = Portal::figures[i];

                std::string figureData =    "{"
                                                "\"status\":" + std::to_string(figure.status) + ","
                                                "\"id\":" + std::to_string(figure.GetShort(0x01, 0x00)) + ","
                                                "\"variant\":" + std::to_string(figure.GetShort(0x01, 0x0C)) +
                                            "}";

                if(i < 15)
                {
                    figureData += std::string(",");
                }

                figures += figureData;
            }

            std::string json =  "{"
                                    "\"portal\": {"
                                        "\"active\":" + std::string((Portal::active)? "true" : "false") + ","
                                        "\"counter\":" + std::to_string(Portal::counter) + ","
                                        "\"colors\":{"
                                            "\"left\":{"
                                                "\"red\":" + std::to_string(Portal::colorLeft.red) + ","
                                                "\"green\":" + std::to_string(Portal::colorLeft.green) + ","
                                                "\"blue\":" + std::to_string(Portal::colorLeft.blue) +
                                            "},"
                                            "\"right\":{"
                                                "\"red\":" + std::to_string(Portal::colorRight.red) + ","
                                                "\"green\":" + std::to_string(Portal::colorRight.green) + ","
                                                "\"blue\":" + std::to_string(Portal::colorRight.blue) +
                                            "},"
                                            "\"trap\":{"
                                                "\"red\":" + std::to_string(Portal::colorTrap.red) + ","
                                                "\"green\":" + std::to_string(Portal::colorTrap.green) + ","
                                                "\"blue\":" + std::to_string(Portal::colorTrap.blue) +
                                            "}"
                                        "}"
                                    "},"
                                    "\"figures\":[" 
                                        + figures +
                                    "]"
                                "}";

            return RespondJSON(json);
        }
    }
    
    return RespondNotFound();

}

void Server::Error(void *arg, err_t err)
{
    if (err != ERR_ABRT) {
        CloseClient();
    }
}

HttpRequest Server::ParseHttpRequest(char* request) {
    std::stringstream stream(request);
    std::string line;
    HttpRequest httpRequest;

    // Parse the first line of the request
    std::getline(stream, line);
    std::stringstream lineStream(line);
    lineStream >> httpRequest.method >> httpRequest.path >> httpRequest.version;

    // Parse the headers
    while (std::getline(stream, line) && !line.empty()) {
        // Do something with the headers if needed
    }

    // Parse the body if present
    if (stream.peek() != EOF) {
        std::getline(stream, httpRequest.body);
    }

    return httpRequest;
}

err_t Server::RespondNotFound()
{
    std::string data = "HTTP/1.1 404 Not Found\r\n\r\n";

    return tcp_write(client_pcb, data.c_str(), data.length(), TCP_WRITE_FLAG_COPY);
}

err_t Server::RespondJSON(std::string json)
{
    std::string data =  "HTTP/1.1 200 OK\r\n" \
                    "Content-Type: application/json\r\n" \
                    "Content-Length: " +
                    std::to_string(json.length()) +
                    "\r\n\r\n" +
                    json;

    return tcp_write(client_pcb, data.c_str(), data.length(), TCP_WRITE_FLAG_COPY);
}