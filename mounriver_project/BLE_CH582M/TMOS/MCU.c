/********************************** (C) COPYRIGHT *******************************
 * File Name          : MCU.c
 * Author             : ChnMasterOG, WCH
 * Version            : V1.2
 * Date               : 2022/1/26
 * Description        : 硬件任务处理函数及BLE和硬件初始化
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CH58x_common.h"
#include "HAL.h"
#include <string.h>

/* 彩蛋 */
#include "snake.h"

/*HID data*/
UINT8 HID_DATA[HID_DATA_LENGTH] = { 0x0, 0x0, 0x0, 0x0, 0x0,
                                    0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                                    0x2, 0x0 }; // bit1~bit4: mouse data, bit6~bit13: key data, bit15: vol data
/*鼠标键盘数据*/
UINT8* HIDMouse = &HID_DATA[1];
UINT8* HIDKey = &HID_DATA[6];
/*音量控制数据*/
UINT8* HIDVolume = &HID_DATA[15];

tmosTaskID halTaskID = INVALID_TASK_ID;
BOOL USB_CapsLock_LEDOn = FALSE, BLE_CapsLock_LEDOn = FALSE, RF_CapsLock_LEDOn = FALSE; // USB/BLE/RF大小写信号
BOOL priority_USB = TRUE;   // USB和蓝牙/RF同时连接选择
BOOL enable_BLE = TRUE;   // 使能或失能蓝牙
BOOL CP_Ready = FALSE;    // CP就绪信号

static BOOL CapsLock_LEDOn_state = FALSE; // Caps Lock LED ON/OFF
static BOOL connection_status[3] = { FALSE, FALSE, FALSE };  // USB/BLE/RF status
static BOOL motor_status = FALSE;  // motor status
static uint32_t EP_counter = 0;  // 彩蛋计数器
static uint32_t idle_cnt = 0;    // 无有效操作计数值，idle_cnt大于阈值则进入休眠

/*******************************************************************************
* Function Name  : TP78_Idle_Clr
* Description    : 清除idle
* Input          : 无
* Return         : 无
*******************************************************************************/
static void TP78_Idle_Clr(void)
{
  if (idle_cnt >= IDLE_MAX_PERIOD) {  // 退出idle
#ifdef HAL_OLED
    OLED_UI_idle(0);
#endif
    idle_cnt = 0;
  }
}

