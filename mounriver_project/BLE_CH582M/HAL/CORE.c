/********************************** (C) COPYRIGHT *******************************
* File Name          : CORE.c
* Author             : ChnMasterOG
* Version            : V1.1
* Date               : 2022/12/24
* Description        : 上下电、MCU Sleep相关控制
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: GPL-3.0
*******************************************************************************/

#include "HAL.h"
#include "CONFIG.h"

/*******************************************************************************
* Function Name  : APPJumpBoot
* Description    : 跳转到bootloader
* Input          : 无
* Return         : 无
*******************************************************************************/
__HIGH_CODE
void APPJumpBoot(void)   //此段代码必须运行在RAM中
{
  while(FLASH_ROM_ERASE(0, EEPROM_BLOCK_SIZE))
  {
    ;//ROM 擦4K1个单位，擦0地址起始
  }
  FLASH_ROM_SW_RESET();
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;  //安全访问模式
  SAFEOPERATE;
  R16_INT32K_TUNE = 0xFFFF;
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  R8_SAFE_ACCESS_SIG = 0;//进入后执行复位，复位类型为上电复位
  while(1);//营造空片的现象，启动时就会停在BOOT，等烧写，超时时间10s
}

/*******************************************************************************
* Function Name  : SoftReset
* Description    : 软件复位
* Input          : 无
* Return         : 无
*******************************************************************************/
__HIGH_CODE
void SoftReset(void)   //软件复位
{
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;  //安全访问模式
  SAFEOPERATE;
  R16_INT32K_TUNE = 0xFFFF;
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  R8_SAFE_ACCESS_SIG = 0;
  while(1);
}

/*******************************************************************************
* Function Name  : GotoLowpower
* Description    : MCU进Low Power模式
* Input          : type: 低功耗模式类型
* Return         : 无
*******************************************************************************/
__HIGH_CODE
void GotoLowpower(enum LP_Type type)
{
#if (defined HAL_KEYBOARD) && (HAL_KEYBOARD == TRUE)
  if (type > 3) return; // error type
#if (defined MSG_CP) && (MSG_CP == TRUE)
  CP_I2C_WR_Reg(CP_ENABLE_REG, CP_BIT_STANDBYMODE_ENABLE);  // 控制CP进低功耗模式
#endif
  Colum_GPIO_(ModeCfg)( Colum_Pin_ALL, GPIO_ModeIN_PD );
  Colum_GPIO_(ITModeCfg)( Colum_Pin_ALL, GPIO_ITMode_RiseEdge ); // 上升沿唤醒
  PFIC_EnableIRQ(GPIO_A_IRQn);  // colum GPIO is GPIOA
  PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE, Short_Delay);
  switch (type)
  {
    case idle_mode: // 空闲模式 - 1.6mA
      LowPower_Idle();
      break;
    case halt_mode: // 暂停模式 - 320uA
      LowPower_Halt();
      break;
    case sleep_mode:  // 睡眠模式 - 0.7uA~2.8uA
      LowPower_Sleep(RB_PWR_RAM30K | RB_PWR_RAM2K);
      break;
    case shutdown_mode: // 下电模式 - 0.2uA~2.3uA
      LowPower_Shutdown(0);
      break;
    default:  // do not run here
      return;
  }
#else
  return;
#endif
}
