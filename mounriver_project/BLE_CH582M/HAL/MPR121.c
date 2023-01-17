/********************************** (C) COPYRIGHT *******************************
 * File Name          : MPR121.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/1/2
 * Description        : mpr121电容传感器驱动
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "HAL.h"

/* pinmux */
uint8_t MPR121_Cap_Mouse_Pinmux[4] = {6, 5, 4, 7}; //{0, 1, 2, 3};  // up-ELE6, down-ELE4, left-ELE3, right-ELE5
uint8_t MPR121_TouchBar_Pinmux[7] = {3, 2, 1, 0, 6, 5, 4};  //{4, 5, 10, 9, 8, 7, 6}; // L1, L2, L3, M, R1, R2, R3

/* algorithom variables */
static alg_ListNode cap_mouse_data[4][ALG_CAP_MOUSE_BUF_LEN]; // 4 Corresponds to UP/DOWN/LEFT/RIGHT
static alg_ListNode cap_mouse_sts[ALG_CAP_MOUSE_BUF_LEN];
static alg_ListNode touchbar_data[ALG_TOUCHBAR_BUF_LEN];
alg_ListNode* cap_mouse_dat_head[4];
alg_ListNode* cap_mouse_sts_head[1];
alg_ListNode* touchbar_head[1];
alg_Param mpr_algParameter;

BOOL cap_mouse_data_change = FALSE;
BOOL touchbar_data_change = FALSE;

Touchbarstate Touchbardat;

/*******************************************************************************
* Function Name  : MPR121_Config_Registers
* Description    : MPR121配置相关寄存器
* Input          : buf - 错误信息
* Return         : None
*******************************************************************************/
static void MPR121_Config_Registers(char* buf)
{
  uint8_t i, err;

  err = MPR121_WriteReg(MPR121_REG_SRST, 0x63);    // soft reset
  if (err) {
    strcpy(buf, "MPR121-ERR");
  }
  MPR121_WriteReg(MPR121_REG_ECR, 0x00);     // stop mode

  // filtering control, data is > baseline
  MPR121_WriteReg(MPR121_REG_MHDR, 0x01);
  MPR121_WriteReg(MPR121_REG_NHDR, 0x01);
  MPR121_WriteReg(MPR121_REG_NCLR, 0x00);
  MPR121_WriteReg(MPR121_REG_FDLR, 0x00);

  // filtering control, data is < baseline
  MPR121_WriteReg(MPR121_REG_MHDF, 0x01);
  MPR121_WriteReg(MPR121_REG_NHDF, 0x01);
  MPR121_WriteReg(MPR121_REG_NCLF, 0xFF);
  MPR121_WriteReg(MPR121_REG_FDLF, 0x00);

  // baseline filtering control register
  MPR121_WriteReg(MPR121_REG_NHDT, 0x00);    //noise half delta touched
  MPR121_WriteReg(MPR121_REG_NCLT, 0x00);    //noise counts touched
  MPR121_WriteReg(MPR121_REG_FDLT, 0x00);    //filter delay touched

  MPR121_WriteReg(MPR121_REG_MHDPROXR, 0x0F);
  MPR121_WriteReg(MPR121_REG_NHDPROXR, 0x0F);
  MPR121_WriteReg(MPR121_REG_NCLPROXR, 0x00);
  MPR121_WriteReg(MPR121_REG_FDLPROXR, 0x00);
  MPR121_WriteReg(MPR121_REG_MHDPROXF, 0x01);
  MPR121_WriteReg(MPR121_REG_NHDPROXF, 0x01);
  MPR121_WriteReg(MPR121_REG_NCLPROXF, 0xFF);
  MPR121_WriteReg(MPR121_REG_FDLPROXF, 0xFF);
  MPR121_WriteReg(MPR121_REG_NHDPROXT, 0x00);
  MPR121_WriteReg(MPR121_REG_NCLPROXT, 0x00);
  MPR121_WriteReg(MPR121_REG_FDLPROXT, 0x00);

  // set touch & release threshold for each ELE
  for (i = 0; i < 4; i++) {
    MPR121_WriteReg(MPR121_REG_E0TTH + MPR121_Cap_Mouse_Pinmux[i] * 2, mpr_algParameter.cap_mouse_tou_thr);
    MPR121_WriteReg(MPR121_REG_E0RTH + MPR121_Cap_Mouse_Pinmux[i] * 2, mpr_algParameter.cap_mouse_rel_thr);
  }
  for (i = 0; i < 7; i++) {
    MPR121_WriteReg(MPR121_REG_E0TTH + MPR121_TouchBar_Pinmux[i] * 2, mpr_algParameter.touchbar_tou_thr);
    MPR121_WriteReg(MPR121_REG_E0RTH + MPR121_TouchBar_Pinmux[i] * 2, mpr_algParameter.touchbar_rel_thr);
  }

  MPR121_WriteReg(MPR121_REG_EPROXTTH, 0x02);
  MPR121_WriteReg(MPR121_REG_EPROXRTH, 0x01);
  MPR121_WriteReg(MPR121_REG_DTR, 0x00);
  MPR121_WriteReg(MPR121_REG_FG_CDC, 0x10);
  MPR121_WriteReg(MPR121_REG_FG_CDT, 0x24);

  // enable auto config & auto reconfig
  MPR121_WriteReg(MPR121_REG_ACCR0, 0x0B);
  MPR121_WriteReg(MPR121_REG_ACUSL, 0xC8);
  MPR121_WriteReg(MPR121_REG_ACLSL, 0x82);
  MPR121_WriteReg(MPR121_REG_ACTLR, 0xB4);

  // ELE control
  MPR121_WriteReg(MPR121_REG_ECR, 0xBC); //enable all electrodes
}