/*******************************************************************************
* Function Name  : HID_KEYBOARD_Process
* Description    : 主循环键盘处理函数
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void HID_KEYBOARD_Process(void)
{
  uint8_t res;
  KEYBOARD_Detection();
  if (KEYBOARD_data_ready != 0) {    // 产生键盘数据
    KEYBOARD_data_ready = 0;
    TP78_Idle_Clr();
    if ( EnterPasskey_flag == TRUE ) { // 处理输入配对密码
      res = KEYBOARD_EnterPasskey( &BLE_Passkey );
      if ( res == 0 ) {
        EnterPasskey_flag = FALSE;
        tmos_start_task( hidEmuTaskId, START_SEND_PASSKEY_EVT, 400 );
      }
    }
    else {
      if ( KEYBOARD_Custom_Function() ) { // 带有Fn键处理信息则不产生键盘事件
        if ( USB_Ready == TRUE && priority_USB == TRUE ) {
          tmos_set_event( usbTaskID, USB_KEYBOARD_EVENT );  // USB键盘事件
        } else if ( BLE_Ready == TRUE ) {
          tmos_set_event( hidEmuTaskId, START_KEYBOARD_REPORT_EVT );  // 蓝牙键盘事件
        } else if ( RF_Ready == TRUE ) {
          tmos_set_event( RFTaskId, SBP_RF_KEYBOARD_REPORT_EVT );  // RF键盘事件
        }
      }
      if (KEYBOARD_mouse_ready != 0) { // 发送键盘鼠标数据
        KEYBOARD_mouse_ready = 0;
        tmos_memset(&HIDMouse[1], 0, 3);   // 只按左中右键没有其他操作
        if ( USB_Ready == TRUE && priority_USB == TRUE ) {
          tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
        } else if ( BLE_Ready == TRUE ) {
          tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
        } else if ( RF_Ready == TRUE ) {
          tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF键盘事件
        }
      }
    }
  }
}

/*******************************************************************************
* Function Name  : HID_PS2TP_Process
* Description    : PS2小红点处理函数
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void HID_PS2TP_Process(void)
{
  if (PS2_data_ready != 0 && enable_TP == TRUE) {    // 发送小红点鼠标数据
    PS2_data_ready = 0;
    TP78_Idle_Clr();
    if ( PS2_byte_cnt == 3 ) {  // 接收完数据报
      PS2_byte_cnt = 0;
      HIDMouse[2] = -HIDMouse[2]; // 反转Y轴
      if ( USB_Ready == TRUE && priority_USB == TRUE ) {
        tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
      } else if ( BLE_Ready == TRUE ) {
        tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
      } else if ( RF_Ready == TRUE ) {
        tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF键盘事件
      }
    }
    PS2_En_Data_Report();
  }
}

/*******************************************************************************
* Function Name  : HID_I2CTP_Process
* Description    : I2C小红点处理函数
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void HID_I2CTP_Process(void)
{
  if (I2C_TP_data_ready != 0 && enable_TP == TRUE) {    // 发送小红点鼠标数据
    I2C_TP_data_ready = 0;
    TP78_Idle_Clr();
    if (I2C_TP_ReadPacket() == 0) { // 正常接受完数据包
      if ( USB_Ready == TRUE && priority_USB == TRUE ) {
        tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
      } else if ( BLE_Ready == TRUE ) {
        tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
      } else if ( RF_Ready == TRUE ) {
        tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF键盘事件
      }
    } else {
      OLED_UI_add_SHOWINFO_task("TPdat ER");
      OLED_UI_add_CANCELINFO_delay_task(3000);
    }
  }
}

/*******************************************************************************
* Function Name  : HID_CapMouse_Process
* Description    : MPR121触摸板处理函数
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void HID_CapMouse_Process(void)
{
  if (cap_mouse_data_change) {
    cap_mouse_data_change = 0;
    TP78_Idle_Clr();
    if ( USB_Ready == TRUE && priority_USB == TRUE ) {
      tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  // USB鼠标事件
    } else if ( BLE_Ready == TRUE ) {
      tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
    } else if ( RF_Ready == TRUE ) {
      tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF键盘事件
    }
  }
}

/*******************************************************************************
* Function Name  : HID_VOL_Process
* Description    : 音量控制处理函数
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void HID_VOL_Process(void)
{
  TP78_Idle_Clr();
  if ( USB_Ready == TRUE && priority_USB == TRUE ) {
    tmos_set_event( usbTaskID, USB_VOL_EVENT );  //USB音量事件
  } else if ( BLE_Ready == TRUE ) {
    //tmos_set_event( hidEmuTaskId, START_VOL_REPORT_EVT );  //蓝牙音量事件
  } else if ( RF_Ready == TRUE ) {
    tmos_set_event( RFTaskId, SBP_RF_VOL_REPORT_EVT );  // RF键盘事件
  }
}

/*******************************************************************************
* Function Name  : SW_PaintedEgg_Process
* Description    : 彩蛋软件处理函数
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void SW_PaintedEgg_Process(void)
{
  KEYBOARD_Detection();
  if (KEYBOARD_data_ready != 0) { // 产生键盘事件
    TP78_Idle_Clr();
    KEYBOARD_data_ready = 0;
    if (KEYBOARD_Custom_Function() != 0) {
      switch (Keyboarddat->Key1) {
        case KEY_W: BodyDir[0] = DirUp; break;
        case KEY_S: BodyDir[0] = DirDown; break;
        case KEY_A: BodyDir[0] = DirLeft; break;
        case KEY_D: BodyDir[0] = DirRight; break;
      }
    }
  }
  if (++EP_counter > 50) {  // 50次事件更新一次(控制snake速度)
    EP_counter = 0;
    MoveSnake();
  }
}

/*******************************************************************************
* Function Name  : SW_OLED_Capslock_Process
* Description    : 大小写状态OLED处理函数(检测LED状态：更新CapsLock指示)
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void SW_OLED_Capslock_Process(void)
{
  if ( USB_Ready && priority_USB && CapsLock_LEDOn_state != USB_CapsLock_LEDOn ) {
    CapsLock_LEDOn_state = USB_CapsLock_LEDOn;
#ifdef OLED_0_91
    OLED_UI_ShowCapslock(56, 1, CapsLock_LEDOn_state);
#endif
  } else if ( BLE_Ready && !priority_USB && CapsLock_LEDOn_state != BLE_CapsLock_LEDOn ) {
    CapsLock_LEDOn_state = BLE_CapsLock_LEDOn;
#ifdef OLED_0_91
    OLED_UI_ShowCapslock(56, 1, CapsLock_LEDOn_state);
#endif
  } else if ( RF_Ready && !priority_USB && CapsLock_LEDOn_state != RF_CapsLock_LEDOn ) {
    CapsLock_LEDOn_state = RF_CapsLock_LEDOn;
#ifdef OLED_0_91
    OLED_UI_ShowCapslock(56, 1, CapsLock_LEDOn_state);
#endif
  } else {
    return;
  }
#ifdef OLED_0_66
  if (CapsLock_LEDOn_state) {
    OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_CAPSLOCK_IDX]);
  } else {
    OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_CAPSLOCK_IDX]);
  }
  OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
}

/*******************************************************************************
* Function Name  : SW_OLED_UBStatus_Process
* Description    : USB或BLE/RF状态OLED处理函数(检测USB/蓝牙/RF连接状态：连接状态改变OLED显示)
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void SW_OLED_UBStatus_Process(void)
{
  if (connection_status[0] != USB_Ready) {
    TP78_Idle_Clr();
    connection_status[0] = USB_Ready;
    if ( USB_Ready ) {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(8, 0, "USB");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(8, 0, "   ");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
#endif
    }
    if ( USB_Ready ^ BLE_Ready ) priority_USB = USB_Ready;
    // 同时出现USB和蓝牙/RF时进行显示
    if ( USB_Ready && (BLE_Ready || RF_Ready) ) {
#ifdef OLED_0_91
      OLED_UI_ShowOK(26 + !priority_USB * 30, 0, TRUE);
#endif
#ifdef OLED_0_66
      if ( priority_USB ) {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
      } else if ( BLE_Ready ) {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]]);
      } else {  // RF_Ready
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX]);
      }
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_ShowOK(26, 0, FALSE);
      OLED_UI_ShowOK(56, 0, FALSE);
#endif
    }
#ifdef OLED_0_66
    OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
  } else if (connection_status[1] != BLE_Ready) {
    TP78_Idle_Clr();
    connection_status[1] = BLE_Ready;
//    HalLedSet(HAL_LED_1, BLE_Ready);
    if ( BLE_Ready ) {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(38, 0, "BLE");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]]);
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(38, 0, "   ");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]]);
#endif
      if ( EnterPasskey_flag == TRUE ) {  // 打断输入配对状态
        EnterPasskey_flag = FALSE;
        BLE_Passkey = 0;
//        OLED_Set_Scroll_ENA(1);
        OLED_UI_add_SHOWINFO_task("Close!");
        OLED_UI_add_CANCELINFO_delay_task(2000);
      }
    }
    if ( USB_Ready ^ BLE_Ready ) priority_USB = USB_Ready;
    // 同时出现USB和蓝牙时进行显示
    if ( USB_Ready && BLE_Ready ) {
#ifdef OLED_0_91
      OLED_UI_ShowOK(26 + !priority_USB * 30, 0, TRUE);
#endif
#ifdef OLED_0_66
      if ( priority_USB ) {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
      } else {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_BLE_UNCONNECT_IDX + DeviceAddress[5]]);
      }
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_ShowOK(26, 0, FALSE);
      OLED_UI_ShowOK(56, 0, FALSE);
#endif
    }
#ifdef OLED_0_66
    OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
  } else if (connection_status[2] != RF_Ready) {
    TP78_Idle_Clr();
    connection_status[2] = RF_Ready;
    if ( RF_Ready ) {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(41, 0, "RF");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_add((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX]);
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_add_SHOWSTRING_task(41, 0, "  ");
#endif
#ifdef OLED_0_66
      OLED_UI_slot_delete((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX]);
#endif
    }
    if ( USB_Ready ^ RF_Ready ) priority_USB = USB_Ready;
    // 同时出现USB和RF时进行显示
    if ( USB_Ready && RF_Ready ) {
#ifdef OLED_0_91
      OLED_UI_ShowOK(26 + !priority_USB * 30, 0, TRUE);
#endif
#ifdef OLED_0_66
      if ( priority_USB ) {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_USB_IDX]);
      } else {
        OLED_UI_slot_active((uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX],
                            (uint8_t*)UI_Slot_Icon[OLED_UI_ICON_RF_IDX]);
      }
#endif
    } else {
#ifdef OLED_0_91
      OLED_UI_ShowOK(26, 0, FALSE);
      OLED_UI_ShowOK(56, 0, FALSE);
#endif
    }
#ifdef OLED_0_66
    OLED_UI_add_default_task(OLED_UI_FLAG_DRAW_SLOT);
#endif
  }
}

/*******************************************************************************
* Function Name  : HW_Battery_Process
* Description    : 电池电量处理函数
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void HW_Battery_Process(void)
{
  BATTERY_ADC_Calculation( );
#if (defined HAL_OLED)  && (HAL_OLED == TRUE)
  if ( EnterPasskey_flag == FALSE ) { // 绘制电池
    BATTERY_DrawBMP( );
  }
  if ( BAT_chrg != BAT_IS_CHARGING ) {  // 判断充电信号
    BAT_chrg = BAT_IS_CHARGING;
    if ( BAT_chrg ) OLED_UI_add_default_task(OLED_UI_FLAG_BAT_CHARGE);
    else OLED_UI_add_default_task(OLED_UI_FLAG_BAT_CLR_CHARGE);
  }
#endif
  BATTERY_DMA_ENABLE( );  // DMA使能
}

/*******************************************************************************
* Function Name  : HW_WS2812_Process
* Description    : 按键背光处理函数
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void HW_WS2812_Process(void)
{
  if (USB_Ready == TRUE) {
    WS2812_Send( );
  }
}

/*******************************************************************************
* Function Name  : HW_MSG_CP_Process
* Description    : I2C核间通信
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void HW_MSG_CP_Process(void)
{
  uint8_t dat, err;
  err = CP_I2C_RD_Reg(CP_CHANGE_REG, &dat);
  /* mouse data is changed */
  if (err == 0 && dat != 0) {
    CP_I2C_WR_Reg(CP_CHANGE_REG, 0);
    CP_I2C_Muti_RD_Reg(CP_MOUSEDATA_REG, HIDMouse, HID_MOUSE_DATA_LENGTH);
#ifdef DEBUG
    OLED_UI_add_SHOWINFO_task("%d %d", HIDMouse[1], HIDMouse[2]); //debug
#endif
    if ( USB_Ready == TRUE && priority_USB == TRUE ) {
      tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
    } else if ( BLE_Ready == TRUE ) {
      tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
    } else if ( RF_Ready == TRUE ) {
      tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF键盘事件
    }
  }
}

