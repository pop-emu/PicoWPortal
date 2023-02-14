#include "tusb.h"

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
{
  .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0x1430,
    .idProduct          = 0x0150,
    .bcdDevice          = 0x0100,//0x2508,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x00,

    .bNumConfigurations = 0x01
// 18 bytes
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

#define TUD_HID_REPORT \
    HID_USAGE_PAGE_N(0xFF00, 2),\
    HID_USAGE(0x01),\
    HID_COLLECTION(HID_COLLECTION_APPLICATION),\
      HID_USAGE_MIN(0x01), \
      HID_USAGE_MAX(0x40), \
      HID_LOGICAL_MIN(0x00), \
      /*HID_LOGICAL_MAX(0xFF), gives wrong result. Use manual data below instead.*/\
      0x26, 0xFF, 0x00, \
      HID_REPORT_SIZE(0x08), \
      HID_REPORT_COUNT(0x20), \
      HID_INPUT(HID_DATA | HID_ARRAY | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION), \
      HID_USAGE_MIN(0x01), \
      HID_USAGE_MAX(0xFF), \
      HID_OUTPUT(HID_DATA | HID_ARRAY | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION | HID_NON_VOLATILE), \
    HID_COLLECTION_END \

uint8_t const desc_hid_report[] =
{
    TUD_HID_REPORT
};

#if 0
uint8_t const desc_hid_report_old[] =
{
  0x06, 0x00, 0xFF,  // Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        // Usage (0x01)
  0xA1, 0x01,        // Collection (Application)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0x40,        //   Usage Maximum (0x40)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x20,        //   Report Count (32)
  0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0xFF,//0x40,        //   Usage Maximum (0x40)
  0x91, 0x00,        //   Output(Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              // End Collection
// 29 bytes
};

#endif

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
{
  (void) instance;
  return desc_hid_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum
{
  ITF_NUM_HID,
  ITF_NUM_TOTAL
};

#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

#define EPNUM_HID   0x81

#if 0

uint8_t const desc_configuration_macro_test[] =
{
    TUD_CONFIG_DESCRIPTOR(0x01, 0x01, 0x00, 41, 0x80, 0x01F4),

    TUD_HID_INOUT_DESCRIPTOR(0x00, 0x00, 0x00, sizeof(desc_hid_report), 0x02, 0x81, 0x00, 0x01)
};

#endif

uint8_t const desc_configuration[] =
{
  0x09,        // bLength
  0x02,        // bDescriptorType (Configuration)
  0x29, 0x00,  // wTotalLength 41
  0x01,        // bNumInterfaces 1
  0x01,        // bConfigurationValue
  0x00,        // iConfiguration (String Index)
  0x80,        // bmAttributes
  0xFA,//0x96,        // bMaxPower 300mA

  0x09,        // bLength
  0x04,        // bDescriptorType (Interface)
  0x00,        // bInterfaceNumber 0
  0x00,        // bAlternateSetting
  0x02,        // bNumEndpoints 2
  0x03,        // bInterfaceClass
  0x00,        // bInterfaceSubClass
  0x00,        // bInterfaceProtocol
  0x00,        // iInterface (String Index)

  0x09,        // bLength
  0x21,        // bDescriptorType (HID)
  0x11, 0x01,  // bcdHID 1.11
  0x00,        // bCountryCode
  0x01,        // bNumDescriptors
  0x22,        // bDescriptorType[0] (HID)
  0x1D, 0x00,  // wDescriptorLength[0] 29

  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x81,        // bEndpointAddress (IN/D2H)
  0x03,        // bmAttributes (Interrupt)
  0x40, 0x00,//0x20, 0x00,  // wMaxPacketSize 32
  0x01,//0x04,        // bInterval 4 (unit depends on device speed)

  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x02,//0x01,        // bEndpointAddress (OUT/H2D)
  0x03,        // bmAttributes (Interrupt)
  0x40, 0x00,//0x20, 0x00,  // wMaxPacketSize 32
  0x01,//0x04,        // bInterval 4 (unit depends on device speed)
// 41 bytes
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations

  return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
// 0: supported language. English is 0x0409, written in little endian
// 1: manufacturer
// 2: product. For every portal, this is "Spyro Porta"
// 3: serial ID. Use one from my own portals. Should be replaced with the device id of the pico
char const* string_desc_arr [] =
{
  (const char[]) { 0x09, 0x04 },
  "Activision",
  "Spyro Porta",
  "99B3f9C9E6"
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;

  uint8_t chr_count;

  if ( index == 0)
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  }else
  {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

    const char* str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    if ( chr_count > 31 ) chr_count = 31;

    // Convert ASCII string into UTF-16
    for(uint8_t i=0; i<chr_count; i++)
    {
      _desc_str[1+i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

  return _desc_str;
}