/*******************************************************************************
* Function Name  : MPR121_ELE_to_Pin
* Description    : MPR121寄存器数据根据pinmux交换引脚顺序
* Input          : ele_data
* Return         : None
*******************************************************************************/
static uint16_t MPR121_ELE_to_Pin(uint16_t ele_data, uint8_t* pinmux, uint8_t size)
{
  uint8_t i;
  uint16_t pin_data = 0;
  for (i = 0; i < size; i++) {
    pin_data |= ((ele_data >> pinmux[i]) & 1) << i;
  }
  return pin_data;
}

/*******************************************************************************
* Function Name  : MPR121_ALG_Init
* Description    : MPR121算法初始化
* Input          : None
* Return         : None
*******************************************************************************/
static void MPR121_ALG_Init(void)
{
  uint8_t i, j;
  /* Cap Mouse Data(cycle list) Init */
  for (i = 0; i < 4; i++) {
    for (j = 0; j < ALG_CAP_MOUSE_BUF_LEN-1; j++) {
      cap_mouse_data[i][j].next = &cap_mouse_data[i][j+1];
      cap_mouse_data[i][j+1].prev = &cap_mouse_data[i][j];
    }
    cap_mouse_data[i][0].prev = &cap_mouse_data[i][ALG_CAP_MOUSE_BUF_LEN-1];
    cap_mouse_data[i][ALG_CAP_MOUSE_BUF_LEN-1].next = &cap_mouse_data[i][0];
    cap_mouse_dat_head[i] = &cap_mouse_data[i][0];
  }
  /* Cap Mouse Status(cycle list) Init */
  for (i = 0; i < ALG_CAP_MOUSE_BUF_LEN-1; i++) {
    cap_mouse_sts[i].next = &cap_mouse_sts[i+1];
    cap_mouse_sts[i+1].prev = &cap_mouse_sts[i];
  }
  cap_mouse_sts[0].prev = &cap_mouse_sts[ALG_CAP_MOUSE_BUF_LEN-1];
  cap_mouse_sts[ALG_CAP_MOUSE_BUF_LEN-1].next = &cap_mouse_sts[0];
  cap_mouse_sts_head[0] = &cap_mouse_sts[0];
  /* TouchBar Data(cycle list) Init */
  for (i = 0; i < ALG_TOUCHBAR_BUF_LEN-1; i++) {
    touchbar_data[i].next = &touchbar_data[i+1];
    touchbar_data[i+1].prev = &touchbar_data[i];
  }
  touchbar_data[0].prev = &touchbar_data[ALG_TOUCHBAR_BUF_LEN-1];
  touchbar_data[ALG_TOUCHBAR_BUF_LEN-1].next = &touchbar_data[0];
  touchbar_head[0] = &touchbar_data[0];
}