/*******************************************************************************
* Function Name  : HW_TouchBar_Process
* Description    : 触摸条处理
* Input          : 无
* Return         : 无
*******************************************************************************/
__attribute__((weak)) void HW_TouchBar_Process(void)
{
  if (touchbar_data_change) {
    touchbar_data_change = 0;
    if ( USB_Ready == TRUE && priority_USB == TRUE ) {
      tmos_set_event( usbTaskID, USB_MOUSE_EVENT );  //USB鼠标事件
    } else if ( BLE_Ready == TRUE ) {
      tmos_set_event( hidEmuTaskId, START_MOUSE_REPORT_EVT );  //蓝牙鼠标事件
    } else if ( RF_Ready == TRUE ) {
      tmos_set_event( RFTaskId, SBP_RF_MOUSE_REPORT_EVT );  // RF键盘事件
    }
  }
}

/*******************************************************************************
 *  处理函数注册
 *******************************************************************************/
HID_ProcessFunc_s HID_ProcessFunc_v = {
  .keyboard_func = HID_KEYBOARD_Process,
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
  .mouse_func = HID_PS2TP_Process,
#elif (defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)
  .mouse_func = HID_I2CTP_Process,
#else  // default MPR121 cap mouse
  .mouse_func = HID_CapMouse_Process,
#endif
  .volume_func = HID_VOL_Process,
};
SW_ProcessFunc_s SW_ProcessFunc_v = {
  .paintedegg_func = SW_PaintedEgg_Process,
  .oled_capslock_func = SW_OLED_Capslock_Process,
  .oled_UBstatus_func = SW_OLED_UBStatus_Process,
};
HW_ProcessFunc_s HW_ProcessFunc_v = {
  .battery_func = HW_Battery_Process,
  .ws2812_func = HW_WS2812_Process,
  .msgcp_func = HW_MSG_CP_Process,
  .touchbar_func = HW_TouchBar_Process,
};

