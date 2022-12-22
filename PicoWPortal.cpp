#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/cyw43_arch.h"

#include "bsp/board.h"
#include "tusb.h"

#include "Portal.hpp"
#include "Server.hpp"

int main()
{
    stdio_init_all();

    Server* server = new Server();

	board_init();

	gpio_init(14);

	gpio_set_dir(14, GPIO_IN);

	gpio_pull_up(14);

	tusb_init();

    while(true)
    {
		server->Poll();
        tud_task();
		Portal::GetPortal()->PortalTask();
		if(!gpio_get(14))
		{
			reset_usb_boot(0, 0);
		}
    }

    return 0;
}

void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{

}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

static void send_hid_report(uint8_t report_id, uint32_t btn)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
	Portal::GetPortal()->readyToSend = true;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{


  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
	if(report_type == HID_REPORT_TYPE_OUTPUT)
	{
		Portal::GetPortal()->HandleCommand(buffer, bufsize);
	}
}