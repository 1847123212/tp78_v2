/********************************** (C) COPYRIGHT *******************************
* File Name          : CORE.c
* Author             : ChnMasterOG
* Version            : V1.1
* Date               : 2022/12/24
* Description        : ���µ硢MCU Sleep��ؿ���
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: GPL-3.0
*******************************************************************************/

#include "HAL.h"
#include "CONFIG.h"

/*******************************************************************************
* Function Name  : APPJumpBoot
* Description    : ��ת��bootloader
* Input          : ��
* Return         : ��
*******************************************************************************/
__HIGH_CODE
void APPJumpBoot(void)   //�˶δ������������RAM��
{
  while(FLASH_ROM_ERASE(0, EEPROM_BLOCK_SIZE))
  {
    ;//ROM ��4K1����λ����0��ַ��ʼ
  }
  FLASH_ROM_SW_RESET();
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;  //��ȫ����ģʽ
  SAFEOPERATE;
  R16_INT32K_TUNE = 0xFFFF;
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  R8_SAFE_ACCESS_SIG = 0;//�����ִ�и�λ����λ����Ϊ�ϵ縴λ
  while(1);//Ӫ���Ƭ����������ʱ�ͻ�ͣ��BOOT������д����ʱʱ��10s
}

/*******************************************************************************
* Function Name  : SoftReset
* Description    : �����λ
* Input          : ��
* Return         : ��
*******************************************************************************/
__HIGH_CODE
void SoftReset(void)   //�����λ
{
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;  //��ȫ����ģʽ
  SAFEOPERATE;
  R16_INT32K_TUNE = 0xFFFF;
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  R8_SAFE_ACCESS_SIG = 0;
  while(1);
}

/*******************************************************************************
* Function Name  : GotoLowpower
* Description    : MCU��Low Powerģʽ
* Input          : type: �͹���ģʽ����
* Return         : ��
*******************************************************************************/
__HIGH_CODE
void GotoLowpower(enum LP_Type type)
{
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
  if (type > 3) return; // error type
#if (defined MSG_CP) && (MSG_CP == TRUE)
  CP_I2C_WR_Reg(CP_ENABLE_REG, CP_BIT_STANDBYMODE_ENABLE);  // ����CP���͹���ģʽ
#endif
  Colum_GPIO_(ModeCfg)( Colum_Pin_ALL, GPIO_ModeIN_PD );
  Colum_GPIO_(ITModeCfg)( Colum_Pin_ALL, GPIO_ITMode_RiseEdge ); // �����ػ���
  PFIC_EnableIRQ(GPIO_A_IRQn);  // colum GPIO is GPIOA
  PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE, Short_Delay);
  switch (type)
  {
    case idle_mode: // ����ģʽ - 1.6mA
      LowPower_Idle();
      break;
    case halt_mode: // ��ͣģʽ - 320uA
      LowPower_Halt();
      break;
    case sleep_mode:  // ˯��ģʽ - 0.7uA~2.8uA
      LowPower_Sleep(RB_PWR_RAM30K | RB_PWR_RAM2K);
      break;
    case shutdown_mode: // �µ�ģʽ - 0.2uA~2.3uA
      LowPower_Shutdown(0);
      break;
    default:  // do not run here
      return;
  }
#else
  return;
#endif
}
