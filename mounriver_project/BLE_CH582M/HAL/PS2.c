/********************************** (C) COPYRIGHT *******************************
 * File Name          : PS2.c
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2022/1/13
 * Description        : PS/2����Դ�ļ�
 *******************************************************************************/

#include "HAL.h"
#include <string.h>

#define Delay_us    DelayUs
#define Delay_ms    DelayMs

uint8_t PS2_bit_cnt = 0,
        PS2_byte_cnt = 0,
        PS2_byte = 0,
        PS2_high_cnt = 0,
        PS2_data_ready = 0;
Mousestate* const PS2dat = (Mousestate*)HIDMouse;

/*******************************************************************************
* Function Name  : PS2_WaitCLKState
* Description    : PS/2Э��ȴ�CLK״̬�仯
* Input          : wait_high - TRUE:�ȴ�CLK�ӵͱ��
*                              FALSE:�ȴ�CLK�Ӹ߱��
* Output         : None
* Return         : �ɹ�����0, ʧ�ܷ���1
*******************************************************************************/
uint8_t PS2_WaitCLKState( BOOL wait_high )
{
    uint32_t timeout = PS2_TIMEOUT;
    if ( wait_high ) {
      while (!PS2CLK_State()) {  //�ȴ�CLK��
        if (timeout == 0) return 1;
        timeout--;
        DelayUs(1);
      }
    } else {
      while (PS2CLK_State()) {  //�ȴ�CLK��
        if (timeout == 0) return 1;
        timeout--;
        DelayUs(1);
      }
    }
    return 0;
}

/*******************************************************************************
* Function Name  : PS2_ReadByte
* Description    : PS/2Э���һ�ֽ�
* Input          : dat - ����ȡ�������ݷŵ�datָ��ָ��Ŀռ���
* Output         : None
* Return         : �ɹ�����0, ʧ�ܷ���1
*******************************************************************************/
uint8_t PS2_ReadByte(uint8_t* dat)
{
    int i;
    uint8_t res = 0;
    uint8_t high = 0, parity;

    Delay_us(50);
    PS2_En_Data_Report();
    Delay_us(20);

    if ( PS2_WaitCLKState(FALSE) ) return 1;  //��ʼλ
    for (i = 0; i < 8; i++) {
        if ( PS2_WaitCLKState(TRUE) ) return 1;
        if ( PS2_WaitCLKState(FALSE) ) return 1;	//�ȴ�һ���½���
        res >>= 1;
        if (PS2DATA_State()) {
            res |= 0x80;
            ++high;
        }
    }
    if ( PS2_WaitCLKState(TRUE) ) return 1;
    if ( PS2_WaitCLKState(FALSE) ) return 1;	//У��λ
    parity = PS2DATA_State();
    if ( PS2_WaitCLKState(TRUE) ) return 1;
    if ( PS2_WaitCLKState(FALSE) ) return 1;	//ֹͣλ
    PS2_Dis_Data_Report();
    //if ((high & 1) != (parity != 0)) return 1;
    *dat = res;
    return 0;
}

