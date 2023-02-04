/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED.c
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2022/3/20
 * Description        : OLED ����Դ�ļ�
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

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

/*******************************************************************************
* Function Name  : OLED_SW_I2C_Delay
* Description    : OLED���ģ��I2C-�ӳٺ���
* Input          : None
* Return         : None
*******************************************************************************/
static void OLED_SW_I2C_Delay(void)
{
	uint32_t i = 5;
	while( i-- );
}

/*******************************************************************************
* Function Name  : OLED_SW_I2C_Start
* Description    : OLED���ģ��I2C-Start�ź�
* Input          : None
* Return         : None
*******************************************************************************/
static void OLED_SW_I2C_Start(void)
{
	OLED_IIC_SDIN_Set();
	I2C_DELAY;

	OLED_IIC_SCLK_Set() ;
	I2C_DELAY;

	OLED_IIC_SDIN_Clr();
	I2C_DELAY;

	OLED_IIC_SCLK_Clr();
	I2C_DELAY;
}

/*******************************************************************************
* Function Name  : OLED_SW_I2C_Stop
* Description    : OLED���ģ��I2C-Stop�ź�
* Input          : None
* Return         : None
*******************************************************************************/
static void OLED_SW_I2C_Stop(void)
{
  OLED_IIC_SDIN_Clr();
	I2C_DELAY;

	OLED_IIC_SCLK_Set();
	I2C_DELAY;

	OLED_IIC_SDIN_Set();
	I2C_DELAY;
}

/*******************************************************************************
* Function Name  : OLED_SW_I2C_SendByte
* Description    : OLED���ģ��I2C-I2C����1�ֽ�
* Input          : Data
* Return         : None
*******************************************************************************/
static void OLED_SW_I2C_SendByte(uint8_t Data)
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
		I2C_DELAY;
		OLED_IIC_SCLK_Set();
		I2C_DELAY;
		OLED_IIC_SCLK_Clr();
		I2C_DELAY;
	}
	OLED_IIC_SDIN_Set();
	I2C_DELAY;
	OLED_IIC_SCLK_Set();
	I2C_DELAY;
	OLED_IIC_SCLK_Clr(); 
}

/*******************************************************************************
* Function Name  : OLED_SW_I2C_WR_Byte
* Description    : OLED���ģ��I2C-д�Ĵ���
* Input          : Data
* Return         : None
*******************************************************************************/
void OLED_SW_I2C_WR_Byte(uint8_t dat, uint8_t cmd)
{
  OLED_SW_I2C_Start();
  OLED_SW_I2C_SendByte(0x78);
  if(cmd == 0) {
    OLED_SW_I2C_SendByte(0x00);
  }
  else {
    OLED_SW_I2C_SendByte(0x40);
  }
  OLED_SW_I2C_SendByte(dat);
  OLED_SW_I2C_Stop();
}

/*******************************************************************************
* Function Name  : OLED_Set_Pos
* Description    : �趨OLED��ʾ�ַ���λ��
* Input          : x, y - �������ҳ����
* Note(SSD1306)  :
* B0~B7 1 0 1 1  0 X2 X1 X0 ����ҳ��ַ X[2:0]:0~7 ��Ӧҳ 0~7
* 00~0F 0 0 0 0 X3 X2 X1 X0 �����е�ַ(����λ) ����8λ��ʼ�е�ַ�ĵ���λ
* 10~1F 0 0 0 1 X3 X2 X1 X0 �����е�ַ(����λ) ����8λ��ʼ�е�ַ�ĸ���λ
* Return         : None
*******************************************************************************/
void OLED_Set_Pos(uint8_t x, uint8_t y)
{ 
#if defined(OLED_0_42) || defined(OLED_0_66)
  x += 32;
#endif
	OLED_WR_Byte(0xb0 + y, OLED_CMD);            				//����ҳ��ַ
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);  	//�����е�ַ��4λ
	OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);      		//�����е�ַ��4λ
}   	  

