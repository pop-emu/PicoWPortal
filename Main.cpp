#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

int main()
{
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    int connected;

    do
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

        connected = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000);

        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

        sleep_ms(500);
    }
    while(connected);

    return 0;
}
