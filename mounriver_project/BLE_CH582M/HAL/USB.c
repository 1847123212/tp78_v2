/********************************** (C) COPYRIGHT *******************************
 * File Name          : USB.c
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2023/2/4
 * Description        : CherryUSB驱动
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "usbd_core.h"
#include "usbd_hid.h"
#include "usbd_msc.h"

#ifndef WBVAL
#define WBVAL(x) (unsigned char)((x) & 0xFF), (unsigned char)(((x) >> 8) & 0xFF)
#endif

/*!< USBD CONFIG */
#define USBD_VERSION 0x0110
#define USBD_PRODUCT_VERSION 0x0001
#define USBD_VID 0xffff
#define USBD_PID 0xffff
#define USBD_MAX_POWER 0xfa
#define USBD_LANGID_STRING 1033
#define USBD_CONFIG_DESCRIPTOR_SIZE 107

/*!< USBD ENDPOINT CONFIG */
#define USBD_IF0_AL0_EP0_ADDR 0x81
#define USBD_IF0_AL0_EP0_SIZE 0x08
#define USBD_IF0_AL0_EP0_INTERVAL 0x01

#define USBD_IF1_AL0_EP0_ADDR 0x82
#define USBD_IF1_AL0_EP0_SIZE 0x04
#define USBD_IF1_AL0_EP0_INTERVAL 0x01

#define USBD_IF2_AL0_EP0_ADDR 0x83
#define USBD_IF2_AL0_EP0_SIZE 0x01
#define USBD_IF2_AL0_EP0_INTERVAL 0x01

#define USBD_IF3_AL0_EP0_ADDR 0x04
#define USBD_IF3_AL0_EP0_SIZE 0x40
#define USBD_IF3_AL0_EP0_INTERVAL 0x00

#define USBD_IF3_AL0_EP1_ADDR 0x84
#define USBD_IF3_AL0_EP1_SIZE 0x40
#define USBD_IF3_AL0_EP1_INTERVAL 0x00

/*!< USBD HID CONFIG */
#define USBD_HID_VERSION 0x0111
#define USBD_HID_COUNTRY_CODE 0
#define USBD_IF0_AL0_HID_REPORT_DESC_SIZE 63
#define USBD_IF1_AL0_HID_REPORT_DESC_SIZE 74
#define USBD_IF2_AL0_HID_REPORT_DESC_SIZE 33

