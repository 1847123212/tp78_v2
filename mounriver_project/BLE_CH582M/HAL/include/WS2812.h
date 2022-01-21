/********************************** (C) COPYRIGHT *******************************
* File Name          : WS2812.h
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2021/12/16
* Description        :
*******************************************************************************/

#ifndef WS2812_H
  #define WS2812_H

  #define USE_PWM1    //ʹ��TMR1���PWM

#ifdef USE_PWM1
  #define WS2812_Pin          GPIO_Pin_10
  #define WS2812_GPIO_(x)     GPIOA_ ## x
#else
  #define WS2812_Pin          GPIO_Pin_14
  #define WS2812_GPIO_(x)     GPIOB_ ## x
#endif

  #define TIMING_ONE          51
  #define TIMING_ZERO         24
  #define TIMING_RESET        0

  #define GREEN_INDEX         0
  #define RED_INDEX           1
  #define BLUE_INDEX          2

  #define PWM_SEND_HIGH_BIT() TMR1_PWMActDataWidth(TIMING_ONE)    //0.85us
  #define PWM_SEND_LOW_BIT()  TMR1_PWMActDataWidth(TIMING_ZERO)   //0.4us
  #define PWM_SEND_RESET()    TMR1_PWMActDataWidth(TIMING_RESET)  //0us

  #define LED_Number          75    // 75���� - �������̲������޸Ĵ˴�

  #define Waterful_Repeat_Times    2   // ��������*2 = ��ˮ������

  typedef void (*WS2812_Style_Func) ( void );   // WS2812�仯��ʽ��Ӧ����

  extern WS2812_Style_Func led_style_func;

  void WS2812_PWM_Init( void );
  void WS2812_Style_Off( void );
  void WS2812_Style_Breath( void );
  void WS2812_Style_Waterful( void );
  void WS2812_Send( void );

#endif

