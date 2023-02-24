#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "bsp/board.h"
#include "tusb.h"

#include "Portal.hpp"
#include "Server.hpp"

int main()
{
    stdio_init_all();

    tusb_init();

    Portal::Initialize();

    Server::Initialize();

    while(true)
    {
        tud_task();
        Portal::Poll();
        Server::Poll();
    }

    return 0;
}
