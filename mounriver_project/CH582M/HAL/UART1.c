/********************************** (C) COPYRIGHT *******************************
 * File Name          : UART.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/17
 * Description        : UART����Դ�ļ�
 *******************************************************************************/

#include "UART1.h"

//��ʼ������1
void UART1_Init(void)
{
  /* ���ô���1��������IO��ģʽ�������ô��� */
  GPIOA_SetBits( GPIO_Pin_9 );
  GPIOA_ModeCfg( GPIO_Pin_8, GPIO_ModeIN_PU );         // RXD-������������
  GPIOA_ModeCfg( GPIO_Pin_9, GPIO_ModeOut_PP_5mA );    // TXD-�������������ע������IO������ߵ�ƽ
  UART1_DefInit();
}

__INTERRUPT
__HIGH_CODE
void UART1_IRQHandler( void )
{

}