/*******************************************************************************
* Function Name  : OLED_Display_On
* Description    : ����OLED��ʾ
* Input          : None
* Note(SSD1306)  :
*   8D   1 0 0 0 1  1 0 1
* A[7:0] * * 0 1 0 A2 0 0  ��ɱ�����   A2=0,�رյ�ɱ�;A2=1,������ɱ�
* AE/AF  1 0 1 0 1  1 1 X0 ������ʾ���� X0=0,�ر���ʾ;X0=1,������ʾ
* Return         : None
*******************************************************************************/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON      //������ɱ�
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON   //������ʾ
}

/*******************************************************************************
* Function Name  : OLED_Display_Off
* Description    : �ر�OLED��ʾ
* Input          : None
* Return         : None
*******************************************************************************/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF     //�رյ�ɱ�
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF  //�ر���ʾ
}

/*******************************************************************************
* Function Name  : OLED_Clear
* Description    : OLED��������
* Input          : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : OLED_ShowChar
* Description    : OLED��ָ��λ����ʾһ���ַ�,���������ַ�
* Input          : x, y - �������ҳ����; chr - ��ʾ���ַ�
* Return         : None
*******************************************************************************/
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{      	
	unsigned char c=0,i=0;	
	c = chr - ' ';                             //�õ�ƫ�ƺ��ֵ(��ӦASCII��)			
	if(x > Max_Column - 1) {
	  x = 0;
	  if (FONT_SIZE == 16) y += 2;
	  else y++;
	}
	if (FONT_SIZE == 16)                       //һ���ַ�ռ8��16��(2ҳ) һ��128*64������һ������ʾ16���ַ� ����ʾ4��
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
		OLED_Set_Pos(x, y);
		for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[c][i], OLED_DATA);
	}
}

/*******************************************************************************
* Function Name  : OLED_Pow
* Description    : ����m^n
* Input          : m, n
* Return         : m^n
*******************************************************************************/
inline uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
	uint32_t result=1;
	while(n--) result*=m;    
	return result;
}				  

/*******************************************************************************
* Function Name  : OLED_ShowNum
* Description    : ��ʾ1������
* Input          : x, y - �������ҳ����; num - ��ʾ������; len - ���ֵĳ���
* Return         : None
*******************************************************************************/
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{         	
	uint8_t t, temp;
	uint8_t enshow = 0;
	for(t = 0; t < len; t++)
	{
		temp=(num / OLED_Pow(10, len-t-1)) % 10;	//�����λ���ν�ÿһλ(ʮ����)ȡ��
		if(enshow == 0 && t < (len - 1))
		{
			if(temp == 0)
			{
			  if ( FONT_SIZE == 16 ) OLED_ShowChar(x + 8 * t, y, ' ');
			  else OLED_ShowChar(x + 6 * t, y, ' '); //����ͷΪ0��λ�Գ�' '
				continue;
			}else enshow = 1; 
		}
		if ( FONT_SIZE == 16 ) OLED_ShowChar(x + 8 * t, y, temp + '0');	//��ӡ����
		else OLED_ShowChar(x + 6 * t, y, temp + '0'); //��ӡ����
	}
}

/*******************************************************************************
* Function Name  : OLED_ShowString
* Description    : OLED��ʾ�ַ���
* Input          : x, y - �������ҳ����; *chr - Ҫ��ʾ���ַ���
* Return         : None
*******************************************************************************/
void OLED_ShowString(uint8_t x, uint8_t y, const uint8_t *chr)
{
	uint8_t j=0;
	while(chr[j]!='\0')                         //��λ��Ϊ�����ӡ
	{
		OLED_ShowChar(x, y, chr[j]);        			//�ӵ�x�е�yҳ��ӡ�ַ�
    if ( FONT_SIZE == 16 ) {
      x += 8;                                 //�ֿ�8�� ��ӡ����8��
      if (x > 128 - 8) {
        x = 0; y += 2;
      }
    } else {
      x += 6;                                 //�ֿ�6�� ��ӡ����6��
      if (x > 128 - 6) {
        x = 0; y += 1;
      }
    }
    j++;
	}
}

