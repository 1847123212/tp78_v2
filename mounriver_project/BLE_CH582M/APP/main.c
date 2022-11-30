/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : ChnMasterOG, WCH
* Version            : V2.0
* Date               : 2022/11/30
* Description        : TP78小红点+蓝牙+2.4G三模键盘应用主函数及任务系统初始化
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: GPL-3.0
*******************************************************************************/

/********************************** (C) COPYRIGHT *******************************
 *  Pin information: 
 *  PB10 & PB11 --- USB1; [PA0~PA7 & PA9 & PA11~PA15] & [PB2/PB3/PB5/PB6/PB8/PB9] --- KeyBoard
 *  PB15 & PB14 --- PS/2 Reserved; PA10 --- WS2812(TMR1); PB13 & PB12 --- TXD1_/RXD1_
 *  PA8 --- Battery ADC; PB7 & PB4 --- TXD0/RXD0; PB21 & PB20 ---SCL_/SDA_
 *  PB0 --- LED; PB1 --- KEY;
 ********************************* (C) COPYRIGHT ********************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "CH58x_common.h"
#include "HAL.h"
#include "hiddev.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84,0xC2,0xE4,0x03,0x02,0x02};
#endif

/*******************************************************************************
* Function Name  : Main_Circulation
* Description    : 主循环
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__HIGH_CODE
void Main_Circulation()
{
  // 开启TMOS任务调度
  while(1){
    TMOS_SystemProcess( );
  }
}

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
#if (defined (DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
  PWR_DCDCCfg( ENABLE );
#endif
  SetSysClock( CLK_SOURCE_PLL_60MHz );
  FLASH_ROM_LOCK( 0 );  // unlock flash
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  GPIOA_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
  GPIOB_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
#endif
#ifdef DEBUG
  GPIOB_SetBits(1<<13); // PB13
  GPIOB_ModeCfg(1<<13, GPIO_ModeOut_PP_5mA);
  GPIOPinRemap(ENABLE, RB_PIN_UART1);
  UART1_DefInit( );
#endif
  PRINT("%s\n",VER_LIB);
  CH57X_BLEInit( );
  HAL_Init( );
  if (RF_Ready == FALSE) {
    GAPRole_PeripheralInit( );
    HidDev_Init( );
    HidEmu_Init( );
  } else {
    RF_RoleInit();
    RF_Init();
  }
  tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, 10 ); // 主循环
  tmos_start_task( halTaskID, WS2812_EVENT, 10 );  // 背光控制
  Main_Circulation();
}

/******************************** endfile @ main ******************************/
