/********************************** (C) COPYRIGHT *******************************
 * File Name          : CORE.h
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2022/12/24
 * Description        : 上下电、MCU Sleep相关控制
 *******************************************************************************/

#ifndef __CORE_H
  #define __CORE_H

  #include "CH58x_common.h"

  enum LP_Type {
    idle_mode = 0,
    halt_mode,
    sleep_mode,
    shutdown_mode,
  };

  void APPJumpBoot(void);
  void SoftReset(void);
  void GotoLowpower(enum LP_Type type);
#endif
