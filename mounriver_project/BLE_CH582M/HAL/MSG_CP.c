/********************************** (C) COPYRIGHT *******************************
 * File Name          : MSG_CP.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/12/25
 * Description        : CapProcessor I2Cͨ������
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

/*******************************************************************************
* Function Name  : MSG_CP_Init
* Description    : ��ʼ��CPmsg
* Input          : debug_info - ������Ϣ
* Return         : �ɹ�����0��ʧ�ܷ���1
*******************************************************************************/
uint8_t MSG_CP_Init(char* debug_info)
{
  uint8_t dat, err;
  uint16_t cnt = 0;
#if !(defined HAL_HW_I2C) || (HAL_HW_I2C == FALSE)  // should config MSG_CP after HW_I2C
  strcpy(debug_info, "MSG-ERR-01");
  return 1;
#endif
  CP_WAKEUP_GPIO(SetBits)( CP_WAKEUP_PIN );
  CP_WAKEUP_GPIO(ModeCfg)( CP_WAKEUP_PIN, GPIO_ModeOut_PP_5mA );
  CP_WAKEUP_GPIO(ResetBits)( CP_WAKEUP_PIN );
  g_Ready_Status.cp = FALSE;
  tmos_start_task( halTaskID, CP_INITIAL_EVENT, MS1_TO_SYSTEM_TIME(100) );
  return 0;
}
