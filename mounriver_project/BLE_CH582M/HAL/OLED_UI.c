/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED_UI.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/12/29
 * Description        : OLED UI�ӿ�
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include <stdarg.h>
#include "HAL.h"

static oled_ui_task_structure oled_ui_task = { 0 };
static oled_ui_delay_task_structure oled_ui_delay_task = { 0 };
static oled_ui_slot_structure oled_ui_slot = { 0 };

uint8_t oled_printf_history[OLED_UI_HIS_LEN][OLED_UI_HIS_DLEN+1] = {};  // ���OLED_printf����ʷ��¼
static uint8_t oled_printf_history_idx = 0;  // ���OLED��ӡ��ʷ���±�

/*******************************************************************************
* Function Name  : OLED_ShowOK
* Description    : OLED��ʾOK(��)
* Input          : x - x����; y - ҳ����; s - state, ��ʾ��1��ȡ����0
* Return         : None
*******************************************************************************/
void OLED_UI_ShowOK(uint8_t x, uint8_t y, uint8_t s)
{
  uint8_t i;
  OLED_Set_Pos(x, y);
  if (SIZE == 8) {
    if (s != 0) for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[92][i], OLED_DATA);
    else for(i = 0; i < 6; i++) OLED_WR_Byte(0x00, OLED_DATA);
  }
}

/*******************************************************************************
* Function Name  : OLED_ShowCapslock
* Description    : OLED��ʾCapslock(��Сд)
* Input          : x - x����; y - ҳ����; s - state, ��ʾ��Сд������1����ʾ0
* Return         : None
*******************************************************************************/
void OLED_UI_ShowCapslock(uint8_t x, uint8_t y, uint8_t s)
{
  uint8_t i;
  OLED_Set_Pos(x, y);
  if (SIZE == 8) {
    if (s != 0) for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[93][i], OLED_DATA);
    else for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[94][i], OLED_DATA);
  }
}

/*******************************************************************************
* Function Name  : OLED_TP78Info
* Description    : TP78��Ϣ��ʾ
* Input          : *chr - Ҫ��ʾ���ַ���
* Return         : None
*******************************************************************************/
static void OLED_TP78Info(uint8_t *chr)
{
#if defined(OLED_0_91) || defined(OLED_0_66)
  // ���ԭ����Ϣ
  uint8_t i;
  OLED_Set_Pos(0, 3);
  for (i = 0; i < 64; i++) {
    OLED_WR_Byte(0x00, OLED_DATA);
  }
  // �������½������Ϣ
  OLED_ShowString(OLED_Midx(strlen(chr), 0, 64), 3, chr);
#endif
}

/*******************************************************************************
* Function Name  : OLED_printf
* Description    : OLED printf�ض�����
* Input          : *pFormat - Ҫ��ʾ��ʽ�����ַ���
* Return         : �������
*******************************************************************************/
int OLED_UI_printf(char *pFormat, ...)
{
  char pStr[OLED_UI_STR_LEN_MAX] = {'\0'};
  va_list ap;
  int res;

  va_start(ap, pFormat);
  res = vsprintf((char*)pStr, pFormat, ap);
  va_end(ap);

  OLED_TP78Info(pStr);

  /* ��¼����ʷ */
  if (strlen(pStr) > OLED_UI_HIS_DLEN) { // ��ȡ����
    pStr[OLED_UI_HIS_DLEN-1] = '\0';
  }
  if (oled_printf_history_idx >= OLED_UI_HIS_LEN) {  // ������ - ���нṹ
    memcpy((uint8_t*)oled_printf_history[0], (uint8_t*)oled_printf_history[1], (OLED_UI_HIS_LEN-1)*(OLED_UI_HIS_DLEN));
    strcpy(oled_printf_history[OLED_UI_HIS_LEN-1], pStr);
  } else {
    oled_printf_history_idx++;
  }

  return res;
}

