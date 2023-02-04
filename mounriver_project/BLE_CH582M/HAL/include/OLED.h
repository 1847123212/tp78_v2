/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/12/30
 * Description        : OLED����ͷ�ļ�
 *******************************************************************************/
	
#ifndef __OLED_H
  #define __OLED_H

  #include "OLEDFONT.h"
  #include "CH58x_common.h"

//  #define OLED_0_42   // ����0.42��OLED
  #define OLED_0_66   // ����0.66��OLED
//  #define OLED_0_91   // ����0.91��OLED
//  #define OLED_0_96   // ����0.96��OLED

  #ifdef OLED_0_66
    #define OLED_WIDTH        64
    #define OLED_HEIGHT       6 // units:page
  #endif

  #ifdef OLED_0_42
    #define OLED_WIDTH        72
    #define OLED_HEIGHT       5 // uints:page
  #endif

  #define FONT_SIZE           8
  #define XLevelL             0x00
  #define XLevelH             0x10
  #define Max_Column          128
  #define Brightness          0xFF
  #define OLED_ADDR           0x78

  #define OLED_CLK_Pin        GPIO_Pin_13
  #define OLED_SDA_Pin        GPIO_Pin_12
  #define OLED_CLK_GPIO_(x)   GPIOB_ ## x
  #define OLED_SDA_GPIO_(x)   GPIOB_ ## x

  #define OLED_IIC_SCLK_Clr() OLED_CLK_GPIO_(ResetBits)(OLED_CLK_Pin)
  #define OLED_IIC_SCLK_Set() OLED_CLK_GPIO_(SetBits)(OLED_CLK_Pin)

  #define OLED_IIC_SDIN_Clr() OLED_SDA_GPIO_(ResetBits)(OLED_SDA_Pin)
  #define OLED_IIC_SDIN_Set() OLED_SDA_GPIO_(SetBits)(OLED_SDA_Pin)

  #define OLED_CMD  0	// д����
  #define OLED_DATA 1	// д����
  #define I2C_DELAY { OLED_SW_I2C_Delay(); }

  #if (defined HAL_HW_I2C) && (HAL_HW_I2C == TRUE)
    #define OLED_WR_Byte(dat, cmd)  HW_I2C_WR_Reg(cmd ? 0x40 : 0, dat, OLED_ADDR)
  #else
    #define OLED_WR_Byte(dat, cmd)  OLED_SW_I2C_WR_Byte(dat, cmd)
  #endif

  typedef enum {
    OLED_5_FRAMES = 0,
    OLED_64_FRAMES,
    OLED_128_FRAMES,
    OLED_256_FRAMES,
    OLED_3_FRAMES,
    OLED_4_FRAMES,
    OLED_25_FRAMES,
    OLED_2_FRAMES,
  }OLED_FRAME_FREQUENCY;

  typedef enum {
    OLED_SCOLL_LEFT = 0,
    OLED_SCOLL_RIGHT,
  }OLED_SCOLL_DIRECTION;

  /* OLED���ƺ��� */
  void OLED_SW_I2C_WR_Byte(uint8_t dat,uint8_t cmd);
  void OLED_Set_Pos(uint8_t x, uint8_t y);
  void OLED_Display_On(void);
  void OLED_Display_Off(void);
  void OLED_Clear(void);
  void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
  void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
  void OLED_ShowString(uint8_t x, uint8_t y, const uint8_t *chr);
  void OLED_ShowString_f(uint8_t x, uint8_t y, const uint8_t *chr);
  void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no);
  void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t* BMP);
  void OLED_Clr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
  void OLED_Fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
  uint8_t OLED_Midx(uint8_t length, uint8_t xstart, uint8_t xend);
  void OLED_Set_Scroll_ENA(uint8_t is_enable);
  void OLED_Scroll(uint8_t h_y0, uint8_t h_y1, uint8_t v_ystart, uint8_t v_len, uint8_t frame, uint8_t scroll_offset, uint8_t dir);
  void HAL_OLED_Init(void);

#endif
	 
/*********************************************END OF FILE**********************/