/*******************************************************************************
* Function Name  : PS2_WriteByte
* Description    : PS/2Э��дһ�ֽ�
* Input          : dat - Ҫд���ֵ
* Output         : None
* Return         : �ɹ�����0, ʧ�ܷ���1
*******************************************************************************/
uint8_t PS2_WriteByte(uint8_t dat)
{
    int i, high = 0;
    PS2CLK_GPIO_(SetBits)( PS2CLK_Pin );
    PS2DATA_GPIO_(SetBits)( PS2DATA_Pin );
    PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeOut_PP_5mA );
    PS2DATA_GPIO_(ModeCfg)( PS2DATA_Pin, GPIO_ModeOut_PP_5mA );

    PS2CLK_Clr();
    Delay_us(120);
    PS2DATA_Clr();
    Delay_us(10);
    PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeIN_PU );	//�ͷ�ʱ����

    if ( PS2_WaitCLKState(FALSE) ) return 1;

    for (i = 0; i < 8; i++) {
        if (dat & 0x01) {
            PS2DATA_Set();
            ++high;
        }
        else PS2DATA_Clr();
        dat >>= 1;
        if ( PS2_WaitCLKState(TRUE) ) return 1;
        if ( PS2_WaitCLKState(FALSE) ) return 1;
    }

    //������-У��λ
    if (high & 0x01) PS2DATA_Clr();
    else PS2DATA_Set();
    if ( PS2_WaitCLKState(TRUE) ) return 1;

    //������-ֹͣλ
    if ( PS2_WaitCLKState(FALSE) ) return 1;
    PS2DATA_Set();
    if ( PS2_WaitCLKState(TRUE) ) return 1;

    PS2DATA_GPIO_(ModeCfg)( PS2DATA_Pin, GPIO_ModeIN_PU );	//�ͷ�������
    if ( PS2_WaitCLKState(FALSE) ) return 1;

    if (!PS2DATA_State()) {
      if ( PS2_WaitCLKState(TRUE) ) return 1;
    }
    else return 1;
    return 0;
}

/*******************************************************************************
* Function Name  : PS2_ReadMouseData
* Description    : PS/2Э�����һ���������
* Input          : dat - �����յ������ݷŵ�datָ��ָ��Ŀռ���(datָ��Ŀռ�Ӧ��ռ4byte)
* Output         : None
* Return         : �ɹ�����0, ʧ�ܷ���1
*******************************************************************************/
uint8_t PS2_ReadMouseData(Mousestate* dat)
{
    int i;
    if (PS2_ReadByte((uint8_t*)dat->data) != 0) return 1;
    if (PS2_ReadByte((uint8_t*)dat->data + 1) != 0) return 1;
    if (PS2_ReadByte((uint8_t*)dat->data + 2) != 0) return 1;
    return 0;
}

/*******************************************************************************
* Function Name  : PS2_Config
* Description    : ����PS/2���
* Input          : reg - ����ֵ, res - ��Ҫ�ȴ�res����Ӧ����ɹ�
* Output         : None
* Return         : �ɹ�����0, ʧ�ܷ���1
*******************************************************************************/
uint8_t PS2_Config(uint8_t reg, uint8_t res)
{
    uint8_t dat, sta;
    sta = PS2_WriteByte(reg);
    if (sta != 0)	return 1;
    sta = PS2_ReadByte(&dat);
    if (sta != 0 || dat != res)	return 1;
    return 0;
}