/*!< USBD Descriptor */
const unsigned char usbd_descriptor[] = {
/********************************************** Device Descriptor */
    0x12,                                       /*!< bLength */
    0x01,                                       /*!< bDescriptorType */
    WBVAL(USBD_VERSION),                        /*!< bcdUSB */
    0x00,                                       /*!< bDeviceClass */
    0x00,                                       /*!< bDeviceSubClass */
    0x00,                                       /*!< bDeviceProtocol */
    0x40,                                       /*!< bMaxPacketSize */
    WBVAL(USBD_VID),                            /*!< idVendor */
    WBVAL(USBD_PID),                            /*!< idProduct */
    WBVAL(USBD_PRODUCT_VERSION),                /*!< bcdDevice */
    0x01,                                       /*!< iManufacturer */
    0x02,                                       /*!< iProduct */
    0x03,                                       /*!< iSerial */
    0x01,                                       /*!< bNumConfigurations */
/********************************************** Config Descriptor */
    0x09,                                       /*!< bLength */
    0x02,                                       /*!< bDescriptorType */
    WBVAL(USBD_CONFIG_DESCRIPTOR_SIZE),         /*!< wTotalLength */
    0x04,                                       /*!< bNumInterfaces */
    0x01,                                       /*!< bConfigurationValue */
    0x00,                                       /*!< iConfiguration */
    0xa0,                                       /*!< bmAttributes */
    USBD_MAX_POWER,                             /*!< bMaxPower */
/********************************************** Interface 0 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x00,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x01,                                       /*!< bNumEndpoints */
    0x03,                                       /*!< bInterfaceClass */
    0x01,                                       /*!< bInterfaceSubClass */
    0x01,                                       /*!< bInterfaceProtocol */
    0x00,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of HID */
    0x09,                                       /*!< bLength */
    0x21,                                       /*!< bDescriptorType */
    WBVAL(USBD_HID_VERSION),                    /*!< bcdHID */
    USBD_HID_COUNTRY_CODE,                      /*!< bCountryCode */
    0x01,                                       /*!< bNumDescriptors */
    0x22,                                       /*!< bDescriptorType */
    WBVAL(USBD_IF0_AL0_HID_REPORT_DESC_SIZE),   /*!< wItemLength */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF0_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF0_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF0_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Interface 1 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x01,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x01,                                       /*!< bNumEndpoints */
    0x03,                                       /*!< bInterfaceClass */
    0x01,                                       /*!< bInterfaceSubClass */
    0x02,                                       /*!< bInterfaceProtocol */
    0x00,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of HID */
    0x09,                                       /*!< bLength */
    0x21,                                       /*!< bDescriptorType */
    WBVAL(USBD_HID_VERSION),                    /*!< bcdHID */
    USBD_HID_COUNTRY_CODE,                      /*!< bCountryCode */
    0x01,                                       /*!< bNumDescriptors */
    0x22,                                       /*!< bDescriptorType */
    WBVAL(USBD_IF1_AL0_HID_REPORT_DESC_SIZE),   /*!< wItemLength */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF1_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF1_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF1_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Interface 2 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x02,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x01,                                       /*!< bNumEndpoints */
    0x03,                                       /*!< bInterfaceClass */
    0x01,                                       /*!< bInterfaceSubClass */
    0x00,                                       /*!< bInterfaceProtocol */
    0x00,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of HID */
    0x09,                                       /*!< bLength */
    0x21,                                       /*!< bDescriptorType */
    WBVAL(USBD_HID_VERSION),                    /*!< bcdHID */
    USBD_HID_COUNTRY_CODE,                      /*!< bCountryCode */
    0x01,                                       /*!< bNumDescriptors */
    0x22,                                       /*!< bDescriptorType */
    WBVAL(USBD_IF2_AL0_HID_REPORT_DESC_SIZE),   /*!< wItemLength */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF2_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF2_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF2_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Interface 3 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x03,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x02,                                       /*!< bNumEndpoints */
    0x08,                                       /*!< bInterfaceClass */
    0x06,                                       /*!< bInterfaceSubClass */
    0x50,                                       /*!< bInterfaceProtocol */
    0x00,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of MSC */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF3_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x02,                                       /*!< bmAttributes */
    WBVAL(USBD_IF3_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF3_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Endpoint 1 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF3_AL0_EP1_ADDR,                      /*!< bEndpointAddress */
    0x02,                                       /*!< bmAttributes */
    WBVAL(USBD_IF3_AL0_EP1_SIZE),               /*!< wMaxPacketSize */
    USBD_IF3_AL0_EP1_INTERVAL,                  /*!< bInterval */
/********************************************** Language ID String Descriptor */
    0x04,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    WBVAL(USBD_LANGID_STRING),                  /*!< wLangID0 */
/********************************************** String 1 Descriptor */
/* Your Manufacturer */
    0x24,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x59, 0x00,                                 /*!< 'Y' wcChar0 */
    0x6f, 0x00,                                 /*!< 'o' wcChar1 */
    0x75, 0x00,                                 /*!< 'u' wcChar2 */
    0x72, 0x00,                                 /*!< 'r' wcChar3 */
    0x20, 0x00,                                 /*!< ' ' wcChar4 */
    0x4d, 0x00,                                 /*!< 'M' wcChar5 */
    0x61, 0x00,                                 /*!< 'a' wcChar6 */
    0x6e, 0x00,                                 /*!< 'n' wcChar7 */
    0x75, 0x00,                                 /*!< 'u' wcChar8 */
    0x66, 0x00,                                 /*!< 'f' wcChar9 */
    0x61, 0x00,                                 /*!< 'a' wcChar10 */
    0x63, 0x00,                                 /*!< 'c' wcChar11 */
    0x74, 0x00,                                 /*!< 't' wcChar12 */
    0x75, 0x00,                                 /*!< 'u' wcChar13 */
    0x72, 0x00,                                 /*!< 'r' wcChar14 */
    0x65, 0x00,                                 /*!< 'e' wcChar15 */
    0x72, 0x00,                                 /*!< 'r' wcChar16 */
/********************************************** String 2 Descriptor */
/* Your Product */
    0x1a,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x59, 0x00,                                 /*!< 'Y' wcChar0 */
    0x6f, 0x00,                                 /*!< 'o' wcChar1 */
    0x75, 0x00,                                 /*!< 'u' wcChar2 */
    0x72, 0x00,                                 /*!< 'r' wcChar3 */
    0x20, 0x00,                                 /*!< ' ' wcChar4 */
    0x50, 0x00,                                 /*!< 'P' wcChar5 */
    0x72, 0x00,                                 /*!< 'r' wcChar6 */
    0x6f, 0x00,                                 /*!< 'o' wcChar7 */
    0x64, 0x00,                                 /*!< 'd' wcChar8 */
    0x75, 0x00,                                 /*!< 'u' wcChar9 */
    0x63, 0x00,                                 /*!< 'c' wcChar10 */
    0x74, 0x00,                                 /*!< 't' wcChar11 */
/********************************************** String 3 Descriptor */
/* Your Serial Number */
    0x26,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x59, 0x00,                                 /*!< 'Y' wcChar0 */
    0x6f, 0x00,                                 /*!< 'o' wcChar1 */
    0x75, 0x00,                                 /*!< 'u' wcChar2 */
    0x72, 0x00,                                 /*!< 'r' wcChar3 */
    0x20, 0x00,                                 /*!< ' ' wcChar4 */
    0x53, 0x00,                                 /*!< 'S' wcChar5 */
    0x65, 0x00,                                 /*!< 'e' wcChar6 */
    0x72, 0x00,                                 /*!< 'r' wcChar7 */
    0x69, 0x00,                                 /*!< 'i' wcChar8 */
    0x61, 0x00,                                 /*!< 'a' wcChar9 */
    0x6c, 0x00,                                 /*!< 'l' wcChar10 */
    0x20, 0x00,                                 /*!< ' ' wcChar11 */
    0x4e, 0x00,                                 /*!< 'N' wcChar12 */
    0x75, 0x00,                                 /*!< 'u' wcChar13 */
    0x6d, 0x00,                                 /*!< 'm' wcChar14 */
    0x62, 0x00,                                 /*!< 'b' wcChar15 */
    0x65, 0x00,                                 /*!< 'e' wcChar16 */
    0x72, 0x00,                                 /*!< 'r' wcChar17 */
    0x00
};

/*!< USBD HID REPORT 0 Descriptor */
const unsigned char usbd_hid_0_report_descriptor[USBD_IF0_AL0_HID_REPORT_DESC_SIZE] = {
        0x05, 0x01, // USAGE_PAGE (Generic Desktop)
        0x09, 0x06, // USAGE (Keyboard)
        0xa1, 0x01, // COLLECTION (Application)
        0x05, 0x07, // USAGE_PAGE (Keyboard)
        0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)
        0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x25, 0x01, // LOGICAL_MAXIMUM (1)
        0x75, 0x01, // REPORT_SIZE (1)
        0x95, 0x08, // REPORT_COUNT (8)
        0x81, 0x02, // INPUT (Data,Var,Abs)
        0x95, 0x01, // REPORT_COUNT (1)
        0x75, 0x08, // REPORT_SIZE (8)
        0x81, 0x03, // INPUT (Cnst,Var,Abs)
        0x95, 0x05, // REPORT_COUNT (5)
        0x75, 0x01, // REPORT_SIZE (1)
        0x05, 0x08, // USAGE_PAGE (LEDs)
        0x19, 0x01, // USAGE_MINIMUM (Num Lock)
        0x29, 0x05, // USAGE_MAXIMUM (Kana)
        0x91, 0x02, // OUTPUT (Data,Var,Abs)
        0x95, 0x01, // REPORT_COUNT (1)
        0x75, 0x03, // REPORT_SIZE (3)
        0x91, 0x03, // OUTPUT (Cnst,Var,Abs)
        0x95, 0x06, // REPORT_COUNT (6)
        0x75, 0x08, // REPORT_SIZE (8)
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x25, 0xFF, // LOGICAL_MAXIMUM (255)
        0x05, 0x07, // USAGE_PAGE (Keyboard)
        0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))
        0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)
        0x81, 0x00, // INPUT (Data,Ary,Abs)
        0xc0        // END_COLLECTION
};

