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

//��ӳ���ʼ������1
void UART1_Remap_Init(void)
{
  /* ���ô���1��������IO��ģʽ�������ô��� */
  GPIOA_SetBits( GPIO_Pin_13 );
  GPIOA_ModeCfg( GPIO_Pin_12, GPIO_ModeIN_PU );         // RXD-������������
  GPIOA_ModeCfg( GPIO_Pin_13, GPIO_ModeOut_PP_5mA );    // TXD-�������������ע������IO������ߵ�ƽ
  GPIOPinRemap( ENABLE, RB_PIN_UART1 );
  UART1_DefInit();
}