/*******************************************************************************
 * @fn          Lib_Calibration_LSI
 *
 * @brief       内部32k校准
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void Lib_Calibration_LSI( void )
{
  Calibration_LSI( Level_128 ); // Level_64
}

#if (defined (BLE_SNV)) && (BLE_SNV == TRUE)
/*******************************************************************************
 * @fn          Lib_Read_Flash
 *
 * @brief       Lib 操作Flash回调
 *
 * input parameters
 *
 * @param       addr.
 * @param       num.
 * @param       pBuf.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
u32 Lib_Read_Flash( u32 addr, u32 num, u32 *pBuf )
{
  EEPROM_READ( addr, pBuf, num * 4 );
  return 0;
}

/*******************************************************************************
 * @fn          Lib_Write_Flash
 *
 * @brief       Lib 操作Flash回调
 *
 * input parameters
 *
 * @param       addr.
 * @param       num.
 * @param       pBuf.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
u32 Lib_Write_Flash( u32 addr, u32 num, u32 *pBuf )
{
  EEPROM_ERASE( addr, EEPROM_PAGE_SIZE*2 );
  EEPROM_WRITE( addr, pBuf, num * 4 );
  return 0;
}
#endif

/*******************************************************************************
 * @fn          CH58X_BLEInit
 *
 * @brief       BLE 库初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void CH58X_BLEInit(void)
{
    uint8_t     i;
    bleConfig_t cfg;
    if(tmos_memcmp(VER_LIB, VER_FILE, strlen(VER_FILE)) == FALSE)
    {
        PRINT("head file error...\n");
        while(1);
    }
    SysTick_Config(SysTick_LOAD_RELOAD_Msk);
    PFIC_DisableIRQ(SysTick_IRQn);

    tmos_memset(&cfg, 0, sizeof(bleConfig_t));
    cfg.MEMAddr = (uint32_t)MEM_BUF;
    cfg.MEMLen = (uint32_t)BLE_MEMHEAP_SIZE;
    cfg.BufMaxLen = (uint32_t)BLE_BUFF_MAX_LEN;
    cfg.BufNumber = (uint32_t)BLE_BUFF_NUM;
    cfg.TxNumEvent = (uint32_t)BLE_TX_NUM_EVENT;
    cfg.TxPower = (uint32_t)BLE_TX_POWER;
#if(defined(BLE_SNV)) && (BLE_SNV == TRUE)
    cfg.SNVAddr = (uint32_t)BLE_SNV_ADDR;
    cfg.readFlashCB = Lib_Read_Flash;
    cfg.writeFlashCB = Lib_Write_Flash;
#endif
#if(CLK_OSC32K)
    cfg.SelRTCClock = (uint32_t)CLK_OSC32K;
#endif
    cfg.ConnectNumber = (PERIPHERAL_MAX_CONNECTION & 3) | (CENTRAL_MAX_CONNECTION << 2);
    cfg.srandCB = SYS_GetSysTickCnt;
#if(defined TEM_SAMPLE) && (TEM_SAMPLE == TRUE)
    cfg.tsCB = HAL_GetInterTempValue; // 根据温度变化校准RF和内部RC( 大于7摄氏度 )
  #if(CLK_OSC32K)
    cfg.rcCB = Lib_Calibration_LSI; // 内部32K时钟校准
  #endif
#endif
#if(defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
    cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;
    cfg.sleepCB = CH58X_LowPower; // 启用睡眠
#endif
#if(defined(BLE_MAC)) && (BLE_MAC == TRUE)
    for(i = 0; i < 6; i++)
    {
        cfg.MacAddr[i] = MacAddr[5 - i];
    }
#else
    {
        uint8_t MacAddr[6];
        GetMACAddress(MacAddr);
        for(i = 0; i < 6; i++)
        {
            cfg.MacAddr[i] = MacAddr[i]; // 使用芯片mac地址
        }
    }
#endif
    if(!cfg.MEMAddr || cfg.MEMLen < 4 * 1024)
    {
        while(1);
    }
    i = BLE_LibInit(&cfg);
    if(i)
    {
        PRINT("LIB init error code: %x ...\n", i);
        while(1);
    }
}

/*******************************************************************************
 * @fn          HAL_ProcessEvent
 *
 * @brief       硬件层事务处理
 *
 * input parameters
 *
 * @param       task_id.
 * @param       events.
 *
 * output parameters
 *
 * @param       events.
 *
 * @return      None.
 */