/*******************************************************************************
* Function Name  : OLED_ShowString_f
* Description    : OLED������ʾ�ַ���
* Input          : x, y - �������ҳ����; *chr - Ҫ��ʾ���ַ���
* Return         : None
*******************************************************************************/
void OLED_ShowString_f(uint8_t x, uint8_t y, const uint8_t *chr)
{
  uint8_t j=0;
  unsigned char c=0,i=0;

  while(chr[j] != '\0')                       //��λ��Ϊ�����ӡ
  {
    c = chr[j] - ' ';                         //�õ�ƫ�ƺ��ֵ(��ӦASCII��)
    if(x > Max_Column - 1) {
      x = 0;
      if (FONT_SIZE == 16) y += 2;
      else y++;
    }
    if (FONT_SIZE == 16) {                    //һ���ַ�ռ8��16��(2ҳ) һ��128*64������һ������ʾ16���ַ� ����ʾ4��
      OLED_Set_Pos(x, y);
      for(i = 0; i < 8; i++) OLED_WR_Byte(~F8X16[c * 16 + i], OLED_DATA);
      OLED_Set_Pos(x, y + 1);
      for(i = 0; i < 8; i++) OLED_WR_Byte(~F8X16[c * 16 + i + 8], OLED_DATA);
    } else {
      OLED_Set_Pos(x, y);
      for(i = 0; i < 6; i++) OLED_WR_Byte(~F6x8[c][i], OLED_DATA);
    }
    if ( FONT_SIZE == 16 ) {
      x += 8;           //�ֿ�8�� ��ӡ����8��
      if (x > 128 - 8) {
        x = 0; y += 2;
      }
    } else {
      x += 6;           //�ֿ�6�� ��ӡ����6��
      if (x > 128 - 6) {
        x = 0; y += 1;
      }
    }
    j++;
  }
}

/*******************************************************************************
* Function Name  : OLED_ShowCHinese
* Description    : OLED��ʾ����
* Input          : x, y - �������ҳ����; no - oledfont.h�д�ź�������Hzk�����
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : OLED_DrawBMP
* Description    : ��ʾλͼ
* Input          : x0Ϊ��ʼ������; y0Ϊ��ʵҳ����; x1Ϊ��ֹ������; y1λ��ֹҳ����; BMP[]Ϊ���ͼƬ������
* Return         : None
*******************************************************************************/
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t* BMP)
{ 	
  //BMP��С((x1-x0+1)*(y1-y0+1))
  unsigned int j=0;
  unsigned char x,y;

  for (y = y0; y < y1; y++) {
    OLED_Set_Pos(x0,y);
    for (x = x0; x < x1; x++) {
      OLED_WR_Byte(BMP[j++],OLED_DATA);	      //�����ʾ�����к���
    }
  }
} 

/*******************************************************************************
* Function Name  : OLED_Fill
* Description    : OLED���
* Input          : x0Ϊ��ʼ������; y0Ϊ��ʵҳ����; x1Ϊ��ֹ������; y1λ��ֹҳ����
* Return         : None
*******************************************************************************/
void OLED_Fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  //BMP��С((x1-x0+1)*(y1-y0+1))
  unsigned int j=0;
  unsigned char x,y;

  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {
      OLED_WR_Byte(0xFF, OLED_DATA);       //�����ʾ�����к���
    }
  }
}

/*******************************************************************************
* Function Name  : OLED_Clr
* Description    : OLED���(OLED_Fill������)
* Input          : x0Ϊ��ʼ������; y0Ϊ��ʵҳ����; x1Ϊ��ֹ������; y1λ��ֹҳ����
* Return         : None
*******************************************************************************/
void OLED_Clr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  //BMP��С((x1-x0+1)*(y1-y0+1))
  unsigned int j=0;
  unsigned char x,y;

  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {
      OLED_WR_Byte(0x00, OLED_DATA);       //�����ʾ�����к���
    }
  }
}

