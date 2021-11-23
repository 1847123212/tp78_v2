/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLEDDriver.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2021/11/21
 * Description        : OLED 9.1�� I2C����Դ�ļ�
 *******************************************************************************/

#include "OLEDDriver.h"

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

/* OLED I2C �ӳٺ��� */
void OLED_IIC_Delay(void)
{
	uint32_t i = 5;
	while( i-- );
}

/* OLED I2C �������� */
void OLED_IIC_Start(void)
{
	OLED_IIC_SDIN_Set();
	IIC_DELAY;

	OLED_IIC_SCLK_Set() ;
	IIC_DELAY;

	OLED_IIC_SDIN_Clr();
	IIC_DELAY;

	OLED_IIC_SCLK_Clr();
	IIC_DELAY;
}

/* OLED I2C ֹͣ���� */
void OLED_IIC_Stop(void)
{
	OLED_IIC_SDIN_Clr();
	IIC_DELAY;

	OLED_IIC_SCLK_Set();
	IIC_DELAY;

	OLED_IIC_SDIN_Set();
	IIC_DELAY;
}

void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{
    OLED_IIC_Start();
	OLED_IIC_SendByte(0x78);
	if(cmd == 0) {
	    OLED_IIC_SendByte(0x00);
	}
	else {
		OLED_IIC_SendByte(0x40);
	}
   OLED_IIC_SendByte(dat);    
   OLED_IIC_Stop(); 
}

void OLED_IIC_SendByte(uint8_t Data)
{
    uint8_t i;
	OLED_IIC_SCLK_Clr();
	for(i = 0; i < 8; i++)
	{  
		if(Data & 0x80)	{
			OLED_IIC_SDIN_Set();
		}
		else {
			OLED_IIC_SDIN_Clr();
		} 
		Data <<= 1;
		IIC_DELAY;
		OLED_IIC_SCLK_Set();
		IIC_DELAY;
		OLED_IIC_SCLK_Clr();
		IIC_DELAY;		
	}
	OLED_IIC_SDIN_Set();
	IIC_DELAY;
	OLED_IIC_SCLK_Set();
	IIC_DELAY;   
	OLED_IIC_SCLK_Clr(); 
}

/**
  * @brief  �趨OLED��ʾ�ַ���λ��
  * @param  uint8_t x, uint8_t y(xΪ�����꣬ȡֵ0~127��yΪҳ���꣬ȡֵ0~7)
  * @retval ��
  ******************************************************************************
  * @attention
  * SSD1306���
  * B0~B7 1 0 1 1  0 X2 X1 X0 ����ҳ��ַ X[2:0]:0~7 ��Ӧҳ 0~7 
  * 00~0F 0 0 0 0 X3 X2 X1 X0 �����е�ַ(����λ) ����8λ��ʼ�е�ַ�ĵ���λ 
  * 10~1F 0 0 0 1 X3 X2 X1 X0 �����е�ַ(����λ) ����8λ��ʼ�е�ַ�ĸ���λ 
  */
void OLED_Set_Pos(uint8_t x, uint8_t y)
{ 
	OLED_WR_Byte(0xb0 + y, OLED_CMD);            				//����ҳ��ַ
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);  	//�����е�ַ��4λ
	OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);      		//�����е�ַ��4λ
}   	  

/**
  * @brief  ����OLED��ʾ
  * @param  ��
  * @retval ��
  ******************************************************************************
  * @attention
  * SSD1306���
  *   8D   1 0 0 0 1  1 0 1
  * A[7:0] * * 0 1 0 A2 0 0  ��ɱ�����   A2=0,�رյ�ɱ�;A2=1,������ɱ�
  * AE/AF  1 0 1 0 1  1 1 X0 ������ʾ���� X0=0,�ر���ʾ;X0=1,������ʾ
  */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON      //������ɱ�
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON   //������ʾ
}

