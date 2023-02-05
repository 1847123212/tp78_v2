/********************************** (C) COPYRIGHT *******************************
 * File Name          : MSG_CP.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/12/14
 * Description        : CapProcessor I2CÍ¨ÐÅÇý¶¯
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#ifndef __MSG_CP_H
  #define __MSG_CP_H

  #include "HAL.h"

  #define CP_WAKEUP_PIN             GPIO_Pin_20
  #define CP_WAKEUP_GPIO(x)         GPIOB_ ## x

  #define CP_I2C_ADDR               0xB0

  #define CP_REG_DATA_SIZE          0x30
  #define CP_REG_FLASH_DATA_SIZE    0x20

  #define CP_STATUS_REG     0x0
  #define CP_ENABLE_REG     0x1
  #define CP_CHANGE_REG     0x2
  #define CP_ADCVAL_REG(x)  (x + 0x3)
  #define CP_MOUSEDATA_REG  0xC
  #define CP_BTN_REG        0x10
  #define CP_DBTN_REG       0x12
  #define CP_CNT_REG        0x14

  #define CP_FLASH_CAPMOUSE_THR       (CP_REG_DATA_SIZE + 0x0)
  #define CP_FLASH_TOUCHBAR_THR       (CP_REG_DATA_SIZE + 0x1)
  #define CP_FLASH_TOUCHBAR_dCNT      (CP_REG_DATA_SIZE + 0x2)
  #define CP_FLASH_TOUCHBAR_lCNT      (CP_REG_DATA_SIZE + 0x4)

  #define CP_BIT_CP_STATUS            0x1
  #define CP_BIT_CAPMOUSE_STATUS      0x2
  #define CP_BIT_TOUCHBAR_STATUS      0x4
  #define CP_BIT_TPMOUSE_STATUS       0x8
  #define CP_BIT_GLOBAL_MOUSE_STATUS  0x10
  #define CP_BIT_CAPMOUSE_ENABLE      0x1
  #define CP_BIT_TOUCHBAR_ENABLE      0x2
  #define CP_BIT_TPMOUSE_ENABLE       0x4
  #define CP_BIT_GLOBAL_MCU_ENABLE    0x8
  #define CP_BIT_STANDBYMODE_ENABLE   0x10
  #define CP_BIT_CAPMOUSE_CHANGE      0x1
  #define CP_BIT_TOUCHBAR_CHANGE      0x2
  #define CP_BIT_TPMOUSE_CHANGE       0x4

  #define CP_I2C_WR_Reg(reg, dat)               HW_I2C_WR_Reg(reg, dat, CP_I2C_ADDR)
  #define CP_I2C_RD_Reg(reg, p_dat)             HW_I2C_RD_Reg(reg, p_dat, CP_I2C_ADDR)
  #define CP_I2C_Muti_RD_Reg(reg, p_dat, len)   HW_I2C_Muti_RD_Reg(reg, p_dat, CP_I2C_ADDR, len)

  uint8_t MSG_CP_Init(char* debug_info);

#endif
