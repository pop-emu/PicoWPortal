#include "Server.hpp"

#include "pico/cyw43_arch.h"

void Server::Initialize()
{
    cyw43_arch_enable_ap_mode("Portal of Power", "Test_Password", CYW43_AUTH_WPA2_AES_PSK);
}

void Server::Poll()
{
    cyw43_arch_poll();
}