/**
  * @brief  �ر�OLED��ʾ
  * @param  ��
  * @retval ��
  */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF     //�رյ�ɱ�
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF  //�ر���ʾ
}

/**
  * @brief  ��������
  * @param  ��
  * @retval ��
  */
void OLED_Clear(void)
{
	uint8_t i,n;		    
	for(i = 0; i < 8; i++)  
	{  
		OLED_WR_Byte(0xb0 + i, OLED_CMD);          //����ҳ��ַ��0~7��
		OLED_WR_Byte(0x00, OLED_CMD);              //������ʾλ�á��е͵�ַ
		OLED_WR_Byte(0x10, OLED_CMD);              //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++) OLED_WR_Byte(0, OLED_DATA); 
	}                                            //������ʾ
}

/**
  * @brief  ��ָ��λ����ʾһ���ַ�,���������ַ�
  * @param  uint8_t x, uint8_t y, uint8_t chr(xΪ�����꣬ȡֵ0~127��yΪҳ���꣬ȡֵ0~7��chrΪ��ʾ�ַ�)
  * @retval ��
  */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{      	
	unsigned char c=0,i=0;	
	c = chr - ' ';                             //�õ�ƫ�ƺ��ֵ(��ӦASCII��)			
	if(x > Max_Column - 1){x = 0; y+=2;}
	if(SIZE ==16)                              //һ���ַ�ռ8��16��(2ҳ) һ��128*64������һ������ʾ16���ַ� ����ʾ4��
	{
		OLED_Set_Pos(x, y);	                     //��ʼд��һҳ
			                                       //д���ַ�ǰ64��(��һҳ���к���)
		for(i = 0; i < 8; i++)                   //ÿ��д8λ д8��
			OLED_WR_Byte(F8X16[c*16 + i], OLED_DATA);   
		OLED_Set_Pos(x, y + 1);                  //��ʼд�ڶ�ҳ
			                                       //д���ַ���64��(�ڶ�ҳ���к���)
		for(i = 0; i < 8; i++)                   //ÿ��д8λ д8��
			OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA); 
	}
	else
	{	
		OLED_Set_Pos(x, y + 1);
		for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[c][i], OLED_DATA);
	}
}

/**
  * @brief  ����m^n
  * @param  uint8_t m, uint8_t n
  * @retval u32(����m^n)
  */
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--) result*=m;    
	return result;
}				  

/**
  * @brief  ��ʾ1������
  * @param  uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size
  *         (xΪ�����꣬ȡֵ0~127��yΪҳ���꣬ȡֵ0~7��numΪ��Ҫ��ʾ�����֣�lenΪ���ֳ��ȣ�sizeΪ��ʾ�����С,Ĭ����16)
  * @retval ��
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{         	
	uint8_t t, temp;
	uint8_t enshow = 0;
	for(t = 0; t < len; t++)
	{
		temp=(num / oled_pow(10, len-t-1)) % 10;	//�����λ���ν�ÿһλ(ʮ����)ȡ��
		if(enshow == 0 && t < (len - 1))
		{
			if(temp == 0)
			{
				OLED_ShowChar(x +(SIZE / 2) * t, y, ' ');	//����ͷΪ0��λ�Գ�' '    
				continue;
			}else enshow = 1; 
		}
	 	OLED_ShowChar(x + (SIZE / 2) * t, y, temp + '0');	//��ӡ����
	}
} 

/**
  * @brief  ��ʾ�ַ���
  * @param  uint8_t x, uint8_t y, uint8_t *chr, uint8_t SIZE
  *		    (xΪ�����꣬ȡֵ0~127��yΪҳ���꣬ȡֵ0~7��*chrָ���ʹ��chr[](��Ҫ������ַ���)����ָ�룻SIZEΪ�����С)
  * @retval ��
  */
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr)
{
	uint8_t j=0;
	while(chr[j]!='\0')                         //��λ��Ϊ�����ӡ
	{	
		OLED_ShowChar(x, y, chr[j]);        			//�ӵ�x�е�yҳ��ӡ�ַ�
		x+=8;                                     //�ֿ�8�� ��ӡ����8��
		if(x > 120){x = 0; y+=2;}                 //��ӡ��2ҳ����2ҳ
			j++;
	}
}

