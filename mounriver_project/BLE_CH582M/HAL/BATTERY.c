/********************************** (C) COPYRIGHT *******************************
 * File Name          : BATTERY.c
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2022/1/26
 * Description        : ���ADC����Դ�ļ�
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

__attribute__((aligned(2))) UINT16 BAT_abcBuff[ADC_MAXBUFLEN];
UINT32 BAT_adcVal = 0;
BOOL BAT_chrg = FALSE;
UINT32 BAT_adcHistory = 0;
static signed short RoughCalib_Value = 0;

/*******************************************************************************
* Function Name  : BATTERY_Init
* Description    : ���ADC��ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_Init( void )
{
  uint8_t i;
//  ADC_InterTSSampInit();
//  RoughCalib_Value = ADC_DataCalib_Rough(); // ���ڼ���ADC�ڲ�ƫ���¼������ RoughCalib_Value��
  // bat charging io
  GPIOB_ModeCfg( GPIO_Pin_14, GPIO_ModeIN_PU );
  // adc init
  GPIOA_ModeCfg( GPIO_Pin_8, GPIO_ModeIN_Floating );
  ADC_ExtSingleChSampInit( SampleFreq_3_2, ADC_PGA_0 );
  ADC_ChannelCfg( 12 );
  ADC_StartUp();
  R8_ADC_AUTO_CYCLE = 0;    // ADC�Զ�ת�����ڣ�256*16/60000000s = 68us
  BATTERY_DMA_ENABLE( );
  tmos_start_task( halTaskID, BATTERY_EVENT, MS1_TO_SYSTEM_TIME(1000) );  // �ȴ��ȶ���1s
}

/*******************************************************************************
* Function Name  : BATTERY_DMA_ENABLE
* Description    : ���ADC DMAʹ��, ��ת�����ͨ��DMA����BAT_abcBuff��
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_DMA_ENABLE( void )
{
  R16_ADC_DMA_BEG = (UINT16) (UINT32) &BAT_abcBuff[0];
  R16_ADC_DMA_END = (UINT16) (UINT32) &BAT_abcBuff[ADC_MAXBUFLEN];
  R8_ADC_CTRL_DMA = RB_ADC_DMA_ENABLE | RB_ADC_AUTO_EN | RB_ADC_CONT_EN; // ��ʱ����Զ�����ADC����; ADC����ת��ģʽ; DMA��ַѭ������ʹ��
}

/*******************************************************************************
* Function Name  : BATTERY_ADC_Convert
* Description    : ���ADC����ת����ȡ��ѹֵ
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_ADC_Convert( void )
{
  uint8_t i;
  for(i = 0; i < ADC_MAXBUFLEN; i++) {
    BAT_abcBuff[i] = ADC_ExcutSingleConver();      // ��������ADC_MAXBUFLEN��
  }
  BATTERY_ADC_Calculation( );
//  OLED_PRINT("ADC: %d", BAT_adcVal);
}

/*******************************************************************************
* Function Name  : BATTERY_ADC_Calculation
* Description    : ������ADC��ѹֵ, ������BAT_adcVal��
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_ADC_Calculation( void )
{
  uint8_t i;
  UINT32 BAT_adcVal_tmp = 0;
  for (i = 0; i < ADC_MAXBUFLEN; i++) {
    BAT_adcVal_tmp += BAT_abcBuff[i] + RoughCalib_Value;
  }
  BAT_adcVal_tmp /= ADC_MAXBUFLEN;
  if ( BAT_adcHistory > 4 * BAT_FLOATING_VAL + BAT_adcVal_tmp ) return;  // ͻȻ����
  BAT_adcHistory = BAT_adcVal;
  BAT_adcVal = BAT_adcVal_tmp;
}

/*******************************************************************************
* Function Name  : BATTERY_ADC_GetLevel
* Description    : ��ȡ���ADC�ȼ�
* Input          : ADCֵ
* Return         : �����ȼ�
*******************************************************************************/
static UINT8 BATTERY_ADC_GetLevel( UINT32 adc_val )
{
#ifdef OLED_0_91
  if ( adc_val < BAT_MINADCVAL ) return 0;
  else if ( adc_val < BAT_20PERCENT_VAL ) return 1;
  else if ( adc_val < BAT_40PERCENT_VAL ) return 2;
  else if ( adc_val < BAT_60PERCENT_VAL ) return 3;
  else if ( adc_val < BAT_80PERCENT_VAL ) return 4;
  else return 5;
#endif
#ifdef OLED_0_66
  if ( adc_val < BAT_MINADCVAL ) return 0;
  else if ( adc_val < BAT_33PERCENT_VAL ) return 1;
  else if ( adc_val < BAT_67PERCENT_VAL ) return 2;
  else return 3;
#endif
}

/*******************************************************************************
* Function Name  : BATTERY_DrawBMP
* Description    : ���Ƶ��ͼ��(�������ȼ��ı�Ż���)
* Input          : None
* Return         : None
*******************************************************************************/
void BATTERY_DrawBMP( void )
{
  const UINT8 BMP_StartX = 91;
  UINT8 i, j;
  UINT8 BAT_level = BATTERY_ADC_GetLevel(BAT_adcVal);
  BOOL isFloating = ABS((long)BAT_adcHistory - (long)BAT_adcVal) >= BAT_FLOATING_VAL;
  if (BATTERY_ADC_GetLevel(BAT_adcHistory) != BAT_level) { // �����ȼ��仯
    OLED_UI_add_default_task(OLED_UI_FLAG_BAT_LEVEL_1 + BAT_level);
  }
  // ���۵����ȼ��Ƿ�仯�����������Ƿ񸡶�
  if ( isFloating ) {
    OLED_Set_Pos(BMP_StartX + 29, 0);
    OLED_WR_Byte(0xE1, OLED_DATA);
    OLED_WR_Byte(0x0B, OLED_DATA);
  } else {
    OLED_Set_Pos(BMP_StartX + 29, 0);
    OLED_WR_Byte(0xE0, OLED_DATA);
    OLED_WR_Byte(0x00, OLED_DATA);
  }
  // ���۵����ȼ��Ƿ�仯�����ADCֵ
  OLED_Set_Pos(BMP_StartX + 4, 3);
  OLED_ShowNum(BMP_StartX + 4, 3, BAT_adcVal, 4);
}
