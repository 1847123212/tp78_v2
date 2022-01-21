/********************************** (C) COPYRIGHT *******************************
 * File Name          : WS2812.c
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2021/1/16
 * Description        : WS2812����Դ����
 *******************************************************************************/

#include "HAL.h"

__attribute__((aligned(4))) UINT32 LED_BYTE_Buffer[LED_Number*24 + 42] = { TIMING_RESET };  // LED��PWM���巭ת����ֵ��������RESETʱ��Ϊ42*1.25us
WS2812_Style_Func led_style_func = WS2812_Style_Off;  // Ĭ�ϱ��⺯��
static uint32_t style_cnt = 0;
static uint8_t breath_dir = 0;

/*******************************************************************************
* Function Name  : WS2812_PWM_Init
* Description    : ��ʼ��PWM
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WS2812_PWM_Init( void )
{
#ifdef USE_PWM1
  // ��ʼ�� TMR1 PA10 �������PWM
  WS2812_GPIO_(SetBits)( WS2812_Pin );
  WS2812_GPIO_(ModeCfg)( WS2812_Pin, GPIO_ModeOut_PP_5mA );
  WS2812_GPIO_(ResetBits)( WS2812_Pin );

  TMR1_PWMInit( High_Level, PWM_Times_1 );
  TMR1_PWMCycleCfg( 75 );        // ���� 1.25us

  style_cnt = 0;
  WS2812_Style_Off( );

#else
  // ��ʼ�� PB14 ���PWM
  WS2812_GPIO_(ModeCfg)( WS2812_Pin, GPIO_ModeOut_PP_5mA );    // PB14 - PWM10
  PWMX_CLKCfg( 1 );                   // cycle = 1/Fsys = 1/60us
  PWMX_CycleCfg( PWMX_Cycle_64 );     // ����T = 64*cycle = 1.067us ~ 1.25us +- 600ns
  PWMX_ACTOUT( CH_PWM10, 0, Low_Level, ENABLE );     // 0% ռ�ձ�
#endif
}

/*******************************************************************************
* Function Name  : WS2812_Style_Off
* Description    : �ر�WS2812
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WS2812_Style_Off( void )
{
  uint16_t i;
  for (i = 0; i < LED_Number*24; i++) LED_BYTE_Buffer[i] = TIMING_ZERO;
}

/*******************************************************************************
* Function Name  : WS2812_Style_Breath
* Description    : PWM����WS2812�����Ʊ仯����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WS2812_Style_Breath( void )
{
  uint16_t i, j, memaddr = 0;
  for (i = 0; i < LED_Number; i++)
  {
    for (j = 0; j < 8; j++) // GREEN data
    {
        LED_BYTE_Buffer[memaddr] = ((style_cnt<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
        memaddr++;
    }
    for (j = 0; j < 8; j++) // RED data
    {
        LED_BYTE_Buffer[memaddr] = ((style_cnt<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
        memaddr++;
    }
    for (j = 0; j < 8; j++) // BLUE data
    {
        LED_BYTE_Buffer[memaddr] = ((style_cnt<<j) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
        memaddr++;
    }
  }
  if (breath_dir == 0) ++style_cnt;
  else --style_cnt;
  if (style_cnt == 15 || style_cnt == 0 ) {
    breath_dir = !breath_dir;
  }
}

/*******************************************************************************
* Function Name  : WS2812_Style_Waterful
* Description    : PWM����WS2812��ˮ�Ʊ仯����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WS2812_Style_Waterful( void )
{
  uint16_t j;
  uint32_t slow_cnt;
  if (style_cnt % Waterful_Repeat_Times != 0) {  // ��������*Waterful_Repeat_Times = ��ˮ������
    ++style_cnt;
    if (style_cnt >= LED_Number * 3 * Waterful_Repeat_Times ) { // GRB�����л� + 120ms�ƶ�һ����
      style_cnt = 0;
    }
    return;
  } else {
    slow_cnt = style_cnt / Waterful_Repeat_Times;
  }

  // �ر���һ����
  uint32_t last_cnt = slow_cnt == 0 ? LED_Number-1 : slow_cnt-1;
  for (j = 0; j < 24; j++) {
    LED_BYTE_Buffer[(last_cnt % LED_Number) * 24 + j] = TIMING_ZERO;
  }
  // ������һ����
  for (j = 0; j < 24; j++) {
    if ( j >= slow_cnt / LED_Number * 8 + 4 && j < slow_cnt / LED_Number * 8 + 8 ) {
      LED_BYTE_Buffer[(slow_cnt % LED_Number) * 24 + j] = TIMING_ONE;
    } else {
      LED_BYTE_Buffer[(slow_cnt % LED_Number) * 24 + j] = TIMING_ZERO;
    }
  }
  ++style_cnt;
  if (style_cnt >= LED_Number * 3 * Waterful_Repeat_Times ) { // GRB�����л� + 120ms�ƶ�һ����
    style_cnt = 0;
  }
}

/*******************************************************************************
* Function Name  : WS2812����RGB����
* Description    : DMA+PWM����WS2812
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WS2812_Send( void )
{
  uint16_t i;

  if ( LED_Change_flag != 0 ) {
    LED_Change_flag = 0;
    style_cnt = breath_dir = 0;
    WS2812_Style_Off( );
  } else {
    led_style_func( ); // ���ñ仯����
  }

  TMR1_DMACfg( ENABLE, (UINT16) (UINT32) LED_BYTE_Buffer, (UINT16) (UINT32) (LED_BYTE_Buffer + LED_Number*24 + 42), Mode_Single );  // ����DMAת�������ڴ浽����
}

