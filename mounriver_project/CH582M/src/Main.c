/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : TP87
 * Version            : V1.0
 * Date               : 2021/11/17
 * Description 		  : USB���̲���
 *******************************************************************************/

/********************************** (C) COPYRIGHT *******************************
   *  ���ŷ��䣺PA8 & PA9 --- UART1; PB10 & PB11 --- USB1; PA0 ~ PA7 & PA10 ~ PA15 & PB12 ~ PB13 & PB16 ~ PB19 --- KeyBoard
 *          PB14 & PB15 --- PS/2 Mouse; PB8 & PB9 --- OLED; PB0 --- LED
 ********************************* (C) COPYRIGHT ********************************/

#include "CH58x_common.h"
#include "PS2Driver.h"
#include "UART1.h"
#include "USBDriver.h"
#include "OLEDDriver.h"
#include "KeyboardDriver.h"

char buf[520];
const uint32_t aaa[] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14};

int main()
{
  Mousestate myData;
  uint8_t res;

  SetSysClock( CLK_SOURCE_PLL_60MHz );

  //led
  GPIOB_SetBits( GPIO_Pin_0 );
  GPIOB_ModeCfg( GPIO_Pin_0, GPIO_ModeOut_PP_5mA );

  //led off
  GPIOB_ResetBits( GPIO_Pin_0 );

  //uart1
  UART1_Init();
  printf("hello\n");

  //oled
  OLED_Init();
  OLED_ShowString(64, 2, "hello");

  //Keyboard
  KEY_Init();

  //PS/2
  if (PS2_Init(buf, 1) != 0) {
      printf("%s\n", buf);
      while (1);
  }
  else {
      printf("mouse ready\n");
  }

  //USB
  pEP0_RAM_Addr = EP0_Databuf;
  pEP1_RAM_Addr = EP1_Databuf;
  pEP2_RAM_Addr = EP2_Databuf;
  pEP3_RAM_Addr = EP3_Databuf;
  USB_DeviceInit();
  PFIC_EnableIRQ( USB_IRQn );

  while (1) {
      if (PS2_data_ready != 0) {    //USB����С����������
          PS2_data_ready = 0;
          if (PS2_byte_cnt == 3) {
              PS2_byte_cnt = 0;
              memcpy(pEP2_IN_DataBuf, PS2dat, 4);
              DevEP2_IN_Deal( 4 );
          }
          PS2_En_Data_Report();
      }
      KEY_RisingEdge_detection();   //���������
      if (KEY_data_ready != 0) {    //USB���ͼ�������
          KEY_data_ready = 0;
          memcpy(pEP1_IN_DataBuf, Keyboarddat, 8);
          DevEP1_IN_Deal( 8 );
          memset(Keyboarddat, 0, 8);
      }
      KEY_FallEdge_detection(); //����½���
  }

}

__INTERRUPT
__HIGH_CODE
void GPIOA_IRQHandler( void )   //GPIOA�ⲿ�ж�
{
    KEY_EX_IT_handler();
}

__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler( void )   //GPIOB�ⲿ�ж�
{
    PS2_IT_handler();
}

__INTERRUPT
__HIGH_CODE
void TMR0_IRQHandler( void )    //TMR0��ʱ�ж�
{

}

__INTERRUPT
__HIGH_CODE
void USB_IRQHandler( void )     //USB�жϷ������,ʹ�üĴ�����1
{
    USB_DevTransProcess();
}