/*******************************************************************************
* Function Name  : OLED_UI_add_task
* Description    : OLED_UI�������
* Input          : flag - new task flag;
*                  pos_len - new task pos and len;
*                  addr - pointer of BMP;
*                  pstr - pointer of string.
* Return         : 0 - success, 1 - failed
*******************************************************************************/
uint8_t OLED_UI_add_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr)
{
  uint8_t index;

  if (oled_ui_task.size >= OLED_UI_TASK_MAX) {
    return 1;
  }
  index = oled_ui_task.head + oled_ui_task.size;
  if (index >= OLED_UI_TASK_MAX) {
    index = index - OLED_UI_TASK_MAX;
  }
  oled_ui_task.oled_ui_draw[index].flag = flag;
  oled_ui_task.oled_ui_draw[index].pos_len = pos_len;
  oled_ui_task.oled_ui_draw[index].addr = addr;
  strcpy(oled_ui_task.oled_ui_draw[index].pstr, pstr);  // assert strlen(pstr)+1 < OLED_UI_STR_LEN_MAX
  oled_ui_task.size++;
  return 0;
}

/*******************************************************************************
* Function Name  : OLED_UI_add_SHOWSTRING_task
* Description    : OLED_UI��Ӵ�ӡ�ַ�������
* Input          : x - position X;
*                  y - position Y;
*                  pstr - pointer of string, and parameterization.
* Return         : 0 - success, 1 - failed
*******************************************************************************/
int OLED_UI_add_SHOWSTRING_task(uint8_t x, uint8_t y, char *pstr, ...)
{
  char pStr[OLED_UI_STR_LEN_MAX] = {'\0'};
  va_list ap;
  uint8_t index;
  oled_ui_pos_len pos_len = {.x = x, .y = y, .len = 0};

  if (oled_ui_task.size >= OLED_UI_TASK_MAX) {
    return 1;
  }

  va_start(ap, pstr);
  vsprintf((char*)pStr, pstr, ap);
  va_end(ap);

  index = oled_ui_task.head + oled_ui_task.size;
  if (index >= OLED_UI_TASK_MAX) {
    index = index - OLED_UI_TASK_MAX;
  }
  oled_ui_task.oled_ui_draw[index].flag = OLED_UI_FLAG_SHOW_STRING;
  oled_ui_task.oled_ui_draw[index].pos_len = pos_len;
  strcpy(oled_ui_task.oled_ui_draw[index].pstr, pStr);  // assert strlen(pstr)+1 < OLED_UI_STR_LEN_MAX
  oled_ui_task.size++;

  return 0;
}

/*******************************************************************************
* Function Name  : OLED_UI_add_SHOWINFO_task
* Description    : OLED_UI��Ӵ�ӡ��Ϣ����
* Input          : pstr - pointer of string, and parameterization.
* Return         : 0 - success, 1 - failed
*******************************************************************************/
int OLED_UI_add_SHOWINFO_task(char *pstr, ...)
{
  char pStr[OLED_UI_STR_LEN_MAX] = {'\0'};
  va_list ap;
  uint8_t index;

  if (oled_ui_task.size >= OLED_UI_TASK_MAX) {
    return 1;
  }

  va_start(ap, pstr);
  vsprintf((char*)pStr, pstr, ap);
  va_end(ap);

  index = oled_ui_task.head + oled_ui_task.size;
  if (index >= OLED_UI_TASK_MAX) {
    index = index - OLED_UI_TASK_MAX;
  }
  oled_ui_task.oled_ui_draw[index].flag = OLED_UI_FLAG_SHOW_INFO;
  strcpy(oled_ui_task.oled_ui_draw[index].pstr, pStr);  // assert strlen(pstr)+1 < OLED_UI_STR_LEN_MAX
  oled_ui_task.size++;

  return 0;
}

/*******************************************************************************
* Function Name  : OLED_UI_add_default_task
* Description    : OLED_UI���Ĭ������
* Input          : flag - task flag;
* Return         : 0 - success, 1 - failed
*******************************************************************************/
int OLED_UI_add_default_task(oled_ui_data_flag flag)
{
  oled_ui_pos_len pos_len = {.x = 0, .y = 0, .len = 0};
  return OLED_UI_add_task(flag, pos_len, NULL, NULL);
}

