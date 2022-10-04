/********************************** (C) COPYRIGHT *******************************
* File Name          : HAL.h
* Author             : WCH
* Version            : V1.0
* Date               : 2016/05/05
* Description        : 
*******************************************************************************/



/******************************************************************************/
#ifndef __HAL_H
#define __HAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "RTC.h"
#include "SLEEP.h"	
#include "LED.h"
#include "KEY.h"
#include "USB.h"
#include "PS2.h"
#include "BLE.h"
#include "KEYBOARD.h"
#include "BATTERY.h"
#include "OLED.h"
#include "WS2812.h"
#include "ISP.h"

/* hal task Event */
#define   LED_BLINK_EVENT                               0x0001
#define   KEY_EVENT			                                0x0002
#define   MAIN_CIRCULATION_EVENT                        0x0004
#define   BATTERY_EVENT                                 0x0008
#define   WS2812_EVENT                                  0x0010
#define   OLED_EVENT                                    0x0020
#define   HAL_REG_INIT_EVENT		                        0x2000
#define   HAL_TEST_EVENT		                            0x4000

// hal sys_message
#define MESSAGE_UART                0xA0    // UART message
#define UART0_MESSAGE             (MESSAGE_UART|0 )    // UART0 message
#define UART1_MESSAGE             (MESSAGE_UART|1 )    // UART1 message

#define USB_MESSAGE              		0xB0    // USB message

#define FLASH_ADDR_CustomKey                (8*1024)  // ��8K��ַ��ʼ��ż��̲���
#define FLASH_ADDR_Extra_CustomKey          (9*1024)  // ��9K��ַ��ʼ��ż��̶��Ⲽ��
#define FLASH_ADDR_LEDStyle                 (10*1024)
#define FLASH_ADDR_BLEDevice                (10*1024+1)

typedef struct HID_ProcessFunc
{
    void (*keyboard_func)();  // ����������
    void (*mouse_func)();     // ��괦����
    void (*volume_func)();    // ����������
}HID_ProcessFunc_s;

typedef struct HW_ProcessFunc
{
    void (*battery_func)();   // ����������
    void (*ws2812_func)();    // ����LED������
}HW_ProcessFunc_s;

typedef struct SW_ProcessFunc
{
    void (*paintedegg_func)();        // �ʵ�������
    void (*oled_capslock_func)();     // ��Сд״̬OLED������
    void (*oled_UBstatus_func)();     // USB��BLE״̬OLED������
}SW_ProcessFunc_s;

typedef struct tag_uart_package
{
  tmos_event_hdr_t hdr;
  uint8            *pData;
} uartPacket_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern BOOL enable_BLE;
extern BOOL priority_USB;
extern tmosTaskID halTaskID;
extern BOOL USB_CapsLock_LEDOn, BLE_CapsLock_LEDOn;
extern HID_ProcessFunc_s HID_ProcessFunc_v;
extern SW_ProcessFunc_s SW_ProcessFunc_v;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */
extern void HAL_Init( void );
extern tmosEvents HAL_ProcessEvent( tmosTaskID task_id, tmosEvents events );
extern void CH57X_BLEInit( void );
extern uint16 HAL_GetInterTempValue( void );
extern void Lib_Calibration_LSI( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
