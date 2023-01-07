/********************************** (C) COPYRIGHT *******************************
 * File Name          : OLED_UI.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/12/29
 * Description        : OLED UI接口
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include <stdarg.h>
#include "HAL.h"

oled_ui_task_structure oled_ui_task = { 0 };
oled_ui_delay_task_structure oled_ui_delay_task = { 0 };

uint8_t oled_printf_history[OLED_UI_HIS_LEN][OLED_UI_HIS_DLEN+1] = {};  // 存放OLED_printf的历史记录
static uint8_t oled_printf_history_idx = 0;  // 存放OLED打印历史的下标

/*******************************************************************************
* Function Name  : OLED_ShowOK
* Description    : OLED显示OK(打勾)
* Input          : x - x坐标; y - 页坐标; s - state, 显示打勾1或取消打勾0
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
* Description    : OLED显示Capslock(大小写)
* Input          : x - x坐标; y - 页坐标; s - state, 显示大小写被按下1或不显示0
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
* Description    : TP78消息显示
* Input          : *chr - 要显示的字符串
* Return         : None
*******************************************************************************/
static void OLED_TP78Info(uint8_t *chr)
{
//#ifdef OLED_0_91
  // 清空原有信息
  uint8_t i;
  OLED_Set_Pos(0, 3);
  for (i = 0; i < 64; i++) {
    OLED_WR_Byte(0x00, OLED_DATA);
  }
  // 定义左下角输出信息
  OLED_ShowString(OLED_Midx(strlen(chr), 0, 64), 3, chr);
//#endif
}

/*******************************************************************************
* Function Name  : OLED_printf
* Description    : OLED printf重定向函数
* Input          : *pFormat - 要显示格式化的字符串
* Return         : 输出长度
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

  /* 记录至历史 */
  if (strlen(pStr) > OLED_UI_HIS_DLEN) { // 截取长度
    pStr[OLED_UI_HIS_DLEN-1] = '\0';
  }
  if (oled_printf_history_idx >= OLED_UI_HIS_LEN) {  // 缓存满 - 队列结构
    memcpy((uint8_t*)oled_printf_history[0], (uint8_t*)oled_printf_history[1], (OLED_UI_HIS_LEN-1)*(OLED_UI_HIS_DLEN));
    strcpy(oled_printf_history[OLED_UI_HIS_LEN-1], pStr);
  } else {
    oled_printf_history_idx++;
  }

  return res;
}

/*******************************************************************************
* Function Name  : OLED_UI_add_task
* Description    : OLED_UI添加任务
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
* Description    : OLED_UI添加打印字符串任务
* Input          : pstr - pointer of string, and parameterization.
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
* Description    : OLED_UI添加打印信息任务
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
* Function Name  : OLED_UI_add_delay_task
* Description    : OLED_UI添加延迟任务
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
* Description    : OLED_UI添加清除字符串延迟任务
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
* Function Name  : OLED_UI_draw_thread_callback
* Description    : OLED_UI绘图回调函数
* Input          : None
* Return         : None
*******************************************************************************/
void OLED_UI_draw_thread_callback(void)
{
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
      default:
        continue;
    }
    // remove delay task
    oled_ui_delay_task.oled_ui_draw[i].flag = OLED_UI_FLAG_DEFAULT;
  }
//  OLED_Set_Pos(oled_ui_data.x0, oled_ui_data.y0++);
}