/*******************************************************************************
* Function Name  : MPR121_Init
* Description    : 初始化MPR121
* Input          : buf - 错误信息
* Return         : None
*******************************************************************************/
void MPR121_Init(char* buf)
{
  /* config PB19 as TP_INT */
  MPRINT_GPIO_(SetBits)( MPRINT_Pin );
  MPRINT_GPIO_(ModeCfg)( MPRINT_Pin, GPIO_ModeIN_PU );

  /* MPR121 IRQ */
  /*
  MPRINT_GPIO_(ITModeCfg)( MPRINT_Pin, GPIO_ITMode_FallEdge );
  PFIC_EnableIRQ( GPIO_B_IRQn );  //MPRINT_GPIO
  */

  FLASH_Read_MPR121_ALG_Parameter();
  MPR121_ALG_Init();
  MPR121_Config_Registers(buf);
}

/*******************************************************************************
* Function Name  : FLASH_Read_MPR121_ALG_Parameter
* Description    : 从Flash读取MPR121算法参数
* Input          : None
* Return         : None
*******************************************************************************/
void FLASH_Read_MPR121_ALG_Parameter(void)
{
  EEPROM_READ( FLASH_ADDR_MPR121_ALG_Param, &mpr_algParameter, sizeof(alg_Param) );
  if (mpr_algParameter.magic != ALG_PARAM_MAGIC) {
    // use default parameter
    mpr_algParameter.magic = ALG_PARAM_MAGIC;
    mpr_algParameter.touchbar_tou_thr = TOUCHBAR_TOU_THRESH;
    mpr_algParameter.touchbar_rel_thr = TOUCHBAR_REL_THRESH;
    mpr_algParameter.cap_mouse_tou_thr = CAP_MOUSE_TOU_THRESH;
    mpr_algParameter.cap_mouse_rel_thr = CAP_MOUSE_REL_THRESH;
  }
}

/*******************************************************************************
* Function Name  : FLASH_Write_MPR121_ALG_Parameter
* Description    : 将MPR121算法参数写入Flash
* Input          : None
* Return         : None
*******************************************************************************/
void FLASH_Write_MPR121_ALG_Parameter(void)
{
  EEPROM_WRITE( FLASH_ADDR_MPR121_ALG_Param, &mpr_algParameter, sizeof(alg_Param) );
}

/*******************************************************************************
* Function Name  : MPR121_ALG_Update_algListNode
* Description    : MPR121算法更新链表
* Input          : p - 链表指针; index - 数组下标; dat - 链表数据
* Return         : None
*******************************************************************************/
void MPR121_ALG_Update_algListNode(alg_ListNode* p[], uint8_t index, uint16_t dat)
{
  p[index] = p[index]->next;
  p[index]->dat = dat;
}

/*******************************************************************************
* Function Name  : MPR121_ALG_Judge_Cap_Mouse
* Description    : MPR121算法判断触摸板
* Input          : None
* Return         : None
*******************************************************************************/
void MPR121_ALG_Judge_Cap_Mouse(void)
{
  Mousestate* const data = (Mousestate*)&HID_DATA[1];
  signed char XMovement = 0;
  signed char YMovement = 0;
  signed char sign_state_x, sign_state_y;
  uint8_t temp;
  uint16_t now_dat = MPR121_ELE_to_Pin(cap_mouse_sts_head[0]->dat, MPR121_Cap_Mouse_Pinmux, 4);

  /* judge dir-up */
  if (now_dat & ALG_PINMUX_BIT(ALG_MOVE_UP_INDEX)) {
    YMovement--;
  }
  /* judge dir-down */
  if (now_dat & ALG_PINMUX_BIT(ALG_MOVE_DOWN_INDEX)) {
    YMovement++;
  }
  /* judge dir-left */
  if (now_dat & ALG_PINMUX_BIT(ALG_MOVE_LEFT_INDEX)) {
    XMovement--;
  }
  /* judge dir-right -> special */
  if (now_dat & ALG_PINMUX_BIT(ALG_MOVE_RIGHT_INDEX)) {
    XMovement++;
  }
  if (now_dat) {
    sign_state_x = __SIGN_STATE__(data->XMovement);
    sign_state_y = __SIGN_STATE__(data->YMovement);
    if (sign_state_x == __SIGN_STATE__(XMovement) && sign_state_y == __SIGN_STATE__(YMovement)) {
      data->XMovement = __ABS_LIMIT__(data->XMovement + 1 * sign_state_x, 15);
      data->YMovement = __ABS_LIMIT__(data->YMovement + 1 * sign_state_y, 15);
    } else {
      data->XMovement = (uint8_t)XMovement;
      data->YMovement = (uint8_t)YMovement;
    }
    cap_mouse_data_change = TRUE;
  } else {  // release
    data->XMovement = 0;
    data->YMovement = 0;
    cap_mouse_data_change = TRUE;
  }
}