tmosEvents HAL_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  uint8 * msgPtr;
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
  uint16_t dat16;
  static uint8_t collect_cnt = 0;
#endif
#if (defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)
  uint8_t i;
#endif
#if (defined MSG_CP) && (MSG_CP == TRUE)
  uint8_t dat, err;
#endif

  if ( events & SYS_EVENT_MSG )
  {    // 处理HAL层消息，调用tmos_msg_receive读取消息，处理完成后删除消息。
    msgPtr = tmos_msg_receive( task_id );
    if ( msgPtr )
    {
      /* De-allocate */
      tmos_msg_deallocate( msgPtr );
    }
    return events ^ SYS_EVENT_MSG;
  }

  // 板载LED闪烁事件
  if ( events & LED_BLINK_EVENT )
  {
#if (defined HAL_LED) && (HAL_LED == TRUE)
    HalLedUpdate( );
#endif // HAL_LED
    return events ^ LED_BLINK_EVENT;
  }

  // 板载按键事件
  if ( events & KEY_EVENT )
  {
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
    HAL_KeyPoll(); /* Check for keys */
    tmos_start_task( halTaskID, KEY_EVENT, MS1_TO_SYSTEM_TIME(100) );
#endif
    return events ^ KEY_EVENT;
  }

  // 电池ADC检测事件
  if ( events & BATTERY_EVENT )
  {
#if (defined HAL_BATTADC) && (HAL_BATTADC == TRUE)
    HW_ProcessFunc_v.battery_func();
#endif
    tmos_start_task( halTaskID, BATTERY_EVENT, MS1_TO_SYSTEM_TIME(5000) );  // 5s更新采样值
    return events ^ BATTERY_EVENT;
  }

  // CP初始化事件
  if ( events & CP_INITIAL_EVENT )
  {
#if (defined MSG_CP) && (MSG_CP == TRUE)
    err = CP_I2C_RD_Reg(CP_STATUS_REG, &dat);
    if (err != 0 || (dat & 0x1) == 0) {
      tmos_start_task( halTaskID, CP_INITIAL_EVENT, MS1_TO_SYSTEM_TIME(20) );  // CP初始化周期 - 20ms
    } else {
      CP_Ready = TRUE;
      OLED_UI_add_SHOWINFO_task("CP Ready");
      OLED_UI_add_CANCELINFO_delay_task(2000);
    }
#endif
    return events ^ CP_INITIAL_EVENT;
  }

  // 定时主循环事件
  if ( events & MAIN_CIRCULATION_EVENT)
  {
    // 彩蛋模式
#if (defined SW_PAINTEDEGG) && (SW_PAINTEDEGG == TRUE)
    if ( PaintedEggMode == TRUE ) {
      SW_ProcessFunc_v.paintedegg_func();
      goto main_circulation_end;
    }
#endif
    // 鼠标处理
#if ((defined HAL_PS2) && (HAL_PS2 == TRUE)) || ((defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)) || ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE))
    HID_ProcessFunc_v.mouse_func();
#endif
    // 键盘处理
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
    HID_ProcessFunc_v.keyboard_func();
#endif
    // 触摸条处理
#if (defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE)
    HW_ProcessFunc_v.touchbar_func();
#endif
    // CP通信处理
#if (defined MSG_CP) && (MSG_CP == TRUE)
    if (CP_Ready == TRUE) {
      HW_ProcessFunc_v.msgcp_func();
    }
#endif
    // OLED信息更新处理
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    SW_ProcessFunc_v.oled_UBstatus_func();
    SW_ProcessFunc_v.oled_capslock_func();
#endif
    main_circulation_end:
    tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, MS1_TO_SYSTEM_TIME(5) ); // 5ms周期
    return events ^ MAIN_CIRCULATION_EVENT;
  }

  // 系统定时时间
  if ( events & SYST_EVENT )
  {
    ++idle_cnt;
    if (idle_cnt == IDLE_MAX_PERIOD) {  // 进入idle
#ifdef HAL_OLED
      OLED_UI_idle(1);
#endif
    }
    if (idle_cnt >= LP_MAX_PERIOD) {  // 进入低功耗模式
      idle_cnt = 0;
#ifdef HAL_OLED
      OLED_WR_Byte(0xAE, OLED_CMD);  // display off
#endif
      GotoLowpower(shutdown_mode);
    }
    tmos_start_task( halTaskID, SYST_EVENT, MS1_TO_SYSTEM_TIME(500) ); // 500ms周期
    return events ^ SYST_EVENT;
  }

  // WS2812控制事件
  if ( events & WS2812_EVENT )
  {
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
    HW_ProcessFunc_v.ws2812_func();
#endif
    tmos_start_task( halTaskID, WS2812_EVENT, MS1_TO_SYSTEM_TIME(60) ); // 60ms周期控制背光
    return events ^ WS2812_EVENT;
  }

  // MPR121事件
  if ( events & MPR121_EVENT )
  {
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
    if (++collect_cnt >= ALG_COLLECT_CNT) {
      collect_cnt = 0;
    }
#endif
#if (defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)
    if (collect_cnt == 0) {
      MPR121_ALG_Judge_Cap_Mouse();
    } else {
      MPR121_ReadHalfWord(MPR121_REG_STS0, &dat16);
      MPR121_ALG_Update_algListNode(cap_mouse_sts_head, 0, dat16);   // update cap_mouse status
      for (i = 0; i < 4; i++) {
        MPR121_ReadHalfWord(MPR121_REG_EFD0LB+2*MPR121_Cap_Mouse_Pinmux[i], &dat16);
        MPR121_ALG_Update_algListNode(cap_mouse_dat_head, i, dat16);   // update cap_mouse data
      }
    }
#endif
#if (defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE)
    if (collect_cnt == 0) {
      MPR121_ALG_Judge_Touchbar();
    } else {
      MPR121_ReadHalfWord(MPR121_REG_STS0, &dat16);
      MPR121_ALG_Update_algListNode(touchbar_head, 0, dat16);   // update touchbar status
    }
#endif
    tmos_start_task( halTaskID, MPR121_EVENT, MS1_TO_SYSTEM_TIME(MPR121_TASK_PERIOD) ); // (MPR121_TASK_PERIOD)ms控制周期
    return events ^ MPR121_EVENT;
  }

  // OLED UI事件
  if ( events & OLED_UI_EVENT )
  {
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
    OLED_UI_draw_thread_callback();
#endif
    tmos_start_task( halTaskID, OLED_UI_EVENT, MS1_TO_SYSTEM_TIME(1000/oled_fresh_rate) ); // 根据OLED刷新率控制
    return events ^ OLED_UI_EVENT;
  }

  // USB Ready事件
  if ( events & USB_READY_EVENT )
  {
    USB_Ready = TRUE;
    return events ^ USB_READY_EVENT;
  }

  // 硬件初始化事件
  if(events & HAL_REG_INIT_EVENT)
  {
#if(defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE) // 校准任务，单次校准耗时小于10ms
    BLE_RegInit();  // 校准RF
#if(CLK_OSC32K)
    Lib_Calibration_LSI(); // 校准内部RC
#endif
    tmos_start_task(halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME(BLE_CALIBRATION_PERIOD));
    return events ^ HAL_REG_INIT_EVENT;
#endif
  }

  // 测试事件
  if ( events & HAL_TEST_EVENT )
  {
    PRINT( "*\n" );
//    if (motor_status == FALSE) {
//      motor_status = TRUE;
//      MOTOR_RUN();
//    } else {
//      motor_status = FALSE;
//      MOTOR_STOP();
//    }
    tmos_start_task( halTaskID, HAL_TEST_EVENT, MS1_TO_SYSTEM_TIME( 1000 ) );
    return events ^ HAL_TEST_EVENT;
  }

  return 0;
}

