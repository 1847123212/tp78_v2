/********************************** (C) COPYRIGHT *******************************
 * File Name          : FATFS.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/2/5
 * Description        : FatFs”¶”√≤„«˝∂Ø
 *******************************************************************************/

#ifndef __FATFS_H
  #define __FATFS_H

  #include "CH58x_common.h"

  #define FS_LINE_START                 0
  #define FS_LINE_BLE_DEVICE            0
  #define FS_LINE_LED_STYLE             1
  #define FS_LINE_RF_READY              2
  #define FS_LINE_MPR_ALG_MAGIC         3
  #define FS_LINE_MPR_TOUCHBAR_TOU_THR  4
  #define FS_LINE_MPR_TOUCHBAR_REL_THR  5
  #define FS_LINE_MPR_CAPMOUSE_TOU_THR  6
  #define FS_LINE_MPR_CAPMOUSE_REL_THR  7

  void HAL_Fs_Init(char* debug_info);
  void HAL_Fs_Create_keyboard_cfg(uint8_t len, uint8_t* p_cfg);
  void HAL_Fs_Write_keyboard_cfg(uint8_t fs_line, uint8_t len, uint8_t* p_cfg);
  void HAL_Fs_Read_keyboard_cfg(uint8_t fs_line, uint8_t len, uint8_t* p_cfg);
  void HAL_Fs_Write_keyboard_mat(const uint8_t* fp, const uint8_t* key_arr);
  void HAL_Fs_Read_keyboard_mat(const uint8_t* fp, uint8_t* key_arr);

#endif
