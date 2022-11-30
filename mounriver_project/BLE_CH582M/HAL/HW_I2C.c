/********************************** (C) COPYRIGHT *******************************
 * File Name          : HW_I2C.c
 * Author             : ChnMasterOG
 * Version            : V2.0
 * Date               : 2022/11/30
 * Description        : 硬件I2C驱动
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HW_I2C.h"

/*******************************************************************************
* Function Name  : HW_I2C_Init
* Description    : 硬件I2C GPIO初始化
* Input          : None
* Return         : None
*******************************************************************************/
void HW_I2C_Init(void)
{
    GPIOB_ModeCfg(HW_I2C_SCL_PIN | HW_I2C_SDA_PIN, GPIO_ModeIN_PU);
    I2C_Init(I2C_Mode_I2C, 400000, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AckAddr_7bit, HW_I2C_MASTER_ADDR);
#if (defined HW_GPIO_REMAP) && (HW_GPIO_REMAP == TRUE)
    GPIOPinRemap(ENABLE, RB_PIN_I2C);
#endif
}

/*******************************************************************************
* Function Name  : HW_I2C_WR_Reg
* Description    : 硬件I2C写寄存器
* Input          : reg: 寄存器, dat: 要写的值, addr: 从机地址
* Return         : None
*******************************************************************************/
void HW_I2C_WR_Reg(uint8_t reg, uint8_t dat, uint8_t addr)
{
    while( I2C_GetFlagStatus( I2C_FLAG_BUSY ) != RESET );
    I2C_GenerateSTART( ENABLE );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );
    I2C_SendData(reg);
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTING ) );
    I2C_SendData(dat);
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTING ) );
    I2C_GenerateSTOP( ENABLE );
}

/*******************************************************************************
* Function Name  : HW_I2C_RD_Reg
* Description    : 硬件I2C读寄存器
* Input          : reg: 寄存器, addr: 从机地址
* Return         : dat读取结果
*******************************************************************************/
uint8_t HW_I2C_RD_Reg(uint8_t reg, uint8_t addr)
{
    uint8_t dat;
    while( I2C_GetFlagStatus( I2C_FLAG_BUSY ) != RESET );
    I2C_GenerateSTART( ENABLE );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );
    I2C_SendData(reg);
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTING ) );

    /* restart */
    I2C_GenerateSTART( ENABLE );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( addr, I2C_Direction_Receiver );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) );
    I2C_GenerateSTOP(DISABLE);
    I2C_AcknowledgeConfig(DISABLE);
    while( !I2C_GetFlagStatus( I2C_FLAG_RXNE ) );
    dat = I2C_ReceiveData( );
    I2C_GenerateSTOP( ENABLE );
    I2C_AcknowledgeConfig(ENABLE);

    return dat;
}

/*******************************************************************************
* Function Name  : HW_I2C_Muti_RD_Reg
* Description    : 硬件I2C连续读寄存器
* Input          : reg: 寄存器, *dat: 要读的值的起始地址, addr: 从机地址, len: 读的长度
* Return         : None
*******************************************************************************/
void HW_I2C_Muti_RD_Reg(uint8_t reg, uint8_t* dat, uint8_t addr, uint8_t len)
{
    while( I2C_GetFlagStatus( I2C_FLAG_BUSY ) != RESET );
    I2C_GenerateSTART( ENABLE );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( addr, I2C_Direction_Transmitter );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );
    I2C_SendData(reg);
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTING ) );

    /* restart */
    I2C_GenerateSTART( ENABLE );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( addr, I2C_Direction_Receiver );
    while( !I2C_CheckEvent( I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) );
    I2C_GenerateSTOP( DISABLE );
    while (len-- != 0) {
        if (len == 0) {
            I2C_AcknowledgeConfig( DISABLE );
        } else {
            I2C_AcknowledgeConfig( ENABLE );
        }
        while( !I2C_GetFlagStatus( I2C_FLAG_RXNE ) );
        *dat++ = I2C_ReceiveData( );
    }
    I2C_GenerateSTOP( ENABLE );
    I2C_AcknowledgeConfig( ENABLE );
}