/*******************************************************************************
* Function Name  : OLED_Midx
* Description    : OLED�����ַ���/���־���λ��x����
* Input          : length - ���ֳ���; xstart, xend - ��ʾ������㡢�յ�
* Return         : None
*******************************************************************************/
inline uint8_t OLED_Midx(uint8_t length, uint8_t xstart, uint8_t xend)
{
  uint8_t w = FONT_SIZE == 16 ? 8 : 6;
	if (length * w > xend - xstart) return 0;
	return xstart + (xend - xstart - length * w) / 2;
}

/*******************************************************************************
* Function Name  : OLED_Set_Scroll_ENA
* Description    : ����OLED�Ƿ�ʹ�ܹ���
* Input          : 0 - ʧ��; 1 - ʹ��
* Return         : None
*******************************************************************************/
void OLED_Set_Scroll_ENA(uint8_t is_enable)
{
  if (is_enable) {
    OLED_WR_Byte(0x2F, OLED_CMD);
  } else {
    OLED_WR_Byte(0x2E, OLED_CMD);
  }
}

/*******************************************************************************
* Function Name  : OLED_Scroll
* Description    : ����OLED����
* Input          : h_y0 - ˮƽ������ʼҳ��ַ; h_y1 - ˮƽ������ֹҳ��ַ;
*                  v_ystart - ��ֱ������ʼ����(0~64); v_len - ��ֱ��������(0~64);
*                  frame - ֡��λ; scroll_offset - ����ƫ��; dir - ����(0��ʾ����, 1��ʾ����)
* Return         : None
*******************************************************************************/
void OLED_Scroll(uint8_t h_y0, uint8_t h_y1, uint8_t v_ystart, uint8_t v_len,
                 uint8_t frame, uint8_t scroll_offset, uint8_t dir)
{
  OLED_WR_Byte(0x2E, OLED_CMD);   // ֹͣ����

  OLED_WR_Byte(0xA3, OLED_CMD);
  OLED_WR_Byte(v_ystart, OLED_CMD);
  OLED_WR_Byte(v_len, OLED_CMD);

  OLED_WR_Byte(dir ? 0x29 : 0x2A, OLED_CMD);
  OLED_WR_Byte(0x00, OLED_CMD);
  OLED_WR_Byte(h_y0 & 0x7, OLED_CMD);
  OLED_WR_Byte(frame & 0x7, OLED_CMD);
  OLED_WR_Byte(h_y1 & 0x7, OLED_CMD);
  OLED_WR_Byte(scroll_offset, OLED_CMD);

  OLED_WR_Byte(0x2F, OLED_CMD);   // ���ù���
}

