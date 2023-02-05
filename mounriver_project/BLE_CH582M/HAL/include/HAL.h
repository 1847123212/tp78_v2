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

#include "CH58x_common.h"

//#define FIRST_USED        // ��������

/* HID data format */
typedef union {
    struct {
        unsigned char LeftBtn : 1;
        unsigned char RightBtn : 1;
        unsigned char MiddleBtn : 1;
        unsigned char Always1 : 1;
        unsigned char Xsignbit : 1;
        unsigned char Xspinbit : 1;
        unsigned char Xoverflow : 1;
        unsigned char Yoverflow : 1;
        signed char XMovement : 8;
        signed char YMovement : 8;
        signed char ZMovement : 8;
    };
    uint8_t data[4];
}Mouse_Data_t;

#include "RTC.h"
#include "SLEEP.h"	
#include "LED.h"
#include "KEY.h"
#include "FATFS.h"
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

/* hal sys_message */
#define MESSAGE_UART                        0xA0    // UART message
#define UART0_MESSAGE                       (MESSAGE_UART|0 )    // UART0 message
#define UART1_MESSAGE                       (MESSAGE_UART|1 )    // UART1 message

#define USB_MESSAGE              		        0xB0    // USB message

#define HID_MOUSE_DATA_LENGTH               4
#define HID_KEYBOARD_DATA_LENGTH            8
#define HID_VOLUME_DATA_LENGTH              1
#define HID_DATA_LENGTH                     16

/* CodeFlash ����ַ0x00000 */
/* 0x0000~0x4FFFF Ԥ�������� */
#define CODEFLASH_ADDR_START                (0x50000)
#define CODEFLASH_LENGTH                    (0x18000)     // ������ַ0x67FFF(Ԥ��0x7FFF)

/* DataFlash ����ַ0x70000 */
#if 0    // for old version
#define DATAFLASH_ADDR_CustomKey            (8*1024)      // ��8K��ַ��ʼ��ż��̲��֣�map����ռ䣺0x4~0x210C
#define DATAFLASH_ADDR_Extra_CustomKey      (9*1024)      // ��9K��ַ��ʼ��ż��̶��Ⲽ��
#define DATAFLASH_ADDR_LEDStyle             (10*1024)     // ������ʽ
#define DATAFLASH_ADDR_BLEDevice            (10*1024+4)   // ����Ĭ�������豸���
#define DATAFLASH_ADDR_RForBLE              (10*1024+8)   // ����Ĭ��RFģʽ����BLEģʽ
#define DATAFLASH_ADDR_MPR121_ALG_Param     (10*1024+12)  // MPR121�㷨�����洢
#endif

#define IDLE_MAX_PERIOD                     240           // idle_cnt���ڸ�ֵ�������������λΪ500ms
#define LP_MAX_PERIOD                       480           // idle_cnt���ڸ�ֵ�����͹���ģʽ����λΪ500ms

#define MOTOR_PIN                           GPIO_Pin_19
#define MOTOR_RUN()                         { GPIOB_SetBits( MOTOR_PIN ); }
#define MOTOR_STOP()                        { GPIOB_ResetBits( MOTOR_PIN ); }
#define MOTOR_Init()                        { GPIOB_SetBits( MOTOR_PIN ); GPIOB_ModeCfg( MOTOR_PIN, GPIO_ModeOut_PP_5mA ); GPIOB_ResetBits( MOTOR_PIN ); }

/* CapsLock LEDOn Status */
#define USB_CAPSLOCK_LEDON_BIT              0x1
#define BLE_CAPSLOCK_LEDON_BIT              0x2
#define RF_CAPSLOCK_LEDON_BIT               0x4
#define UI_CAPSLOCK_LEDON_BIT               0x80

typedef struct tag_uart_package
{
  tmos_event_hdr_t hdr;
  uint8            *pData;
} uartPacket_t;

typedef struct _CapsLock_LEDOn_Status_t
{
    uint8_t usb : 1;
    uint8_t ble : 1;
    uint8_t rf : 1;
    uint8_t reserved : 4;
    uint8_t ui : 1;
}CapsLock_LEDOn_Status_t;

typedef struct _Ready_Status_t
{
    uint8_t usb : 1;
    uint8_t ble : 1;
    uint8_t rf : 1;
    uint8_t usb_l : 1;
    uint8_t ble_l : 1;
    uint8_t rf_l : 1;
    uint8_t cp : 1;
    uint8_t fatfs : 1;
}Ready_Status_t;

typedef struct _Enable_Status_t
{
    uint8_t ble : 1;
    uint8_t motor : 1;
    uint8_t tp : 1;
    uint8_t reserved : 5;
}Enable_Status_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern UINT8 HID_DATA[HID_DATA_LENGTH];
extern UINT8* HIDMouse;
extern UINT8* HIDKeyboard;
extern UINT8* HIDVolume;

extern BOOL priority_USB;
extern tmosTaskID halTaskID;

extern CapsLock_LEDOn_Status_t g_CapsLock_LEDOn_Status;
extern Ready_Status_t g_Ready_Status;
extern Enable_Status_t g_Enable_Status;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

void HID_KEYBOARD_Process( void );
void HID_PS2TP_Process( void );
void HID_I2CTP_Process( void );
void HID_CapMouse_Process( void );
void HID_VOL_Process( void );
void SW_PaintedEgg_Process( void );
void SW_OLED_Capslock_Process( void );
void SW_OLED_UBStatus_Process( void );
void HW_Battery_Process( void );
void HW_WS2812_Process( void );
void HW_MSG_CP_Process( void );
void HW_TouchBar_Process( void );

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
