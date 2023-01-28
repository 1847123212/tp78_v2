/********************************** (C) COPYRIGHT *******************************
* File Name          : HAL.h
* Author             : ChnMasterOG, WCH
* Version            : V1.1
* Date               : 2022/11/13
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
#include "MSG_CP.h"
#include "HW_I2C.h"
#include "I2C_TP.h"
#include "MPR121.h"
#include "OLED.h"
#include "OLED_UI.h"
#include "RF_PHY.h"
#include "KEYBOARD.h"
#include "BATTERY.h"
#include "WS2812.h"
#include "CORE.h"

/* hal task Event */
#define LED_BLINK_EVENT                     0x0001
#define KEY_EVENT                           0x0002
#define MAIN_CIRCULATION_EVENT              0x0004
#define BATTERY_EVENT                       0x0008
#define WS2812_EVENT                        0x0010
#define OLED_UI_EVENT                       0x0020
#define USB_READY_EVENT                     0x0040
#define MPR121_EVENT                        0x0080
#define SYST_EVENT                          0x0100
#define CP_INITIAL_EVENT                    0x0200
#define HAL_REG_INIT_EVENT                  0x2000
#define HAL_TEST_EVENT                      0x4000

// hal sys_message
#define MESSAGE_UART                        0xA0    // UART message
#define UART0_MESSAGE                       (MESSAGE_UART|0 )    // UART0 message
#define UART1_MESSAGE                       (MESSAGE_UART|1 )    // UART1 message

#define USB_MESSAGE              		        0xB0    // USB message

#define HID_MOUSE_DATA_LENGTH               4
#define HID_KEYBOARD_DATA_LENGTH            8
#define HID_VOLUME_DATA_LENGTH              1
#define HID_DATA_LENGTH                     16

#define FLASH_ADDR_CustomKey                (8*1024)      // 从8K地址开始存放键盘布局，map空余空间：0x4~0x210C
#define FLASH_ADDR_Extra_CustomKey          (9*1024)      // 从9K地址开始存放键盘额外布局
#define FLASH_ADDR_LEDStyle                 (10*1024)     // 背光样式
#define FLASH_ADDR_BLEDevice                (10*1024+4)   // 蓝牙默认连接设备编号
#define FLASH_ADDR_RForBLE                  (10*1024+8)   // 启动默认RF模式或者BLE模式
#define FLASH_ADDR_MPR121_ALG_Param         (10*1024+12)  // MPR121算法参数存储

#define IDLE_MAX_PERIOD                     240           // idle_cnt大于该值则进入屏保，单位为500ms
#define LP_MAX_PERIOD                       480           // idle_cnt大于该值则进入低功耗模式，单位为500ms

#define MOTOR_PIN                           GPIO_Pin_18
#define MOTOR_RUN()                         { GPIOB_SetBits( MOTOR_PIN ); }
#define MOTOR_STOP()                        { GPIOB_ResetBits( MOTOR_PIN ); }
#define MOTOR_Init()                        { GPIOB_SetBits( MOTOR_PIN ); GPIOB_ModeCfg( MOTOR_PIN, GPIO_ModeOut_PP_5mA ); GPIOB_ResetBits( MOTOR_PIN ); }

typedef struct HID_ProcessFunc
{
    void (*keyboard_func)();  // 按键处理函数
    void (*mouse_func)();     // 鼠标处理函数
    void (*volume_func)();    // 音量处理函数
}HID_ProcessFunc_s;

typedef struct HW_ProcessFunc
{
    void (*battery_func)();   // 电量处理函数
    void (*ws2812_func)();    // 背光LED处理函数
    void (*msgcp_func)();     // I2C核间通信处理函数
    void (*touchbar_func)();  // 触摸条处理函数
}HW_ProcessFunc_s;

typedef struct SW_ProcessFunc
{
    void (*paintedegg_func)();        // 彩蛋处理函数
    void (*oled_capslock_func)();     // 大小写状态OLED处理函数
    void (*oled_UBstatus_func)();     // USB或BLE/RF状态OLED处理函数
}SW_ProcessFunc_s;

typedef struct tag_uart_package
{
  tmos_event_hdr_t hdr;
  uint8            *pData;
} uartPacket_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern UINT8 HID_DATA[HID_DATA_LENGTH];
extern UINT8* HIDMouse;
extern UINT8* HIDKey;
extern UINT8* HIDVolume;

extern BOOL CP_Ready;
extern BOOL enable_BLE;
extern BOOL priority_USB;
extern tmosTaskID halTaskID;
extern BOOL USB_CapsLock_LEDOn, BLE_CapsLock_LEDOn, RF_CapsLock_LEDOn;
extern HID_ProcessFunc_s HID_ProcessFunc_v;
extern SW_ProcessFunc_s SW_ProcessFunc_v;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */
extern void HAL_Init( void );
extern tmosEvents HAL_ProcessEvent( tmosTaskID task_id, tmosEvents events );
extern void CH58X_BLEInit( void );
extern uint16 HAL_GetInterTempValue( void );
extern void Lib_Calibration_LSI( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
