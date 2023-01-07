/********************************** (C) COPYRIGHT *******************************
 * File Name          : MPR121.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/1/3
 * Description        : mpr121电容传感器驱动
 *******************************************************************************/

#ifndef __MPR121_H
  #define __MPR121_H

  #include "CH58x_common.h"

  #define MPRINT_Pin        GPIO_Pin_19   // the same as PS/2 scl pin
  #define MPRINT_GPIO_(x)   GPIOB_ ## x
  #define MPR121_ADDR       0xB6          // 0x5A<<1, addr pin connects to GND

  #define MPR121_Number         1

  #define MPR121_REG_STS0       0x00  // ELE0~ELE7 Touch Status
  #define MPR121_REG_STS1       0x01  // ELE8~ELE11, ELEPROX Touch Status

  #define MPR121_REG_EFD0LB     0x04  // ELE0 Electrode Filtered Data LSB
  #define MPR121_REG_EFD0HB     0x05  // ELE0 Electrode Filtered Data MSB
  #define MPR121_REG_EFD1LB     0x06  // ELE1 Electrode Filtered Data LSB
  #define MPR121_REG_EFD1HB     0x07  // ELE1 Electrode Filtered Data MSB
  #define MPR121_REG_EFD2LB     0x08  // ELE2 Electrode Filtered Data LSB
  #define MPR121_REG_EFD2HB     0x09  // ELE2 Electrode Filtered Data MSB
  #define MPR121_REG_EFD3LB     0x0A  // ELE3 Electrode Filtered Data LSB
  #define MPR121_REG_EFD3HB     0x0B  // ELE3 Electrode Filtered Data MSB
  #define MPR121_REG_EFD4LB     0x0C  // ELE4 Electrode Filtered Data LSB
  #define MPR121_REG_EFD4HB     0x0D  // ELE4 Electrode Filtered Data MSB
  #define MPR121_REG_EFD5LB     0x0E  // ELE5 Electrode Filtered Data LSB
  #define MPR121_REG_EFD5HB     0x0F  // ELE5 Electrode Filtered Data MSB
  #define MPR121_REG_EFD6LB     0x10  // ELE6 Electrode Filtered Data LSB
  #define MPR121_REG_EFD6HB     0x11  // ELE6 Electrode Filtered Data MSB
  #define MPR121_REG_EFD7LB     0x12  // ELE7 Electrode Filtered Data LSB
  #define MPR121_REG_EFD7HB     0x13  // ELE7 Electrode Filtered Data MSB
  #define MPR121_REG_EFD8LB     0x14  // ELE8 Electrode Filtered Data LSB
  #define MPR121_REG_EFD8HB     0x15  // ELE8 Electrode Filtered Data MSB
  #define MPR121_REG_EFD9LB     0x16  // ELE9 Electrode Filtered Data LSB
  #define MPR121_REG_EFD9HB     0x17  // ELE9 Electrode Filtered Data MSB
  #define MPR121_REG_EFD10LB    0x18  // ELE10 Electrode Filtered Data LSB
  #define MPR121_REG_EFD10HB    0x19  // ELE10 Electrode Filtered Data MSB
  #define MPR121_REG_EFD11LB    0x1A  // ELE10 Electrode Filtered Data LSB
  #define MPR121_REG_EFD11HB    0x1B  // ELE10 Electrode Filtered Data MSB
  #define MPR121_REG_EFDPROXLB  0x1C  // ELEPROX Electrode Filtered Data LSB
  #define MPR121_REG_EFDPROXHB  0x1D  // ELEPROX Electrode Filtered Data MSB

  #define MPR121_REG_MHDR       0x2B  // Maximum Half Delta Rising(1~63).
  #define MPR121_REG_NHDR       0x2C  // Noise Half Delta Rising(1~63).
  #define MPR121_REG_NCLR       0x2D  // Noise Count Limit Rising(0~255).
  #define MPR121_REG_FDLR       0x2E  // Filter Delay Count Limit Rising(0~255).
  #define MPR121_REG_MHDF       0x2F  // Maximum Half Delta Falling(1~63).
  #define MPR121_REG_NHDF       0x30  // Noise Half Delta Falling(1~63).
  #define MPR121_REG_NCLF       0x31  // Noise Count Limit Falling(0~255).
  #define MPR121_REG_FDLF       0x32  // Filter Delay Count Limit Falling(0~255).
  #define MPR121_REG_NHDT       0x33  // NHD Amount Touched
  #define MPR121_REG_NCLT       0x34  // NCL Touched
  #define MPR121_REG_FDLT       0x35  // FDL Touched
  #define MPR121_REG_MHDPROXR   0x36  // ELEPROX MHD Rising
  #define MPR121_REG_NHDPROXR   0x37  // ELEPROX NHD Amount Rising
  #define MPR121_REG_NCLPROXR   0x38  // ELEPROX NCL Rising
  #define MPR121_REG_FDLPROXR   0x39  // ELEPROX FDL Rising
  #define MPR121_REG_MHDPROXF   0x3A  // ELEPROX MHD Falling
  #define MPR121_REG_NHDPROXF   0x3B  // ELEPROX NHD Amount Falling
  #define MPR121_REG_NCLPROXF   0x3C  // ELEPROX NCL Falling
  #define MPR121_REG_FDLPROXF   0x3D  // ELEPROX FDL Falling
  #define MPR121_REG_NHDPROXT   0x3E  // ELEPROX NHD Amount Touched
  #define MPR121_REG_NCLPROXT   0x3F  // ELEPROX NCL Touched
  #define MPR121_REG_FDLPROXT   0x40  // ELEPROX FDL Touched

  #define MPR121_REG_E0TTH      0x41  // ELE0 Touch Threshold
  #define MPR121_REG_E0RTH      0x42  // ELE0 Release Threshold
  #define MPR121_REG_E1TTH      0x43  // ELE1 Touch Threshold
  #define MPR121_REG_E1RTH      0x44  // ELE1 Release Threshold
  #define MPR121_REG_E2TTH      0x45  // ELE2 Touch Threshold
  #define MPR121_REG_E2RTH      0x46  // ELE2 Release Threshold
  #define MPR121_REG_E3TTH      0x47  // ELE3 Touch Threshold
  #define MPR121_REG_E3RTH      0x48  // ELE3 Release Threshold
  #define MPR121_REG_E4TTH      0x49  // ELE4 Touch Threshold
  #define MPR121_REG_E4RTH      0x4A  // ELE4 Release Threshold
  #define MPR121_REG_E5TTH      0x4B  // ELE5 Touch Threshold
  #define MPR121_REG_E5RTH      0x4C  // ELE5 Release Threshold
  #define MPR121_REG_E6TTH      0x4D  // ELE6 Touch Threshold
  #define MPR121_REG_E6RTH      0x4E  // ELE6 Release Threshold
  #define MPR121_REG_E7TTH      0x4F  // ELE7 Touch Threshold
  #define MPR121_REG_E7RTH      0x50  // ELE7 Release Threshold
  #define MPR121_REG_E8TTH      0x51  // ELE8 Touch Threshold
  #define MPR121_REG_E8RTH      0x52  // ELE8 Release Threshold
  #define MPR121_REG_E9TTH      0x53  // ELE9 Touch Threshold
  #define MPR121_REG_E9RTH      0x54  // ELE9 Release Threshold
  #define MPR121_REG_E10TTH     0x55  // ELE10 Touch Threshold
  #define MPR121_REG_E10RTH     0x56  // ELE10 Release Threshold
  #define MPR121_REG_E11TTH     0x57  // ELE11 Touch Threshold
  #define MPR121_REG_E11RTH     0x58  // ELE11 Release Threshold

  #define MPR121_REG_EPROXTTH   0x59  // ELEPROX Touch Threshold
  #define MPR121_REG_EPROXRTH   0x5A  // ELEPROX Release Threshold
  #define MPR121_REG_DTR        0x5B  // Debounce Touch & Release
  #define MPR121_REG_FG_CDC     0x5C  // Filter/Global CDC Configuration
  #define MPR121_REG_FG_CDT     0x5D  // Filter/Global CDT Configuration

  #define MPR121_REG_ACCR0      0x7B  // AUTO-CONFIG Control Register 0
  #define MPR121_REG_ACCR1      0x7C  // AUTO-CONFIG Control Register 1
  #define MPR121_REG_ACUSL      0x7D  // AUTO-CONFIG USL Register
  #define MPR121_REG_ACLSL      0x7E  // AUTO-CONFIG LSL Register
  #define MPR121_REG_ACTLR      0x7F  // AUTO-CONFIG Target Level Register

  #define MPR121_REG_ECR        0x5E  // D7~D6:CL, D5~D4:ELEPROX_EN, D3~D0:ELE_EN
  #define MPR121_REG_SRST       0x80  // write 0x63 to reset

  #define TOUCHBAR_TOU_THRESH   20    // touchbar touch threshold (default 3)
  #define TOUCHBAR_REL_THRESH   10    // touchbar release threshold (default 2)
  #define CAP_MOUSE_TOU_THRESH  15    // cap_mouse touch threshold (default 3)
  #define CAP_MOUSE_REL_THRESH  7     // cap_mouse release threshold (default 2)

  #define MPR121_WriteReg(reg, dat)           HW_I2C_WR_Reg(reg, dat, MPR121_ADDR)
  #define MPR121_ReadReg(reg, pdat)           HW_I2C_RD_Reg(reg, pdat, MPR121_ADDR)
  #define MPR121_ReadHalfWord(reg, pdat)      HW_I2C_Muti_RD_Reg(reg, (uint8*)pdat, MPR121_ADDR, 2);
  #define MPR121_ReadMutiReg(reg, pdat, len)  HW_I2C_Muti_RD_Reg(reg, pdat, MPR121_ADDR, len)

  /* mpr121 algorithm */
  #define ALG_COLLECT_CNT         2
  #define ALG_CAP_MOUSE_BUF_LEN   2 // must be 2
  #define ALG_TOUCHBAR_BUF_LEN    2 // must be 2

  #define ALG_MOVE_UP_INDEX       0
  #define ALG_MOVE_DOWN_INDEX     1
  #define ALG_MOVE_LEFT_INDEX     2
  #define ALG_MOVE_RIGHT_INDEX    3

  #define ALG_PARAM_MAGIC         0x78

  #define __LIMIT__(data, limit_val)  (data >= limit_val ? limit_val : data)

  typedef struct alg_ListNode{
    uint16_t dat;
    struct alg_ListNode* next;
    struct alg_ListNode* prev;
  }alg_ListNode;

  typedef struct alg_Param{
    uint16_t double_touch_cnt;
    uint16_t long_touch_cnt;
    uint16_t btn_dat;
    uint16_t dbtn_dat;
    uint16_t cnt_dat;
    uint8_t magic;
    uint8_t cap_mouse_event_thr;
  }alg_Param;

  typedef struct Touchbarstate{
      BOOL swip_left;
      BOOL swip_right;
  }Touchbarstate;

  typedef enum {
    DIRECT_OTHER = 0,
    DIRECT_LEFT,
    DIRECT_RIGHT,
    DIRECT_LEFT2,
    DIRECT_RIGHT2,
    DIRECT_JUDGE_LEFT,
    DIRECT_JUDGE_RIGHT,
  }alg_TouchbarDirectiion;

  extern uint8_t MPR121_Cap_Mouse_Pinmux[4];
  extern uint8_t MPR121_TouchBar_Pinmux[7];

  extern alg_ListNode* cap_mouse_dat_head[4];
  extern alg_ListNode* cap_mouse_sts_head[1];
  extern alg_ListNode* touchbar_head[1];

  extern uint8_t cap_mouse_data_change;
  extern uint8_t touchbar_data_change;

  void MPR121_Init(char* buf);
  void FLASH_Read_MPR121_ALG_Parameter(void);
  void FLASH_Write_MPR121_ALG_Parameter(void);
  void MPR121_ALG_Update_algListNode(alg_ListNode* p[], uint8_t index, uint16_t dat);
  void MPR121_ALG_Judge_Cap_Mouse(void);
  void MPR121_ALG_Judge_Touchbar(void);

#endif