#if 0
/*******************************************************************************
* Function Name  : MPR121_ALG_Judge_Cap_Mouse_2
* Description    : MPR121算法判断触摸板
* Input          : None
* Return         : None
*******************************************************************************/
void MPR121_ALG_Judge_Cap_Mouse_2(void)
{
  Mousestate* const data = (Mousestate*)&HID_DATA[1];
  signed char XMovement = 0;
  signed char YMovement = 0;
  uint16_t sum1, sum2;
  uint8_t ret;
  uint16_t now_dat = MPR121_ELE_to_Pin(cap_mouse_sts_head[0]->dat, MPR121_Cap_Mouse_Pinmux, 4);
  uint16_t pre_dat = MPR121_ELE_to_Pin(cap_mouse_sts_head[0]->prev->dat, MPR121_Cap_Mouse_Pinmux, 4);

  /* assert CAP_MOUSE_BUF_LEN == 2 */
  /* judge up */
  sum1 = cap_mouse_dat_head[ALG_MOVE_UP_INDEX]->dat/2;
  sum2 = cap_mouse_dat_head[ALG_MOVE_UP_INDEX]->prev->dat/2;
  if (sum1 < sum2 && (sum2 - sum1) > mpr_algParameter.cap_mouse_tou_thr) YMovement--;
  else if (sum1 > sum2 && (sum1 - sum2) > mpr_algParameter.cap_mouse_tou_thr) YMovement++;
  /* judge down */
  sum1 = cap_mouse_dat_head[ALG_MOVE_DOWN_INDEX]->dat/2;
  sum2 = cap_mouse_dat_head[ALG_MOVE_DOWN_INDEX]->prev->dat/2;
  if (sum1 < sum2 && (sum2 - sum1) > mpr_algParameter.cap_mouse_tou_thr) YMovement++;
  else if (sum1 > sum2 && (sum1 - sum2) > mpr_algParameter.cap_mouse_tou_thr) YMovement--;
  /* judge left */
  sum1 = cap_mouse_dat_head[ALG_MOVE_LEFT_INDEX]->dat/2;
  sum2 = cap_mouse_dat_head[ALG_MOVE_LEFT_INDEX]->prev->dat/2;
  if (sum1 < sum2 && (sum2 - sum1) > mpr_algParameter.cap_mouse_tou_thr) XMovement++;
  else if (sum1 > sum2 && (sum1 - sum2) > mpr_algParameter.cap_mouse_tou_thr) XMovement--;
  /* judge right */
  sum1 = cap_mouse_dat_head[ALG_MOVE_RIGHT_INDEX]->dat/2;
  sum2 = cap_mouse_dat_head[ALG_MOVE_RIGHT_INDEX]->prev->dat/2;
  if (sum1 < sum2 && (sum2 - sum1) > mpr_algParameter.cap_mouse_tou_thr) XMovement--;
  else if (sum1 > sum2 && (sum1 - sum2) > mpr_algParameter.cap_mouse_tou_thr) XMovement++;
  /* prepare ret */
//  if (XMovement != 0) {
//    if (XMovement <= 2) XMovement--;
//    else XMovement++;
//  }
//  if (YMovement != 0) {
//    if (YMovement <= 2) YMovement--;
//    else YMovement++;
//  }
  XMovement *= 5;
  YMovement *= 5; // multiple
  ret = ((data->XMovement != XMovement) || (data->YMovement != YMovement)) && (XMovement || YMovement);
  if (ret && pre_dat != 0) {
    data->XMovement = (uint8_t)XMovement;
    data->YMovement = (uint8_t)YMovement;
    cap_mouse_data_change = TRUE;
  } else if ((XMovement == 0 && YMovement == 0) && now_dat != 0) {  // unrelease
    cap_mouse_data_change = TRUE; // keep last move
  } else if ((data->XMovement || data->YMovement) && now_dat == 0) {  // judge release
    data->XMovement = 0;
    data->YMovement = 0;
    cap_mouse_data_change = TRUE;
  }
}
#endif

