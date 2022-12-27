#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include <cstring>

#define MAX_HEADERS 50
#define MAX_METHOD_LEN 10
#define MAX_URL_LEN 100
#define MAX_VERSION_LEN 10

typedef struct {
  char method[MAX_METHOD_LEN];
  char url[MAX_URL_LEN];
  char version[MAX_VERSION_LEN];
  char headers[MAX_HEADERS][2][1024];
  int num_headers;
} http_request_t;

int parse_http_request(char* buffer, http_request_t* request);

static err_t tcp_server_accept(void* arg, struct tcp_pcb* pcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
static void tcp_server_err(void *arg, err_t err);

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
    char buffer_recv[2048];
};