/*******************************************************************************
 * @fn          HAL_Init
 *
 * @brief       硬件初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HAL_Init()
{
  char debug_info[128] = "All Ready!";

  halTaskID = TMOS_ProcessEventRegister( HAL_ProcessEvent );
  HAL_TimeInit();
#if (defined HAL_SLEEP) && (HAL_SLEEP == TRUE)
  HAL_SleepInit();
#endif
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  HAL_KeyInit( );
#endif
#if (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
  HW_I2C_Init( );
#if (defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)  // I2C_TP is used by HW I2C
  I2C_TP_Init(debug_info);
#endif
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
  MPR121_Init(debug_info);
#endif
#endif
#if (defined HAL_OLED) && (HAL_OLED == TRUE)
  HAL_OLED_Init( );
#endif
#if (defined HAL_BATTADC) && (HAL_BATTADC == TRUE)
  BATTERY_Init( );
#endif
#if (defined HAL_USB) && (HAL_USB == TRUE)
  HAL_USBInit( );
#endif
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
  PS2_Init(debug_info, TRUE); // PS/2中断实现
#endif
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
  KEYBOARD_Init( );
#endif
#if (defined HAL_WS2812_PWM) && (HAL_WS2812_PWM == TRUE)
  WS2812_PWM_Init( );
#endif
#if (defined HAL_MOTOR) && (HAL_MOTOR == TRUE)
  MOTOR_Init( );
#endif
#if (defined MSG_CP) && (MSG_CP == TRUE)
  MSG_CP_Init(debug_info);
#endif
#if (defined HAL_RF) && (HAL_RF == TRUE) && !(defined TEST)
  FLASH_Read_RForBLE( );
#endif
#if (defined HAL_LED) && (HAL_LED == TRUE)
  debug_info[7] = '\0';
  if ( strcmp(debug_info, "[ERROR]") == 0 ) {
    HAL_LedInit(1);
  } else {
    HAL_LedInit(0);
  }
#endif
#if ( defined BLE_CALIBRATION_ENABLE ) && ( BLE_CALIBRATION_ENABLE == TRUE )
  tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );    // 添加校准任务，单次校准耗时小于10ms
#endif
  PRINT("%s\n", debug_info);
  /******* 初始化OLED UI *******/
  FLASH_Read_DeviceID();