/*******************************************************************************
* Function Name  : PS2_En_Data_Report
* Description    : ʹ��PS/2���ݴ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PS2_En_Data_Report(void)
{
	PS2CLK_Set();
	PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeIN_PU );
}

/*******************************************************************************
* Function Name  : PS2_Dis_Data_Report
* Description    : ��ֹPS/2���ݴ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PS2_Dis_Data_Report(void)
{
    PS2CLK_GPIO_(SetBits)( PS2CLK_Pin );
    PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeOut_PP_20mA );
	PS2CLK_Clr();
}

/*******************************************************************************
* Function Name  : PS2_IT_handler
* Description    : PS/2�жϴ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PS2_IT_handler(void)
{
    if ( PS2CLK_GPIO_(ReadITFlagBit)( PS2CLK_Pin ) != 0 ) {
        PS2CLK_GPIO_(ClearITFlagBit)( PS2CLK_Pin );
        ++PS2_bit_cnt;
        if (PS2_bit_cnt == 1) { //��ʼλ
            PS2_byte = 0;
            PS2_high_cnt = 0;
        }
        else if (PS2_bit_cnt <= 9) {
            PS2_byte >>= 1;
            if (PS2DATA_State()) {
                PS2_byte |= 0x80;
                ++PS2_high_cnt;
            }
        }
        else if (PS2_bit_cnt == 10) {   //У��λ
//            if ((PS2_high_cnt & 1) == (PS2DATA_State() != 0)) {
//                PS2_data_ready = 2;
//            }
        }
        else if (PS2_bit_cnt == 11) {   //ֹͣλ
            if ((PS2_byte_cnt == 0 && (PS2_byte & 0x8)) || PS2_byte_cnt > 0) {    //���Always1λ�Ƿ�Ϊ1
                if ( PS2_byte_cnt > 0 ) PS2dat->data[PS2_byte_cnt] = PS2_byte;  //С��㲻���ܰ�����Ϣ
                PS2_byte_cnt++;
            }
            PS2_Dis_Data_Report();
            PS2_bit_cnt = 0;
            PS2_data_ready = 1;
        }
    }
}

/*******************************************************************************
* Function Name  : PS2_Init
* Description    : ��ʼ��PS/2���
* Input          : None
* Output         : None
* Return         : �ɹ�����0��ʧ�ܷ���1
*******************************************************************************/
uint8_t PS2_Init(char* buf, BOOL is_IT)
{ 	
  uint8_t res, sta;

  //IO
	PS2CLK_GPIO_(SetBits)( PS2CLK_Pin );
	PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeOut_PP_20mA );
	PS2_Dis_Data_Report();
	PS2DATA_GPIO_(ModeCfg)( PS2DATA_Pin, GPIO_ModeIN_PU );

	//config
	sta = PS2_Config(PS_RESET, 0xFA);
	if (sta != 0) {
	    strcpy(buf, "[ERROR]PS_RESET STEP1");
	    return 1;
	}
	sta = PS2_ReadByte(&res);   //AA
    if (sta != 0 || res != 0xAA) {
        strcpy(buf, "[ERROR]PS_RESET STEP2");
        return 1;
    }
    sta = PS2_ReadByte(&res);   //ID�ţ�0
    if (sta != 0 || res != 0x00) {
        strcpy(buf, "[ERROR]PS_RESET STEP3");
        return 1;
    }
    sta = PS2_Config(SET_DEFAULT, 0xFA);
    if (sta != 0) {
        strcpy(buf, "[ERROR]SET_DEFAULT");
        return 1;
    }
    sta = PS2_Config(EN_DATA_REPORT, 0xFA);
    if (sta != 0) {
        strcpy(buf, "[ERROR]EN_DATA_REPORT");
        return 1;
    }
    if (is_IT) {
        PS2_En_Data_Report();
        DelayMs(10);   //�ȴ��ȶ�
        PS2CLK_GPIO_(ITModeCfg)( PS2CLK_Pin, GPIO_ITMode_FallEdge );
        PFIC_EnableIRQ( GPIO_B_IRQn );  //PS2CLK_GPIO
    }
    strcpy(buf, "[READY]Mouse");
    return 0;
}

__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler( void )   //GPIOB�ⲿ�ж�
{
    PS2_IT_handler();
}

/****************************************************************
 *
 *  //��ʼ��д��
 *  if (PS2_Init(buf, 1) != 0) {
 *      printf("%s\n", buf);
 *      while (1);
 *  } else printf("mouse ready\n");
 *
 *  //��ѭ���ж�д��
 *  while (1) {
 *      if (PS2_data_ready != 0) {
 *          PS2_data_ready = 0;
 *          if (PS2_byte_cnt == 3) {
 *              PS2_byte_cnt = 0;
 *              printf("%d %d %d %d\n", PS2dat.LeftBtn, PS2dat.RightBtn, PS2dat.XMovement, PS2dat.YMovement);
 *          }
 *          PS2_En_Data_Report();
 *      }
 *      DelayUs(50);
 *  }
 *
 *  //��ѭ�����ж�д��
 *  while (1) {
 *      PS2_ReadMouseData(&PS2dat);
 *      printf("%d %d %d %d\n", PS2dat.LeftBtn, PS2dat.RightBtn, PS2dat.XMovement, PS2dat.YMovement);
 *  }
 *
 *  //�ⲿ�жϵ���
 *  PS2_IT_handler();
 *
 ****************************************************************/