/*!< USBD HID REPORT 1 Descriptor */
const unsigned char usbd_hid_1_report_descriptor[USBD_IF1_AL0_HID_REPORT_DESC_SIZE] = {
        0x05, 0x01, // USAGE_PAGE (Generic Desktop)
        0x09, 0x02, // USAGE (Mouse)
        0xA1, 0x01, // COLLECTION (Application)
        0x09, 0x01, //   USAGE (Pointer)

        0xA1, 0x00, //   COLLECTION (Physical)
        0x05, 0x09, //     USAGE_PAGE (Button)
        0x19, 0x01, //     USAGE_MINIMUM (Button 1)
        0x29, 0x03, //     USAGE_MAXIMUM (Button 3)

        0x15, 0x00, //     LOGICAL_MINIMUM (0)
        0x25, 0x01, //     LOGICAL_MAXIMUM (1)
        0x95, 0x03, //     REPORT_COUNT (3)
        0x75, 0x01, //     REPORT_SIZE (1)

        0x81, 0x02, //     INPUT (Data,Var,Abs)
        0x95, 0x01, //     REPORT_COUNT (1)
        0x75, 0x05, //     REPORT_SIZE (5)
        0x81, 0x01, //     INPUT (Cnst,Var,Abs)

        0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
        0x09, 0x30, //     USAGE (X)
        0x09, 0x31, //     USAGE (Y)
        0x09, 0x38,

        0x15, 0x81, //     LOGICAL_MINIMUM (-127)
        0x25, 0x7F, //     LOGICAL_MAXIMUM (127)
        0x75, 0x08, //     REPORT_SIZE (8)
        0x95, 0x03, //     REPORT_COUNT (2)

        0x81, 0x06, //     INPUT (Data,Var,Rel)
        0xC0, 0x09,
        0x3c, 0x05,
        0xff, 0x09,

        0x01, 0x15,
        0x00, 0x25,
        0x01, 0x75,
        0x01, 0x95,

        0x02, 0xb1,
        0x22, 0x75,
        0x06, 0x95,
        0x01, 0xb1,

        0x01, 0xc0 //   END_COLLECTION
};


