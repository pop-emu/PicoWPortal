#include "lwip/pbuf.h"
#include "lwip/tcp.h"

static err_t tcp_server_accept(void* arg, struct tcp_pcb* pcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
void tcp_server_err(void *arg, err_t err);

class Server
{
    public:
    Server();
    void Poll();
    err_t Accept(struct tcp_pcb* pcb, err_t err);
    err_t Recv(struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    err_t Sent(struct tcp_pcb *tpcb, u16_t len);
    err_t Poll(struct tcp_pcb* tpcb);
    void Err(err_t err);
    err_t CloseClient();
    

    private:
    tcp_pcb* server_pcb;
    tcp_pcb* client_pcb;
    uint8_t buffer_sent[2048];
    uint8_t recv_count = 0;
    uint8_t buffer_recv[2048];
};