/*******************************************************************************
* Function Name  : OLED_UI_add_delay_task
* Description    : OLED_UI����ӳ�����
* Input          : flag - new task flag;
*                  pos_len - new task pos and len;
*                  addr - pointer of BMP;
*                  pstr - pointer of string;
*                  count - the delay counter.
* Return         : 0 - success, 1 - failed
*******************************************************************************/
uint8_t OLED_UI_add_delay_task(oled_ui_data_flag flag, oled_ui_pos_len pos_len, uint8_t* addr, uint8_t* pstr, uint32_t count)
{
  uint8_t i;

  for (i = 0; i < OLED_UI_DELAY_TASK_MAX; i++) {
    if (oled_ui_delay_task.oled_ui_draw[i].flag == OLED_UI_FLAG_DEFAULT) {
      break;
    }
  }
  if (i >= OLED_UI_DELAY_TASK_MAX) {
    return 1;
  }
  oled_ui_delay_task.oled_ui_draw[i].flag = flag;
  oled_ui_delay_task.oled_ui_draw[i].pos_len = pos_len;
  oled_ui_delay_task.oled_ui_draw[i].addr = addr;
  strcpy(oled_ui_delay_task.oled_ui_draw[i].pstr, pstr);  // assert strlen(pstr)+1 < OLED_UI_STR_LEN_MAX
  oled_ui_delay_task.count[i] = count;
  return 0;
}

/*******************************************************************************
* Function Name  : OLED_UI_add_CANCELINFO_delay_task
* Description    : OLED_UI�������ַ����ӳ�����
* Input          : pstr - pointer of string;
*                  count - the delay counter.
* Return         : 0 - success, 1 - failed
*******************************************************************************/
uint8_t OLED_UI_add_CANCELINFO_delay_task(uint32_t count)
{
  uint8_t i;

  for (i = 0; i < OLED_UI_DELAY_TASK_MAX; i++) {
    if (oled_ui_delay_task.oled_ui_draw[i].flag == OLED_UI_FLAG_DEFAULT) {
      break;
    }
  }
  if (i >= OLED_UI_DELAY_TASK_MAX) {
    return 1;
  }
  oled_ui_delay_task.oled_ui_draw[i].flag = OLED_UI_FLAG_CANCEL_INFO;
  oled_ui_delay_task.count[i] = count;
  return 0;
}

/*******************************************************************************
* Function Name  : OLED_UI_slot_add
* Description    : ��OLED_UI�����һ��icon
* Input          : p - Ҫ��ӵ�icon����ָ��
* Return         : 0 - success, 1 - failed
*******************************************************************************/
uint8_t OLED_UI_slot_add(uint8_t* p)
{
  if (p == NULL) {
    return 1;
  }
  if (oled_ui_slot.slot_size == OLED_UI_MAX_SLOT) {
    return 1;
  }
  oled_ui_slot.icon_p[oled_ui_slot.slot_size] = p;
  oled_ui_slot.slot_size++;
  return 0;
}

/*******************************************************************************
* Function Name  : OLED_UI_slot_delete
* Description    : ��OLED_UI��ɾ��һ��icon
* Input          : p - Ҫɾ����icon����ָ��
* Return         : 0 - success, 1 - failed
*******************************************************************************/
uint8_t OLED_UI_slot_delete(uint8_t *p)
{
  uint8_t i, j;
  if (p == NULL) {
    return 1;
  }
  for (i = 0; i < oled_ui_slot.slot_size; i++) {
    if (oled_ui_slot.icon_p[i] == p) {
      for (j = i; j < oled_ui_slot.slot_size - 1; j++) {
        oled_ui_slot.icon_p[j] = oled_ui_slot.icon_p[j + 1];
      }
      oled_ui_slot.icon_p[oled_ui_slot.slot_size - 1] = NULL;
      oled_ui_slot.slot_size--;
      return 0;
    }
  }
  return 1;
}

