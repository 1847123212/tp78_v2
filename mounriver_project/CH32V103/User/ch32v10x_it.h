/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v10x_it.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/04/30
* Description        : This file contains the headers of the interrupt handlers.
*******************************************************************************/
#ifndef __CH32V10x_IT_H
#define __CH32V10x_IT_H

#include "debug.h"

#define GET_INT_SP()   asm("csrrw sp,mscratch,sp")
#define FREE_INT_SP()  asm("csrrw sp,mscratch,sp")

#endif /* __CH32V10x_IT_H */


