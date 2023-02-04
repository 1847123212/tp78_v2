/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED_UI.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/12/29
 * Description        : OLED UI接口
 *******************************************************************************/

#ifndef __OLED_UI_H
  #define __OLED_UI_H

  #define OLED_UI_TASK_MAX          5     // UI最大普通任务数
  #define OLED_UI_DELAY_TASK_MAX    3     // UI最大延迟任务数
  #define OLED_UI_STR_LEN_MAX       17    // UI打印字符串最长字符数
  #define OLED_UI_MAX_SLOT          4     // UI槽的最多显示个数
  #define OLED_UI_MENU_MAX_LEN      3     // UI菜单每级最多选项个数(暂时不支持超过OLED屏幕显示范围)

  #define OLED_UI_HIS_LEN           3                     // 保存OLED打印历史条数
  #define OLED_UI_HIS_DLEN          OLED_UI_STR_LEN_MAX   // 每条OLED打印历史长度

  #define OLED_UI_ICON_WIDTH        10
  #define OLED_UI_ICON_HEIGHT       2     // units: page

  #define OLED_FRESH_RATE           30    // uints: 1Hz

  typedef enum {
    OLED_UI_FLAG_DEFAULT = 0,
    OLED_UI_FLAG_DRAW_OK,
    OLED_UI_FLAG_CANCEL_OK,
    OLED_UI_FLAG_SHOW_STRING,
    OLED_UI_FLAG_SHOW_INFO,
    OLED_UI_FLAG_CANCEL_INFO,
    OLED_UI_FLAG_DRAW_BMP,
    OLED_UI_FLAG_DRAW_SLOT,
    OLED_UI_FLAG_IDLE_DRAW,
    OLED_UI_FLAG_CTL_STOP_SCOLL,
    OLED_UI_FLAG_SMOOTH_SELECT,
    /* only for normal task */
    OLED_UI_FLAG_BAT_CHARGE,
    OLED_UI_FLAG_BAT_CLR_CHARGE,
    OLED_UI_FLAG_BAT_LEVEL_1,   // 0.91寸-0%; 0.66寸-0%
    OLED_UI_FLAG_BAT_LEVEL_2,   // 0.91寸-20%; 0.66寸-33%
    OLED_UI_FLAG_BAT_LEVEL_3,   // 0.91寸-40%; 0.66寸-67%
    OLED_UI_FLAG_BAT_LEVEL_4,   // 0.91寸-60%; 0.66寸-100%
    OLED_UI_FLAG_BAT_LEVEL_5,   // 0.91寸-80%; 0.66寸-reserved
    OLED_UI_FLAG_BAT_LEVEL_6,   // 0.91寸-100%; 0.66寸-reserved
  }oled_ui_data_flag;

  typedef enum {
    OLED_UI_ICON_USB_IDX = 0,
    OLED_UI_ICON_RF_IDX,
    OLED_UI_ICON_CP_IDX,
    OLED_UI_ICON_CAPSLOCK_IDX,
    OLED_UI_ICON_BLE_UNCONNECT_IDX,
    OLED_UI_ICON_BLE1_IDX,
    OLED_UI_ICON_BLE2_IDX,
    OLED_UI_ICON_BLE3_IDX,
    OLED_UI_ICON_BLE4_IDX,
  }oled_ui_icon_index;

  typedef enum {
    OLED_UI_MENU_REFRESH = 0,
    OLED_UI_SWIPE_UP,
    OLED_UI_SWIPE_DOWN,
    OLED_UI_SWIPE_LEFT,
    OLED_UI_SWIPE_RIGHT,
  }oled_ui_swipe;

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

  typedef struct {
    uint8_t* icon_p[OLED_UI_MAX_SLOT];
    uint8_t slot_size;
  }oled_ui_slot_structure;

  typedef struct oled_ui_menu_structure{
    const struct oled_ui_menu_structure* p[OLED_UI_MENU_MAX_LEN+1]; // 末位表示上级菜单指针
    uint8_t text[OLED_UI_MENU_MAX_LEN][OLED_UI_STR_LEN_MAX];
    uint8_t cur_x;  // 表示当前菜单中使用RAM的x坐标
  }oled_ui_menu_structure;

  extern uint8_t oled_fresh_rate;

  void OLED_UI_ShowOK(uint8_t x, uint8_t y, uint8_t s);
  void OLED_UI_ShowCapslock(uint8_t x, uint8_t y, uint8_t s);
  int OLED_UI_printf(char *pFormat, ...);
  uint8_t OLED_UI_add_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr);
  uint8_t OLED_UI_add_SHOWSTRING_task(uint8_t x, uint8_t y, char *pstr, ...);
  uint8_t OLED_UI_add_SHOWINFO_task(char *pstr, ...);
  uint8_t OLED_UI_add_default_task(oled_ui_data_flag flag);
  uint8_t OLED_UI_add_delay_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr, uint32_t count);
  uint8_t OLED_UI_add_default_delay_task(oled_ui_data_flag flag, uint32_t count);
  uint8_t OLED_UI_add_CANCELINFO_delay_task(uint32_t count);
  uint8_t OLED_UI_slot_add(uint8_t* p);
  uint8_t OLED_UI_slot_delete(uint8_t *p);
  uint8_t OLED_UI_slot_active(uint8_t *old_p, uint8_t *new_p);
  void OLED_UI_show_version(uint8_t ena);
  void OLED_UI_draw_empty_battery(void);
  void OLED_UI_draw_menu(oled_ui_swipe fresh_type);
  void OLED_UI_smooth_select_cfg(const uint8_t* str0, const uint8_t* str1, uint8_t y0, uint8_t y1);
  void OLED_UI_idle(uint8_t is_entrying);
  void OLED_UI_draw_thread_callback(void);

#endif