/*******************************************************************************
* Function Name  : HAL_OLED_Init
* Description    : ��ʼ��SSD1306
* Input          : None
* Return         : None
*******************************************************************************/
void HAL_OLED_Init(void)
{ 	
  //IO
#if !(defined HAL_HW_I2C) || (HAL_HW_I2C != TRUE)
  OLED_CLK_GPIO_(SetBits)( OLED_CLK_Pin );
  OLED_SDA_GPIO_(SetBits)( OLED_SDA_Pin );
  OLED_CLK_GPIO_(ModeCfg)( OLED_CLK_Pin, GPIO_ModeOut_PP_5mA );
  OLED_SDA_GPIO_(ModeCfg)( OLED_SDA_Pin, GPIO_ModeOut_PP_5mA );
#endif
  DelayMs(20);  //�ϵ��ӳ�

#ifdef OLED_0_42
  //display off
  OLED_WR_Byte(0xAE,OLED_CMD);
  OLED_WR_Byte(0xD5,OLED_CMD);
  OLED_WR_Byte(0xF0,OLED_CMD);
  OLED_WR_Byte(0xA8,OLED_CMD);
  OLED_WR_Byte(0x27,OLED_CMD);
  OLED_WR_Byte(0xD3,OLED_CMD);
  OLED_WR_Byte(0x00,OLED_CMD);
  OLED_WR_Byte(0x40,OLED_CMD);
  OLED_WR_Byte(0x8D,OLED_CMD);
  OLED_WR_Byte(0x14,OLED_CMD);
  OLED_WR_Byte(0x20,OLED_CMD);
  OLED_WR_Byte(0x02,OLED_CMD);
  OLED_WR_Byte(0xA1,OLED_CMD);
  OLED_WR_Byte(0xC8,OLED_CMD);
  OLED_WR_Byte(0xDA,OLED_CMD);
  OLED_WR_Byte(0x12,OLED_CMD);
  OLED_WR_Byte(0xAD,OLED_CMD);
  OLED_WR_Byte(0x30,OLED_CMD);

  //�Աȶ�
  OLED_WR_Byte(0x81,OLED_CMD);
  OLED_WR_Byte(0x2F,OLED_CMD);

  //Pre-Charge Period
  OLED_WR_Byte(0xD9,OLED_CMD);
  OLED_WR_Byte(0x22,OLED_CMD);

  //Vcomh
  OLED_WR_Byte(0xDB,OLED_CMD);
  OLED_WR_Byte(0x20,OLED_CMD);

  //������ʾ(�������RAM)
  OLED_WR_Byte(0xA4,OLED_CMD);

  //������ʾ
  OLED_WR_Byte(0xA6,OLED_CMD);

  //�п�ʼ��140
  OLED_WR_Byte(0x0C,OLED_CMD);
  OLED_WR_Byte(0x11,OLED_CMD);
#endif

#ifdef OLED_0_66
  OLED_WR_Byte(0xAE,OLED_CMD);  // display off

  OLED_WR_Byte(0xD5,OLED_CMD);  // set display clock divide ratio/oscillator frequency
  OLED_WR_Byte(0xF0,OLED_CMD);  // divide ratio = 0x0 + 1, oscillator frequency = 0xF

  OLED_WR_Byte(0xA8,OLED_CMD);  // set multiplex ratio
  OLED_WR_Byte(0x27,OLED_CMD);  // 28 MUX

  OLED_WR_Byte(0xD3,OLED_CMD);  // set vertical shift by COM
  OLED_WR_Byte(0x00,OLED_CMD);  // vertical shift = 0x0

  OLED_WR_Byte(0x40,OLED_CMD);  // start line 0x40 - 0x40 = 0
  OLED_WR_Byte(0xB0,OLED_CMD);  // start page 0xB0 - 0xB0 = 0

  OLED_WR_Byte(0x8D,OLED_CMD);
  OLED_WR_Byte(0x14,OLED_CMD);

  OLED_WR_Byte(0x20,OLED_CMD);  // set memory addressing mode
  OLED_WR_Byte(0x02,OLED_CMD);  // page address mode

  OLED_WR_Byte(0xA1,OLED_CMD);  // column 127 is mapped to SEG0

  OLED_WR_Byte(0xC8,OLED_CMD);  // set COM output scan direction. scan from COM[N-1] to COM0

  OLED_WR_Byte(0xDA,OLED_CMD);  // set COM pins hardware configuration
  OLED_WR_Byte(0x12,OLED_CMD);  // alternative COM pin configuration. disable COM Left/Right remap

  OLED_WR_Byte(0xAD,OLED_CMD);
  OLED_WR_Byte(0x30,OLED_CMD);

  OLED_WR_Byte(0x81,OLED_CMD);  // set contrast control
  OLED_WR_Byte(0x2F,OLED_CMD);

  OLED_WR_Byte(0xD9,OLED_CMD);  // set pre-charge period
  OLED_WR_Byte(0x22,OLED_CMD);

  OLED_WR_Byte(0xDB,OLED_CMD);  // set Vcomh
  OLED_WR_Byte(0x20,OLED_CMD);

  OLED_WR_Byte(0xA4,OLED_CMD);  // output folows RAM content

  OLED_WR_Byte(0xA6,OLED_CMD);  // set normal display (not inverse)
#endif

#ifdef OLED_0_96
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
#endif

  //display ON
  OLED_WR_Byte(0xAF, OLED_CMD);
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}
/*********************************************END OF FILE**********************/
