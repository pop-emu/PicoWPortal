#include <lwip/tcp.h>
#include <lwip/pbuf.h>
#include "pico/cyw43_arch.h"
#include <iostream>
#include <string>
#include <sstream>
#include "Portal.hpp"

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::string body;
};

bool PathsMatch(std::string given, std::string expected);

class Server {
public:
    static void Initialize();
    static void Poll();

    static err_t CloseClient();

    static err_t Accept(void* arg, struct tcp_pcb* pcb, err_t err);
    static err_t Poll(void *arg, struct tcp_pcb *tpcb);
    static err_t Sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
    static err_t Recieved(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static void Error(void *arg, err_t err);

    static HttpRequest ParseHttpRequest(char* request);

    static err_t RespondNotFound();
    static err_t RespondJSON(std::string json);

private:
    static tcp_pcb* server_pcb;
    static tcp_pcb* client_pcb;
    static unsigned int recv_count;
    static unsigned char buffer_sent[2048];
    static unsigned char buffer_recv[2048];
};