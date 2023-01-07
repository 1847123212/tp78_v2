/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED_UI.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/12/29
 * Description        : OLED UI接口
 *******************************************************************************/

#ifndef __OLED_UI_H
  #define __OLED_UI_H

  #define OLED_UI_TASK_MAX          5
  #define OLED_UI_DELAY_TASK_MAX    2
  #define OLED_UI_STR_LEN_MAX       19

  #define OLED_UI_HIS_LEN           10                    // 保存OLED打印历史条数
  #define OLED_UI_HIS_DLEN          OLED_UI_STR_LEN_MAX   // 每条OLED打印历史长度

  #define OLED_TASK_PERIOD          30    // units: 1ms

  typedef enum {
    OLED_UI_FLAG_DEFAULT = 0,
    OLED_UI_FLAG_DRAW_OK,
    OLED_UI_FLAG_CANCEL_OK,
    OLED_UI_FLAG_SHOW_STRING,
    OLED_UI_FLAG_SHOW_INFO,
    OLED_UI_FLAG_CANCEL_INFO,
    OLED_UI_FLAG_DRAW_BMP,
  }oled_ui_data_flag;

  typedef struct {
    uint8_t x0;
    uint8_t y0;
    uint8_t x1;
    uint8_t y1;
  }oled_ui_pos;

  typedef struct {
    uint8_t x;
    uint8_t y;
    uint16_t len;
  }oled_ui_pos_len;

  typedef struct {
    oled_ui_data_flag flag;
    oled_ui_pos_len pos_len;
    uint8_t* addr;
    uint8_t pstr[OLED_UI_STR_LEN_MAX];
  }oled_ui_draw_structure;

  typedef struct {
    oled_ui_draw_structure oled_ui_draw[OLED_UI_TASK_MAX];
    uint8_t head;
    uint8_t size;
  }oled_ui_task_structure;

  typedef struct {
    oled_ui_draw_structure oled_ui_draw[OLED_UI_DELAY_TASK_MAX];
    uint32_t count[OLED_UI_DELAY_TASK_MAX];
  }oled_ui_delay_task_structure;

  void OLED_UI_ShowOK(uint8_t x, uint8_t y, uint8_t s);
  void OLED_UI_ShowCapslock(uint8_t x, uint8_t y, uint8_t s);
  int OLED_UI_printf(char *pFormat, ...);
  uint8_t OLED_UI_add_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr);
  int OLED_UI_add_SHOWSTRING_task(uint8_t x, uint8_t y, char *pstr, ...);
  int OLED_UI_add_SHOWINFO_task(char *pstr, ...);
  uint8_t OLED_UI_add_delay_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr, uint32_t count);
  uint8_t OLED_UI_add_CANCELINFO_delay_task(uint32_t count);
  void OLED_UI_draw_thread_callback(void);

#endif