/*******************************************************************************
* Function Name  : OLED_UI_slot_active
* Description    : ����OLED_UI���е�һ��icon(��ɾ��һ��icon�����һ��icon)
* Input          : old_p - Ҫɾ����icon����ָ��, new_p - Ҫ�¼ӵ�icon����ָ��
* Return         : 0 - success, others - failed
*******************************************************************************/
uint8_t OLED_UI_slot_active(uint8_t *old_p, uint8_t *new_p)
{
  uint8_t res = 0;
  if (new_p == NULL || old_p == NULL) {
    return 1;
  }
  res += OLED_UI_slot_delete(old_p);
  res += OLED_UI_slot_add(new_p);
  return res;
}

/*******************************************************************************
* Function Name  : OLED_UI_slot_draw
* Description    : ����OLED UI��
* Input          : None
* Return         : None
*******************************************************************************/
void OLED_UI_slot_draw(void)
{
  uint8_t i;
  for (i = 0; i < OLED_UI_MAX_SLOT; i++) {
    if (oled_ui_slot.icon_p[i] == NULL) {
      OLED_Clr(OLED_WIDTH - (i + 1) * OLED_UI_ICON_WIDTH, 0,
               OLED_WIDTH - i * OLED_UI_ICON_WIDTH, OLED_UI_ICON_HEIGHT);
    } else {
      OLED_DrawBMP(OLED_WIDTH - (i + 1) * OLED_UI_ICON_WIDTH, 0,
                   OLED_WIDTH - i * OLED_UI_ICON_WIDTH, OLED_UI_ICON_HEIGHT,
                   oled_ui_slot.icon_p[i]);
    }
  }
}

/*******************************************************************************
* Function Name  : OLED_UI_show_version
* Description    : ��ӡ�̼��汾��Ϣ
* Input          : ena - 0��ʾʧ��, 1��ʾʹ��
* Return         : None
*******************************************************************************/
void OLED_UI_show_version(uint8_t ena)
{
#ifdef OLED_0_66
  if (ena) {
    OLED_ShowString(OLED_Midx(8, 0, 64), 2, "Firmware");
    OLED_ShowString(OLED_Midx(6, 0, 64), 3, FIRMWARE_VERSION);
    OLED_ShowString(OLED_Midx(8, 0, 64), 4, "Designers");
    OLED_ShowString(0, 5, "Qi.C, Chengwei.L");
    OLED_Scroll(5, 5, 16, 48, 2, 1, 0);
  } else {
    OLED_Clr(0, 2, 63, 8);
    OLED_Scroll(7, 7, 24, 16, 2, 1, 0);
  }
#endif
}

/*******************************************************************************
* Function Name  : OLED_UI_draw_empty_battery
* Description    : ���ƿյ��
* Input          : None
* Return         : None
*******************************************************************************/
void OLED_UI_draw_empty_battery(void)
{
#ifdef OLED_0_91
  extern const unsigned char BatteryBMP[][96];
  OLED_DrawBMP(91, 0, 91 + 32, 3, (uint8_t*)BatteryBMP[0]);
#endif
#ifdef OLED_0_66
  extern const unsigned char BatteryBMP[][46];
  OLED_DrawBMP(0, 0, 23, 2, (uint8_t*)BatteryBMP[0]);
#endif
}

