#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "bsp/board.h"
#include "tusb.h"

int main()
{
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return -1;
    }

    tusb_init();

    while(true)
    {
        tud_task();
    }

    return 0;
}
