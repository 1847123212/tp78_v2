/********************************** (C) COPYRIGHT *******************************
 * File Name          : HW_I2C.c
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2022/11/30
 * Description        : Ӳ��I2C����
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

/*******************************************************************************
* Function Name  : HW_I2C_WaitUntilTimeout
* Description    : Ӳ��I2C �ȴ��������ʽ����ֱ����ʱ
* Input          : expression_func: �������ʽ����ָ��,
*                  event_flag: �¼����־λ
*                  flip: ��ת(���Կ���flip����һֱѭ��)
* Return         : 1: �������ʽδ����, 0: �������ʽ����, flip��Ϊ0���෴
*******************************************************************************/
uint8_t HW_I2C_WaitUntilTimeout(expression_func exp_func, uint32_t event_flag, uint8_t flip)
{
  volatile uint32_t timeout = HW_I2C_TIMOUT;
  if (!flip) {
    while (!exp_func(event_flag) && timeout > 0) {
      timeout--;
    }
  } else {
    while (exp_func(event_flag) && timeout > 0) {
       timeout--;
    }
  }
  return timeout == 0;
}

/*******************************************************************************
* Function Name  : HW_I2C_Init
* Description    : Ӳ��I2C GPIO��ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void HW_I2C_Init(void)
{
#ifdef HW_I2C_GPIO_REMAP
    GPIOPinRemap(ENABLE, RB_PIN_I2C);
#endif
    GPIOB_ModeCfg(HW_I2C_SCL_PIN | HW_I2C_SDA_PIN, GPIO_ModeIN_PU);
    I2C_Init(I2C_Mode_I2C, 400000, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AckAddr_7bit, HW_I2C_MASTER_ADDR);
//    I2C_Cmd(ENABLE);
}

/*******************************************************************************
* Function Name  : HW_I2C_WR_Reg
* Description    : Ӳ��I2Cд�Ĵ���
* Input          : reg: �Ĵ���, dat: Ҫд��ֵ, addr: �ӻ���ַ
* Return         : 0: success, others: failed
*******************************************************************************/
uint8_t HW_I2C_WR_Reg(uint8_t reg, uint8_t dat, uint8_t addr)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_SendData(dat);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);
  I2C_GenerateSTOP( ENABLE );

  return err;
}

/*******************************************************************************
* Function Name  : HW_I2C_RD_Reg
* Description    : Ӳ��I2C���Ĵ���
* Input          : reg: �Ĵ���, dat��ȡ���ָ��, addr: �ӻ���ַ
* Return         : 0: success, others: failed
*******************************************************************************/
uint8_t HW_I2C_RD_Reg(uint8_t reg, uint8_t *dat, uint8_t addr)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);

  /* restart */
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( addr, I2C_Direction_Receiver );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, RESET);
  I2C_GenerateSTOP(DISABLE);
  I2C_AcknowledgeConfig(DISABLE);
  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
  *dat = I2C_ReceiveData( );
  I2C_GenerateSTOP( ENABLE );
  I2C_AcknowledgeConfig(ENABLE);

  return err;
}

/*******************************************************************************
* Function Name  : HW_I2C_Muti_RD_Reg
* Description    : Ӳ��I2C�������Ĵ���
* Input          : reg: �Ĵ���, *dat: Ҫ����ֵ����ʼ��ַ, addr: �ӻ���ַ, len: ���ĳ���
* Return         : 0: success, others: failed
*******************************************************************************/
uint8_t HW_I2C_Muti_RD_Reg(uint8_t reg, uint8_t *dat, uint8_t addr, uint8_t len)
{
  uint8_t err = 0;

  err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_BUSY, SET);
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, RESET);
  I2C_SendData(reg);
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_BYTE_TRANSMITTING, RESET);

  /* restart */
  I2C_GenerateSTART( ENABLE );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_MODE_SELECT, RESET);
  I2C_Send7bitAddress( addr, I2C_Direction_Receiver );
  err += HW_I2C_WaitUntilTimeout(I2C_CheckEvent, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, RESET);
  I2C_GenerateSTOP( DISABLE );
  while (len-- != 0) {
    if (len == 0) {
      I2C_AcknowledgeConfig( DISABLE );
    } else {
      I2C_AcknowledgeConfig( ENABLE );
    }
    err += HW_I2C_WaitUntilTimeout((expression_func)I2C_GetFlagStatus, I2C_FLAG_RXNE, RESET);
    *dat++ = I2C_ReceiveData( );
  }
  I2C_GenerateSTOP( ENABLE );
  I2C_AcknowledgeConfig( ENABLE );

  return err;
}
