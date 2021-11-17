
#include "PS2Driver.h"
#include <string.h>

#define Delay_us    DelayUs
#define Delay_ms    DelayMs

Mousestate PS2dat;

/* PS/2Э���һ�ֽڣ��ɹ�����0��ʧ�ܷ���1 */
uint8_t PS2_ReadByte(uint8_t* dat)
{
    int i;
    uint8_t res = 0;
    uint8_t high = 0, parity;
    Delay_us(50);
    PS2_En_Data_Report();
    Delay_us(20);
    while (PS2CLK_State());	//��ʼλ
    for (i = 0; i < 8; i++) {
        while (!PS2CLK_State());
        while (PS2CLK_State());	//�ȴ�һ���½���
        res >>= 1;
        if (PS2DATA_State()) {
            res |= 0x80;
            ++high;
        }
    }
    while (!PS2CLK_State());
    while (PS2CLK_State());	//У��λ
    parity = PS2DATA_State();
    while (!PS2CLK_State());
    while (PS2CLK_State());	//ֹͣλ
    PS2_Dis_Data_Report();
    //if ((high & 1) != (parity != 0)) return 1;
    *dat = res;
    return 0;
}

/* PS/2Э��дһ�ֽڣ��ɹ�����0��ʧ�ܷ���1 */
uint8_t PS2_WriteByte(uint8_t dat)
{
    int i, high = 0;

    PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeOut_PP_20mA );
    PS2DATA_GPIO_(ModeCfg)( PS2DATA_Pin, GPIO_ModeOut_PP_20mA );

    PS2CLK_Clr();
    Delay_us(120);
    PS2DATA_Clr();
    Delay_us(10);
    PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeIN_PU );	//�ͷ�ʱ����

    while (PS2CLK_State());
    for (i = 0; i < 8; i++) {
        if (dat & 0x01) {
            PS2DATA_Set();
            ++high;
        }
        else PS2DATA_Clr();
        dat >>= 1;
        while (!PS2CLK_State());
        while (PS2CLK_State());
    }

    //������-У��λ
    if (high & 0x01) PS2DATA_Clr();
    else PS2DATA_Set();
    while (!PS2CLK_State());

    //������-ֹͣλ
    while (PS2CLK_State());
    PS2DATA_Set();
    while (!PS2CLK_State());

    PS2DATA_GPIO_(ModeCfg)( PS2DATA_Pin, GPIO_ModeIN_PU );	//�ͷ�������
    while (PS2CLK_State());

    if (!PS2DATA_State()) while (!PS2CLK_State());
    else return 1;
    return 0;
}

/* PS/2Э�����һ��������ݣ��ɹ�����0��ʧ�ܷ���1 */
uint8_t PS2_ReadMouseData(Mousestate* dat)
{
    int i;
    if (PS2_ReadByte((uint8_t*)dat->data) != 0) return 1;
    if (PS2_ReadByte((uint8_t*)dat->data + 1) != 0) return 1;
    if (PS2_ReadByte((uint8_t*)dat->data + 2) != 0) return 1;
    return 0;
}


/* ����PS/2��꣬�ɹ�����0��ʧ�ܷ���1 */
uint8_t PS2_Config(uint8_t reg, uint8_t res)
{
    uint8_t dat, sta;
    sta = PS2_WriteByte(reg);
    if (sta != 0)	return 1;
    sta = PS2_ReadByte(&dat);
    if (sta != 0 || dat != res)	return 1;
    return 0;
}

//׼����������
void PS2_En_Data_Report(void)
{
	PS2CLK_Set();
	PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeIN_PU );
}

//��ֹ��������
void PS2_Dis_Data_Report(void)
{
    PS2CLK_GPIO_(ModeCfg)( PS2CLK_Pin, GPIO_ModeOut_PP_20mA );
	PS2CLK_Clr();
}

//��ʼ��PS/2�ӿڣ��ɹ�����0��ʧ�ܷ���1
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
        DelayMs(50);
        PS2CLK_GPIO_(ITModeCfg)( PS2CLK_Pin, GPIO_ITMode_FallEdge );
        PFIC_EnableIRQ( GPIO_B_IRQn );  //PS2CLK_GPIO
    }
    strcpy(buf, "[READY]Mouse");
    return 0;
}