/*!< USBD HID REPORT 2 Descriptor */
const unsigned char usbd_hid_2_report_descriptor[USBD_IF2_AL0_HID_REPORT_DESC_SIZE] = {
        0x05, 0x0C,
        0x09, 0x01,
        0xA1, 0x01,

        0x09, 0xB0,
        0x09, 0xB5,
        0x09, 0xB6,
        0x09, 0xE9,
        0x09, 0xEA,
        0x09, 0xE2,
        0x09, 0xB1,
        0x09, 0xB7,

        0x15, 0x00,
        0x25, 0x01,
        0x95, 0x08,
        0x75, 0x01,
        0x81, 0x02,
        0xC0
};

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

/*!< hid state ! Data can be sent only when state is idle  */
static volatile uint8_t hid_state = HID_STATE_IDLE;

/* function ------------------------------------------------------------------*/
static void usbd_hid_int_callback(uint8_t ep, uint32_t nbytes)
{
    hid_state = HID_STATE_IDLE;
}

/*!< endpoint call back */
static struct usbd_endpoint hid_keyboard_in_ep = {
    .ep_cb = usbd_hid_int_callback,
    .ep_addr = USBD_IF0_AL0_EP0_ADDR
};

static struct usbd_endpoint hid_mouse_in_ep = {
    .ep_cb = usbd_hid_int_callback,
    .ep_addr = USBD_IF1_AL0_EP0_ADDR
};