#ifdef OLED_0_96
  OLED_ShowString(2, 1, "L1");
  OLED_ShowChar(20, 1, 'S');
  OLED_ShowNum(26, 1, FLASH_Read_LEDStyle( ), 1);
  OLED_ShowChar(38, 1, 'D');
  OLED_ShowNum(44, 1, DeviceAddress[5], 1);
  OLED_UI_ShowCapslock(56, 1, FALSE);
#endif
  OLED_UI_draw_empty_battery();  // 绘制空电池
//  OLED_Scroll(7, 7, 24, 16, 2, 1, 0);    // 128 FRAMES
  { // 限时打印
//    OLED_UI_add_SHOWINFO_task("%s", debug_info);
//    OLED_UI_add_CANCELINFO_delay_task(3000);
//    OLED_UI_show_version(1);
  }
  OLED_UI_draw_menu(OLED_UI_MENU_REFRESH);
  OLED_UI_draw_menu(OLED_UI_SWIPE_LEFT);
//  OLED_UI_draw_menu(OLED_UI_SWIPE_LEFT);
//  tmos_start_task( halTaskID, HAL_TEST_EVENT, 1600 );    // 添加测试任务
}

/*******************************************************************************
 * @fn      HAL_GetInterTempValue
 *
 * @brief   获取内部温感采样值，如果使用了ADC中断采样，需在此函数中暂时屏蔽中断.
 *
 * @return  内部温感采样值.
 */
uint16_t HAL_GetInterTempValue(void)
{
    uint8_t  sensor, channel, config, tkey_cfg;
    uint16_t adc_data;

    tkey_cfg = R8_TKEY_CFG;
    sensor = R8_TEM_SENSOR;
    channel = R8_ADC_CHANNEL;
    config = R8_ADC_CFG;
    ADC_InterTSSampInit();
    R8_ADC_CONVERT |= RB_ADC_START;
    while(R8_ADC_CONVERT & RB_ADC_START);
    adc_data = R16_ADC_DATA;
    R8_TEM_SENSOR = sensor;
    R8_ADC_CHANNEL = channel;
    R8_ADC_CFG = config;
    R8_TKEY_CFG = tkey_cfg;
    return (adc_data);
}

/******************************** endfile @ mcu ******************************/
