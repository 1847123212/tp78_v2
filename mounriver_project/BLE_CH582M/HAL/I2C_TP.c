/********************************** (C) COPYRIGHT *******************************
 * File Name          : I2C_TP.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/1/2
 * Description        : i2cС�������
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

uint8_t I2C_TP_data_ready = 0;
Mousestate* const I2CTPdat = (Mousestate*)&HID_DATA[1];

/*******************************************************************************
* Function Name  : I2C_TP_Init
* Description    : ��ʼ��I2CС���
* Input          : buf - ������Ϣ
* Return         : None
*******************************************************************************/
void I2C_TP_Init(char* buf)
{
  uint8_t err;

  /* config PB15 as TP_INT */
  TPINT_GPIO_(SetBits)( TPINT_Pin );
  TPINT_GPIO_(ModeCfg)( TPINT_Pin, GPIO_ModeIN_PU );
  TPINT_GPIO_(ITModeCfg)( TPINT_Pin, GPIO_ITMode_FallEdge );
  PFIC_EnableIRQ( GPIO_B_IRQn );  //TPINT_GPIO
  err = I2C_TP_SendCommand_Reset();
  if (err != 0) {
    strcpy(buf, "I2C_TP-ERR");
  }
}

/*******************************************************************************
* Function Name  : I2C_TP_SendCommand_Sleep
* Description    : I2CС��㷢��Sleep����
* Input          : None
* Return         : 0: success, others: failed
*******************************************************************************/
uint8_t I2C_TP_SendCommand_Sleep(void)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData( 0x22 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x01 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x08 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
* Function Name  : I2C_TP_SendCommand_Wakeup
* Description    : I2CС��㷢��Wakeup����
* Input          : None
* Return         : 0: success, others: failed
*******************************************************************************/
uint8_t I2C_TP_SendCommand_Wakeup(void)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData( 0x22 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x08 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
* Function Name  : I2C_TP_SendCommand_Reset
* Description    : I2CС��㷢��Reset����
* Input          : None
* Return         : 0: success, others: failed
*******************************************************************************/
uint8_t I2C_TP_SendCommand_Reset(void)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData( 0x25 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x06 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x29 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x77 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x77 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x77 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
* Function Name  : I2C_TP_SendCommand_EnterExitIdleMode
* Description    : I2CС��㷢�ͽ�����˳�idle����
* Input          : flag: 0x1 ��  0x0
* Return         : 0: success, others: failed
*******************************************************************************/
uint8_t I2C_TP_SendCommand_EnterExitIdleMode(uint8_t flag)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData( 0x25 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x06 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x00 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x29 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x06 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( 0x06 );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData( flag );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
* Function Name  : I2C_TP_ReadPacket
* Description    : I2CС����ȡ���ݰ�
* Input          : None
* Return         : 0: success, others: failed
*******************************************************************************/
uint8_t I2C_TP_ReadPacket(void)
{
  uint8_t packet_check[4] = { 0x07, 0x00, 0x01, 0x0 };
  uint8_t* pbuf = (uint8_t*)I2CTPdat;
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( I2C_TP_ADDR, I2C_Direction_Receiver );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, RESET);
  I2C_GenerateSTOP(DISABLE);
  I2C_AcknowledgeConfig(ENABLE);
  /* packet check 0 */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  if (I2C_ReceiveData( ) != packet_check[0]) pbuf = packet_check;
  /* packet check 1 */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  if (I2C_ReceiveData( ) != packet_check[1]) pbuf = packet_check;
  /* packet check 2 */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  if (I2C_ReceiveData( ) != packet_check[2]) pbuf = packet_check;
  /* receive packet */
  /* left/right/middle btn */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  packet_check[3] = I2C_ReceiveData( );
  pbuf++;
  /* Xmovement */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  *pbuf++ = I2C_ReceiveData( );
  /* Ymovement */
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  *pbuf++ = I2C_ReceiveData( );
  /* Reserved */
  I2C_AcknowledgeConfig(DISABLE);
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  packet_check[3] = I2C_ReceiveData( );
  I2C_GenerateSTOP(ENABLE);
  I2C_AcknowledgeConfig(ENABLE);

  if (pbuf == &packet_check[3]) return 0xFF;
  else return err;
}

/*******************************************************************************
* Function Name  : I2C_TP_IT_handler
* Description    : I2CС����жϴ�����
* Input          : None
* Return         : None
*******************************************************************************/
void I2C_TP_IT_handler(void)
{
  if ( TPINT_GPIO_(ReadITFlagBit)( TPINT_Pin ) != 0 ) {
    TPINT_GPIO_(ClearITFlagBit)( TPINT_Pin );
    I2C_TP_data_ready = 1;
  }
}