/*******************************************************************************
* Function Name  : OLED_UI_draw_thread_callback
* Description    : OLED_UI��ͼ�ص�����
* Input          : None
* Return         : None
*******************************************************************************/
void OLED_UI_draw_thread_callback(void)
{
#ifdef OLED_0_91
  extern const unsigned char BatteryBMP[][96];
#endif
#ifdef OLED_0_66
  extern const unsigned char BatteryBMP[][46];
#endif
  uint8_t i;
  /* normal task */
  switch (oled_ui_task.oled_ui_draw[oled_ui_task.head].flag)
  {
    case OLED_UI_FLAG_DRAW_OK:
      break;
    case OLED_UI_FLAG_CANCEL_OK:
      break;
    case OLED_UI_FLAG_SHOW_STRING:
      OLED_ShowString(oled_ui_task.oled_ui_draw[oled_ui_task.head].pos_len.x,
                      oled_ui_task.oled_ui_draw[oled_ui_task.head].pos_len.y,
                      oled_ui_task.oled_ui_draw[oled_ui_task.head].pstr);
      break;
    case OLED_UI_FLAG_SHOW_INFO:
      OLED_PRINT("%s", oled_ui_task.oled_ui_draw[oled_ui_task.head].pstr);
      break;
    case OLED_UI_FLAG_CANCEL_INFO:
      OLED_PRINT("");
      break;
    case OLED_UI_FLAG_DRAW_SLOT:
      OLED_UI_slot_draw();
      break;
    case OLED_UI_FLAG_BAT_CHARGE:
#ifdef OLED_0_91
      OLED_DrawBMP(72, 0, 81, 4, (uint8_t*)ChargeBattery);
#endif
#ifdef OLED_0_66
      OLED_DrawBMP(0, 0, 23, 2, (uint8_t*)ChargeBattery);
#endif
      break;
    case OLED_UI_FLAG_BAT_CLR_CHARGE:
#ifdef OLED_0_91
      OLED_Clr(72, 0, 81, 4);
#endif
#ifdef OLED_0_66
      BAT_adcHistory = 0; // clear adc history
#endif
      break;
    case OLED_UI_FLAG_BAT_LEVEL_1:
    case OLED_UI_FLAG_BAT_LEVEL_2:
    case OLED_UI_FLAG_BAT_LEVEL_3:
    case OLED_UI_FLAG_BAT_LEVEL_4:
    case OLED_UI_FLAG_BAT_LEVEL_5:
    case OLED_UI_FLAG_BAT_LEVEL_6:
#ifdef OLED_0_91
      OLED_DrawBMP(91, 0, 91 + 32, 3, (uint8_t*)BatteryBMP[oled_ui_task.oled_ui_draw[oled_ui_task.head].flag - OLED_UI_FLAG_BAT_LEVEL_1]);
#endif
#ifdef OLED_0_66
      OLED_DrawBMP(0, 0, 23, 2, (uint8_t*)BatteryBMP[oled_ui_task.oled_ui_draw[oled_ui_task.head].flag - OLED_UI_FLAG_BAT_LEVEL_1]);
#endif
      break;
    default:
      goto normal_task_process_end;
  }
  // remove task
  oled_ui_task.oled_ui_draw[oled_ui_task.head].flag = OLED_UI_FLAG_DEFAULT;
  oled_ui_task.head = (oled_ui_task.head == OLED_UI_TASK_MAX - 1) ? 0 : oled_ui_task.head + 1;
  oled_ui_task.size--;
  normal_task_process_end:
  /* delay task */
  for (i = 0; i < OLED_UI_DELAY_TASK_MAX; i++) {
    if (oled_ui_delay_task.count[i] != 0) {
      oled_ui_delay_task.count[i]--;
      continue;
    }
    switch (oled_ui_delay_task.oled_ui_draw[i].flag)
    {
      case OLED_UI_FLAG_DRAW_OK:
        break;
      case OLED_UI_FLAG_CANCEL_OK:
        break;
      case OLED_UI_FLAG_SHOW_STRING:
        OLED_ShowString(oled_ui_delay_task.oled_ui_draw[i].pos_len.x,
                        oled_ui_delay_task.oled_ui_draw[i].pos_len.y,
                        oled_ui_delay_task.oled_ui_draw[i].pstr);
        break;
      case OLED_UI_FLAG_SHOW_INFO:
        OLED_PRINT("%s", oled_ui_delay_task.oled_ui_draw[i].pstr);
        break;
      case OLED_UI_FLAG_CANCEL_INFO:
        OLED_PRINT("");
        break;
      case OLED_UI_FLAG_DRAW_SLOT:
        OLED_UI_slot_draw();
        break;
      default:
        continue;
    }
    // remove delay task
    oled_ui_delay_task.oled_ui_draw[i].flag = OLED_UI_FLAG_DEFAULT;
  }
//  OLED_Set_Pos(oled_ui_data.x0, oled_ui_data.y0++);
}