static struct usbd_endpoint hid_vol_in_ep = {
    .ep_cb = usbd_hid_int_callback,
    .ep_addr = USBD_IF2_AL0_EP0_ADDR
};

struct usbd_interface intf0;
struct usbd_interface intf1;
struct usbd_interface intf2;
struct usbd_interface intf3;

void usb_device_init(void)
{
    usbd_desc_register(usbd_descriptor);
    usbd_add_interface(usbd_hid_init_intf(&intf0, usbd_hid_0_report_descriptor, USBD_IF0_AL0_HID_REPORT_DESC_SIZE));
    usbd_add_endpoint(&hid_keyboard_in_ep);
    usbd_add_interface(usbd_hid_init_intf(&intf1, usbd_hid_1_report_descriptor, USBD_IF1_AL0_HID_REPORT_DESC_SIZE));
    usbd_add_endpoint(&hid_mouse_in_ep);
    usbd_add_interface(usbd_hid_init_intf(&intf2, usbd_hid_2_report_descriptor, USBD_IF2_AL0_HID_REPORT_DESC_SIZE));
    usbd_add_endpoint(&hid_vol_in_ep);
    usbd_add_interface(usbd_msc_init_intf(&intf3, USBD_IF3_AL0_EP0_ADDR, USBD_IF3_AL0_EP1_ADDR));

    usbd_initialize();
}

#include "HAL.h"

// Task id
tmosTaskID usbTaskID = INVALID_TASK_ID;

/*--------------------------- define for hid ---------------------------*/

void usbd_configure_done_callback(void)
{
    /* no out ep, do nothing */
}

void usbh_hid_set_idle(uint8_t intf, uint8_t report_id, uint8_t duration)
{

}

void usbh_hid_set_protocol(uint8_t intf, uint8_t protocol)
{

}

void usbh_hid_set_report(uint8_t intf, uint8_t report_id, uint8_t report_type, uint8_t *report, uint8_t report_len)
{
    g_CapsLock_LEDOn_Status.usb = (report[0] & 0x2) ? TRUE : FALSE;
}

void usb_suspend_wake_up_cb(uint8_t type)
{
    if (type) { // wake up
        tmos_start_task( halTaskID, USB_READY_EVENT, 20 );
    } else {
        g_Ready_Status.usb = FALSE;
        tmos_clear_event( halTaskID, USB_READY_EVENT );
    }
}

/*--------------------------- define for msc ---------------------------*/

#define BLOCK_SIZE  512

void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    *block_num = 64; //Pretend having so many buffer,not has actually.
    *block_size = BLOCK_SIZE;
}
int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length)
{
//    if (sector < 64) {
    EEPROM_READ(sector * BLOCK_SIZE, buffer, length);
//    }
    return 0;
}

int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length)
{
//    if (sector < 64) {
    EEPROM_ERASE(sector * BLOCK_SIZE, length);
    EEPROM_WRITE(sector * BLOCK_SIZE, buffer, length);
//    }
    return 0;
}

#if 0
void usbh_hid_set_report(uint8_t intf, uint8_t report_id, uint8_t report_type, uint8_t *report, uint8_t report_len)
{
    g_CapsLock_LEDOn_Status.usb = (report[0] & (1<<1) ? TRUE : FALSE);
}

void hid_mouse_test(void)
{
    HIDMouse[1] = HIDMouse[2] = 5;

    while (hid_state == HID_STATE_BUSY) {}
    int ret = usbd_ep_start_write(USBD_IF1_AL0_EP0_ADDR, HIDMouse, HID_MOUSE_DATA_LENGTH);
    if (ret >= 0) {
        hid_state = HID_STATE_BUSY;
    }
}

void hid_keyboard_test(void)
{
    HIDKeyboard[2] = HID_KBD_USAGE_A;

    while (hid_state == HID_STATE_BUSY) {}
    int ret = usbd_ep_start_write(USBD_IF0_AL0_EP0_ADDR, HIDKeyboard, HID_KEYBOARD_DATA_LENGTH);
    if (ret >= 0) {
        hid_state = HID_STATE_BUSY;
    }

    while (hid_state == HID_STATE_BUSY) {}
    DelayMs(30);
    HIDKeyboard[2] = 0;
    ret = usbd_ep_start_write(USBD_IF0_AL0_EP0_ADDR, HIDKeyboard, HID_KEYBOARD_DATA_LENGTH);
    if (ret >= 0) {
        hid_state = HID_STATE_BUSY;
    }
}
#endif

