/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2021/12/06
* Description        : TP78С���+��������Ӧ��������������ϵͳ��ʼ��
*******************************************************************************/

/********************************** (C) COPYRIGHT *******************************
 *  M.2���ŷ��䣺    PB10 & PB11 --- USB1; PA0 ~ PA7 & PA10 ~ PA15 & PB2 ~ PB7 --- KeyBoard
 *                PB13 & PB12 --- PS/2 Mouse; PB9 & PB8 --- OLED; PB14 --- W2812
 *                PA8 & PA9 --- Battery / UART1(download)
 *  ���İ����ţ�       PB0 --- LED; PB1 --- KEY
********************************* (C) COPYRIGHT ********************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "CH58x_common.h"
#include "HAL.h"
#include "hiddev.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84,0xC2,0xE4,0x03,0x02,0x02};
#endif

/*******************************************************************************
* Function Name  : Main_Circulation
* Description    : ��ѭ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__HIGH_CODE
void Main_Circulation()
{
  while(1){
    TMOS_SystemProcess( );
  }
}

/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
#if (defined (DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
  PWR_DCDCCfg( ENABLE );
#endif
  SetSysClock( CLK_SOURCE_PLL_60MHz );
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  GPIOA_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
  GPIOB_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
#endif
#ifdef DEBUG
  GPIOA_SetBits(bTXD1);
  GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
  UART1_DefInit( );
#endif   
  PRINT("%s\n",VER_LIB);
  CH57X_BLEInit( );
	HAL_Init( );
	GAPRole_PeripheralInit( );
	HidDev_Init( ); 
	HidEmu_Init( );
	tmos_start_task( halTaskID, MAIN_CIRCULATION_EVENT, 10 );
  Main_Circulation();
}

/******************************** endfile @ main ******************************/
