/********************************** (C) COPYRIGHT *******************************
 * File Name          : BATTERY.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/12/15
 * Description        : ���ADC����Դ�ļ�
 *******************************************************************************/

#include "HAL.h"

__attribute__((aligned(2))) UINT16 BAT_abcBuff[ADC_MAXBUFLEN];
UINT32 BAT_adcVal = 0;
static UINT32 BAT_adcHistory = 0;
static signed short RoughCalib_Value = 0;

/*******************************************************************************
* Function Name  : BATTERY_Init
* Description    : ���ADC��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BATTERY_Init( void )
{
  uint8_t i;
//  ADC_InterTSSampInit();
//  RoughCalib_Value = ADC_DataCalib_Rough(); // ���ڼ���ADC�ڲ�ƫ���¼������ RoughCalib_Value��
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
* Output         : None
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
* Output         : None
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
* Output         : None
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
  BAT_adcHistory = BAT_adcVal;
  BAT_adcVal = BAT_adcVal_tmp;
}

/*******************************************************************************
* Function Name  : BATTERY_ADC_GetLevel
* Description    : ��ȡ���ADC�ȼ�(1:0%~25%, 2:25%~50%, 3:50%~75%, 4:75%~100%, 0��5��������Χ)
* Input          : ADCֵ
* Output         : None
* Return         : 0/1/2/3/4
*******************************************************************************/
static UINT8 BATTERY_ADC_GetLevel( UINT32 adc_val )
{
  if ( adc_val < BAT_MINADCVAL ) return 0;
  else if ( adc_val < BAT_25PERCENT_VAL ) return 1;
  else if ( adc_val < BAT_50PERCENT_VAL ) return 2;
  else if ( adc_val < BAT_75PERCENT_VAL ) return 3;
  else if ( adc_val < BAT_MAXADCVAL ) return 4;
  else return 5;
}

/*******************************************************************************
* Function Name  : BATTERY_DrawBMP
* Description    : ���Ƶ��ͼ��(�������ȼ��ı�Ż���)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BATTERY_DrawBMP( void )
{
  UINT8 i, j;
  UINT8 BAT_level = BATTERY_ADC_GetLevel(BAT_adcVal);
  if (BATTERY_ADC_GetLevel(BAT_adcHistory) != BAT_level) { // �����ȼ��仯
//    OLED_Clear();
    OLED_PRINT("%4d", BAT_adcVal); // �����ǰADC����ֵ
    OLED_DrawBMP(92, 0, 128, 4, (uint8_t*)EmptyBattery);  // ���ƿյ��
    OLED_Set_Pos(96, 1);
    for (i = 0; i < BAT_level; i++) { // ���Ƶ���(�ϰ벿��)
      for (j = 0; j < 4; j++) {
        OLED_WR_Byte(0xFD, OLED_DATA);
      }
      OLED_WR_Byte(0x01, OLED_DATA);
    }
    OLED_Set_Pos(96, 2);
    for (i = 0; i < BAT_level; i++) { // ���Ƶ���(�°벿��)
      for (j = 0; j < 4; j++) {
        OLED_WR_Byte(0xBF, OLED_DATA);
      }
      OLED_WR_Byte(0x80, OLED_DATA);
    }
  }
}