/*******************************************************************************
* Function Name  : MPR121_ALG_Judge_Touchbar
* Description    : MPR121算法判断触摸条
* Input          : None
* Return         : None
*******************************************************************************/
void MPR121_ALG_Judge_Touchbar(void)
{
  /* read touch status version */
  static alg_TouchbarDirectiion dir = DIRECT_OTHER;
  Mousestate* const m_data = (Mousestate*)&HID_DATA[1];;
  uint8_t record;
  uint16_t now_dat = MPR121_ELE_to_Pin(touchbar_head[0]->dat, MPR121_TouchBar_Pinmux, 7);
  uint16_t pre_dat = MPR121_ELE_to_Pin(touchbar_head[0]->prev->dat, MPR121_TouchBar_Pinmux, 7);

  record = m_data->data[0] & 0x7;

  if (now_dat != pre_dat) { // Btn0 => Btn1 or Btn1 => Btn0
    mpr_algParameter.btn_dat = now_dat;  // set btn data
    mpr_algParameter.cnt_dat = 0;
    if ((now_dat & 0xF) == 0 && (pre_dat & 0xF) != 0) { // left Btn1 => Btn0
      mpr_algParameter.dbtn_dat = now_dat; // next touch is double touch
    }
    if (now_dat > pre_dat) { // swip to right
      if (dir & 1) dir = DIRECT_RIGHT;
      else {
        dir += 2;
        if (dir == DIRECT_JUDGE_RIGHT) {  // judge to swipe right
          dir = DIRECT_OTHER;
          Touchbardat.swip_right = TRUE;
          OLED_PRINT("RIGHT!");
        }
      }
    } else if (now_dat < pre_dat) { // swip to left
      if (!(dir & 1)) dir = DIRECT_LEFT;
      else {
        dir += 2;
        if (dir == DIRECT_JUDGE_LEFT) { // judge to swipe left
          dir = DIRECT_OTHER;
          Touchbardat.swip_left = TRUE;
          OLED_PRINT("LEFT!");
        }
      }
    }
  } else {  // Btn1 => Btn1 or Btn0 => Btn0
    mpr_algParameter.cnt_dat = __LIMIT__(mpr_algParameter.cnt_dat+1, 65534);
    if ((now_dat & 0xF) == 0 && (pre_dat & 0xF) == 0) { // left Btn0 => Btn0
      if (mpr_algParameter.cnt_dat >= mpr_algParameter.double_touch_cnt) {
        mpr_algParameter.dbtn_dat = 0;
      }
      m_data->data[0] &= ~0x1;  // release left btn
    } else {  // left Btn1 => Btn1
      if (mpr_algParameter.dbtn_dat != 0) {
        mpr_algParameter.dbtn_dat = 0;
        m_data->LeftBtn = 1;  // left btn = 1
      }
    }
    if ((now_dat & 0x10) == 0 && (pre_dat & 0x10) == 0) { // middle Btn0 => Btn0
      m_data->data[0] &= ~0x4;  // release middle btn
    } else {  // middle Btn1 => Btn1
      if (mpr_algParameter.cnt_dat >= mpr_algParameter.long_touch_cnt) {
        m_data->MiddleBtn = 1;  // middle btn = 1
      }
    }
    if ((now_dat & 0x1E0) == 0 && (pre_dat & 0x1E0) == 0) { // right Btn0 => Btn0
      m_data->data[0] &= ~0x2;  // release right btn
    } else {  // right Btn1 => Btn1
      if (mpr_algParameter.cnt_dat >= mpr_algParameter.long_touch_cnt) {
        m_data->RightBtn = 1; // right btn = 1
      }
    }
  }

  if ((m_data->data[0] & 0x7) != record) {
    touchbar_data_change = TRUE;
  }
}