/*******************************************************************************
* Function Name  : USB_ProcessEvent
* Description    : USB处理事件
* Input          : task_id： 任务id, events: USB事件
* Return         : tmosEvents
*******************************************************************************/
tmosEvents USB_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  int ret;

  if ( events & START_USB_EVENT )
  {
    PFIC_EnableIRQ( USB_IRQn );
    return events ^ START_USB_EVENT;
  }

  if ( events & USB_KEYBOARD_EVENT )
  {
    if (hid_state != HID_STATE_BUSY) {
      ret = usbd_ep_start_write(USBD_IF0_AL0_EP0_ADDR, HIDKeyboard, HID_KEYBOARD_DATA_LENGTH);
      if (ret >= 0) {
          hid_state = HID_STATE_BUSY;
      }
    }
    return events ^ USB_KEYBOARD_EVENT;
  }

  if ( events & USB_MOUSE_EVENT )
  {
    if (hid_state != HID_STATE_BUSY) {
      ret = usbd_ep_start_write(USBD_IF1_AL0_EP0_ADDR, HIDMouse, HID_MOUSE_DATA_LENGTH);
      if (ret >= 0) {
          hid_state = HID_STATE_BUSY;
      }
    }
    return events ^ USB_MOUSE_EVENT;
  }

  if ( events & USB_VOL_EVENT )
  {
    if (hid_state != HID_STATE_BUSY) {
      ret = usbd_ep_start_write(USBD_IF2_AL0_EP0_ADDR, HIDVolume, HID_VOLUME_DATA_LENGTH);
      if (ret >= 0) {
          hid_state = HID_STATE_BUSY;
      }
    }
    return events ^ USB_VOL_EVENT;
  }

  if ( events & USB_TEST_EVENT )
  {
#if 0
    hid_keyboard_test();
    hid_mouse_test();
#endif
    tmos_start_task(usbTaskID, USB_TEST_EVENT, MS1_TO_SYSTEM_TIME(500));
    return events ^ USB_TEST_EVENT;
  }

  return 0;
}

/*******************************************************************************
* Function Name  : HAL_USBInit
* Description    : USB初始化
* Input          : None
* Return         : None
*******************************************************************************/
void HAL_USBInit( void )
{
  usbTaskID = TMOS_ProcessEventRegister( USB_ProcessEvent );
  /* support to cherry usb */
  TMR0_TimerInit(FREQ_SYS / 1000);
  TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
  PFIC_EnableIRQ(TMR0_IRQn);
  PFIC_SetPriority(TMR0_IRQn, 20);
  /* usb device init */
  usb_device_init();
  while (!usb_device_is_configured());
}

void usb_dc_low_level_init(void)
{
  extern void USB_IRQHandler(void);
  PFIC_EnableIRQ(USB_IRQn);
  PFIC_EnableFastINT0(USB_IRQn, (uint32_t)(void *)USB_IRQHandler);
}

void usb_hc_low_level_init(void)
{
  PFIC_EnableIRQ(USB2_IRQn);
}

volatile uint32_t timer_count_user = 0;

__INTERRUPT
__HIGH_CODE
void TMR0_IRQHandler(void)
{
  /*!< Timer 0 IRQ */
  if (TMR0_GetITFlag(TMR0_3_IT_CYC_END)) {
    /*!< Clear Pending flag */
    TMR0_ClearITFlag(TMR0_3_IT_CYC_END);

    /*!< Updata the ms count */
    timer_count_user++;
    /*!< Set timing time 1ms */
    R32_TMR0_CNT_END = GetSysClock() / 1000;
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;

    /*!< Enable interrupt */
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
  }
}

uint32_t chey_board_millis(void)
{
  return timer_count_user;
}
