/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : ChnMasterOG, WCH
* Version            : V2.0
* Date               : 2022/11/30
* Description        : TP78С���+����+2.4G��ģ����Ӧ��������������ϵͳ��ʼ��
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: GPL-3.0
*******************************************************************************/

/********************************** (C) COPYRIGHT *******************************
 *  Pin information:
 *  PB10 & PB11 --- USB1_D-/USB1_D+; [PA0~PA7 & PA9 & PA11~PA15] & [PB0/PB1/PB2/PB3/PB5/PB6] --- KeyBoard
 *  PB15(*) & PB14 --- PS/2 Reserved; PA10 --- WS2812(TMR1); PB13 & PB12 --- SCL/SDA
 *  PA8 --- Battery ADC; PB16 & PB17 --- Battery COM/CHRG; PB7 & PB4 --- TXD0/RXD0;
 *  PB21 & PB20 ---TXD3_/RXD3_; PB15(*) --- I2C TrackPoint IRQ; PB18 --- MPR121 IRQ; PB19 --- Motor CTL
 ********************************* (C) COPYRIGHT ********************************/

/*********************************************************************
 * INCLUDES
 */
#include "HAL.h"
#include "CONFIG.h"
#include "CH58x_common.h"
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
* Description    : ��ѭ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__HIGH_CODE
void Main_Circulation()
{
  // ����TMOS�������
  while(1){
    TMOS_SystemProcess( );
  }
}

/*******************************************************************************
* Function Name  : main
* Description    : ������
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
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  GPIOA_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
  GPIOB_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
#endif
#ifdef DEBUG  // DEBUG = 3(use UART3)
  GPIOB_SetBits(GPIO_Pin_21);
  GPIOB_ModeCfg(GPIO_Pin_20, GPIO_ModeIN_PU);       // RXD-������������
  GPIOB_ModeCfg(GPIO_Pin_21, GPIO_ModeOut_PP_5mA);  // TXD-�������������ע������IO������ߵ�ƽ
  GPIOPinRemap(ENABLE, RB_PIN_UART3);
  UART3_DefInit();
#endif
  PRINT("%s\n",VER_LIB);
  CH58X_BLEInit( );
  HAL_Init( );
#if (defined (HAL_RF)) && (HAL_RF == TRUE)
  if (g_Ready_Status.rf == FALSE) {
#endif
    GAPRole_PeripheralInit( );
    HidDev_Init( );
    HidEmu_Init( );
#if (defined (HAL_RF)) && (HAL_RF == TRUE)
  } else {
    RF_RoleInit();
    RF_Init();
  }
#endif
  tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, 10 ); // ��ѭ��
  tmos_start_task( halTaskID, WS2812_EVENT, 10 );  // �������
  tmos_start_task( halTaskID, OLED_UI_EVENT, 10 );  // OLED UI
  tmos_start_task( usbTaskID, USB_TEST_EVENT, 1000 );  // USB TEST
#if ((defined HAL_MPR121_CAPMOUSE) && (HAL_MPR121_CAPMOUSE == TRUE)) || ((defined HAL_MPR121_TOUCHBAR) && (HAL_MPR121_TOUCHBAR == TRUE))
  tmos_start_task( halTaskID, MPR121_EVENT, 10 );  // MPR121
#endif
  tmos_start_task( halTaskID, SYST_EVENT, 10 );  // ϵͳ��ʱ
  Main_Circulation();
}

/*******************************************************************************
* Function Name  : GPIOA_IRQHandler
* Description    : GPIOA�ⲿ�ж�
* Input          : None
* Return         : None
*******************************************************************************/
__INTERRUPT
__HIGH_CODE
void GPIOA_IRQHandler(void)
{
#if (defined MSG_CP) && (MSG_CP == TRUE)
  CP_WAKEUP_GPIO(SetBits)( CP_WAKEUP_PIN );   // ����CP
  g_Ready_Status.cp = FALSE;
  tmos_start_task( halTaskID, CP_INITIAL_EVENT, MS1_TO_SYSTEM_TIME(300) );  // CP�����ϵ�
#endif
  GPIOA_ClearITFlagBit(Colum_Pin_ALL);  // ���ڻ���
}

/*******************************************************************************
* Function Name  : GPIOB_IRQHandler
* Description    : GPIOB�ⲿ�ж�
* Input          : None
* Return         : None
*******************************************************************************/
__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler( void )
{
#if (defined HAL_PS2) && (HAL_PS2 == TRUE)
  PS2_IT_handler();
#endif
#if (defined HAL_I2C_TP) && (HAL_I2C_TP == TRUE)
  I2C_TP_IT_handler();
#endif
}

/******************************** endfile @ main ******************************/