/**
  * @brief  ��ʾ�ַ���
  * @param  uint8_t x, uint8_t y, uint8_t no(xΪ�����꣬ȡֵ0~127��yΪҳ���꣬ȡֵ0~7��noΪoledfont.h�д�ź�������Hzk�����)
  * @retval ��
  */
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)                           //��ӡ���ĵ�һҳ����
    {
        OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
        adder+=1;
    }
	OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)                           //��ӡ���ĵڶ�ҳ����
	{
		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
		adder+=1;
    }
}

/**
  * @brief  ��ʾ��ʾBMPͼƬ
  * @param  uint8_t x0, uint8_t y0, uint8_tx1, uint8_t y1, uint8_t BMP[]
  *         (x0Ϊ��ʼ�����ꣻy0Ϊ��ʵҳ���ꣻx1Ϊ��ֹ�����ꣻy1λ��ֹҳ���ꣻBMP[]Ϊ���ͼƬ������)
  * @retval ��
  */
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{ 	
 //BMP��С((x1-x0+1)*(y1-y0+1))
 unsigned int j=0;
 unsigned char x,y;

 for(y=y0;y<y1;y++)
 {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {
        OLED_WR_Byte(BMP[j++],OLED_DATA);	      //�����ʾ�����к���
    }
 }
} 

/**
  * @brief  OLED�����ַ���/���־���λ��x����
  * @param  ��
  * @retval x����
  */
uint8_t OLED_Midx(uint8_t size)
{
	if(size * 8 > 128) return 0;
	return (128 - size * 8) / 2;
}

/**
  * @brief  ��ʼ��SSD1306
  * @param  ��
  * @retval ��
  */
void OLED_Init(void)
{ 	
    //IO
    OLED_CLK_GPIO_(SetBits)( OLED_CLK_Pin );
    OLED_SDA_GPIO_(SetBits)( OLED_SDA_Pin );
    OLED_CLK_GPIO_(ModeCfg)( OLED_CLK_Pin, GPIO_ModeOut_PP_5mA );
    OLED_SDA_GPIO_(ModeCfg)( OLED_SDA_Pin, GPIO_ModeOut_PP_5mA );

    DelayMs(200);
	
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel

    OLED_WR_Byte(0x40,OLED_CMD);//---set low column address
    OLED_WR_Byte(0xB0,OLED_CMD);//---set high column address

    OLED_WR_Byte(0xC8,OLED_CMD);//-not offset

    OLED_WR_Byte(0x81,OLED_CMD);//���öԱȶ�
    OLED_WR_Byte(0xff,OLED_CMD);

    OLED_WR_Byte(0xa1,OLED_CMD);//���ض�������

    OLED_WR_Byte(0xa6,OLED_CMD);//

    OLED_WR_Byte(0xa8,OLED_CMD);//��������·��
    OLED_WR_Byte(0x1f,OLED_CMD);

    OLED_WR_Byte(0xd3,OLED_CMD);
    OLED_WR_Byte(0x00,OLED_CMD);

    OLED_WR_Byte(0xd5,OLED_CMD);
    OLED_WR_Byte(0xf0,OLED_CMD);

    OLED_WR_Byte(0xd9,OLED_CMD);
    OLED_WR_Byte(0x22,OLED_CMD);

    OLED_WR_Byte(0xda,OLED_CMD);
    OLED_WR_Byte(0x02,OLED_CMD);

    OLED_WR_Byte(0xdb,OLED_CMD);
    OLED_WR_Byte(0x49,OLED_CMD);

    OLED_WR_Byte(0x8d,OLED_CMD);
    OLED_WR_Byte(0x14,OLED_CMD);
	
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}
/*********************************************END OF FILE**********************/
