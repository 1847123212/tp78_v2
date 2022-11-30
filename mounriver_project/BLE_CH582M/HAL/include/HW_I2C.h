/********************************** (C) COPYRIGHT *******************************
 * File Name          : HW_I2C.h
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2022/11/30
 * Description        : Ó²¼þI2CÇý¶¯
 *******************************************************************************/

#ifndef __HW_I2C_H
  #define __HW_I2C_H

  #include "CH58x_common.h"

  #define HW_I2C_SCL_PIN      GPIO_Pin_21
  #define HW_I2C_SDA_PIN      GPIO_Pin_20
  #define HW_GPIO_REMAP       TRUE
  #define HW_I2C_MASTER_ADDR  0x52

  void HW_I2C_Init(void);
  void HW_I2C_WR_Reg(uint8_t reg, uint8_t dat, uint8_t addr);
  uint8_t HW_I2C_RD_Reg(uint8_t reg, uint8_t addr);
  void HW_I2C_Muti_RD_Reg(uint8_t reg, uint8_t* dat, uint8_t addr, uint8_t len);

#endif
