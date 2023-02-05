/********************************** (C) COPYRIGHT *******************************
 * File Name          : FATFS.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/2/5
 * Description        : FatFs应用层驱动
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "diskio.h"
#include "string.h"
#include "HAL.h"

const BYTE WriteBuffer[] = {
    "---TP78改键说明---\n"
    "改键后需要重启键盘生效(Fn+R)\n"
    "---文件目录说明(数字均为十进制)---\n"
    "keyboard_cfg.txt---存放键盘配置\n"
    "按行顺序分别表示:\n"
    "默认蓝牙设备(1~4)\n"
    "默认LED模式(0~4)\n"
    "是否启用RF(0~1)\n"
    "MPR_alg_magic\n"
    "MPR_touchbar_tou_thr\n"
    "MPR_touchbar_rel_thr\n"
    "MPR_capmouse_tou_thr\n"
    "MPR_capmouse_rel_thr\n"
    "keyboard_mat.txt---存放键盘按键映射\n"
    "keyboard_ext_mat.txt---存放键盘层2按键映射\n"
};

/*******************************************************************************
* Function Name  : HAL_Fs_Init
* Description    : 初始化文件系统
* Input          : debug_info - 错误信息
* Return         : None
*******************************************************************************/
void HAL_Fs_Init(char* debug_info)
{
  FATFS fs;                                                   // FatFs文件系统对象
  FIL fnew;                                                   // 文件对象
  FRESULT res_flash;                                          // 文件操作结果
  FILINFO fnow;                                               // 定义静态文件信息结构对象
  BYTE work[FF_MAX_SS];
  UINT fnum;                                                  // 文件成功读写数量

  res_flash = f_mount( &fs, "0:", 1 );                        // 挂载文件系统到分区 0

  if( res_flash == FR_NO_FILESYSTEM )                         // 没有文件系统
  {
      res_flash = f_mkfs( "0:", 0, work, sizeof(work) );      // 格式化 创建创建文件系统
      res_flash = f_mount( NULL, "0:", 1 );                   // 格式化后，先取消挂载
      res_flash = f_mount( &fs,  "0:", 1 );                   // 重新挂载
  }
  if( res_flash == FR_OK ) {                                  // 挂载成功
#ifdef FIRST_USED // 出产设置
      res_flash = f_open( &fnew, "0:readme.txt", FA_CREATE_ALWAYS | FA_WRITE );               // 以写入方式打开文件，若未发现文件则新建文件
      if( res_flash == FR_OK ) {
        res_flash = f_write( &fnew, WriteBuffer, strlen(WriteBuffer), &fnum );                // 写向文件内写入指定数据
        f_close(&fnew);                                                                       // 关闭文件
        g_Ready_Status.fatfs = TRUE;
      }
#else
      g_Ready_Status.fatfs = TRUE;
#endif
  } else {
      strcpy(debug_info, "FATFS-FAIL");
      g_Ready_Status.fatfs = FALSE;
  }
  f_mount( NULL, "0:", 1 );                                    // 卸载文件系统
}

/*******************************************************************************
* Function Name  : u8_dec_to_string
* Description    : u8类型转换为十进制字符串
* Input          : num - u8类型; buff - char指针存放字符串;
*                  type - 0默认, 1末尾放置空格, 2末尾放置换行
* Return         : 字符串长度
*******************************************************************************/
uint8_t u8_dec_to_string(uint8_t num, char *buff, uint8_t type)
{
  uint8_t len, ret;

  if (num <= 9) len = 1;
  else if (num <= 99) len = 2;
  else len = 3;

  if (type == 1) {
    buff[len] = ' ';
    buff[len + 1] = '\0';
    ret = len + 1;
  } else if (type == 2) {
    buff[len] = '\n';
    buff[len + 1] = '\0';
    ret = len + 1;
  } else {
    buff[len] = '\0';
    ret = len;
  }

  do {
    --len;
    buff[len] = num % 10 + '0';
    num /= 10;
  } while (num != 0);

  return ret;
}

/*******************************************************************************
* Function Name  : string_dec_to_u8
* Description    : 十进制字符串转换为u8类型
* Input          : buff - char指针存放字符串; num - u8类型存放的指针
* Return         : 指向数字末尾+1个字符
*******************************************************************************/
uint8_t string_dec_to_u8(char *buff, uint8_t *num)
{
  uint8_t ret = 1;
  *num = 0;

  while (*buff != '\0' && *buff != ' ' && *buff != '\n') {
    *num *= 10;
    *num += *buff - '0';
    buff++;
    ret++;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : HAL_Fs_Create_keyboard_cfg
* Description    : 读取keyboard_cfg.txt参数
* Input          : len - 写入长度, 单位: 行数(至少为1); p_cfg - 参数指针
* Return         : None
*******************************************************************************/
void HAL_Fs_Create_keyboard_cfg(uint8_t len, uint8_t* p_cfg)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t l = 0;
  uint8_t wr_buf[512];

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, "0:keyboard_cfg.txt", FA_CREATE_ALWAYS | FA_WRITE );
  if ( res_flash != FR_OK ) goto fs_create_kbd_cfg_end;

  while (len) {
    l += u8_dec_to_string(*p_cfg, &wr_buf[l], 2);
    p_cfg++;
    len--;
  }
  f_write( &fnew, wr_buf, l, &fnum );

  f_close(&fnew);

  fs_create_kbd_cfg_end:
  f_mount( NULL, "0:", 1 );                                    // 卸载文件系统
}

/*******************************************************************************
* Function Name  : HAL_Fs_Write_keyboard_cfg
* Description    : 向keyboard_cfg.txt写参数
* Input          : fs_line - 开始行数(0为第1行); len - 写入长度, 单位: 行数(至少为1); p_cfg - 参数指针
* Return         : None
*******************************************************************************/
void HAL_Fs_Write_keyboard_cfg(uint8_t fs_line, uint8_t len, uint8_t* p_cfg)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t l = 0;
  uint8_t rd_buf[512];

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, "0:keyboard_cfg.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE );
  if ( res_flash != FR_OK ) goto fs_write_kbd_cfg_end;    // 文件不存在

  res_flash = f_read(&fnew, rd_buf, sizeof(rd_buf), &fnum);
  while (fs_line) {
    l += string_dec_to_u8(&rd_buf[l], p_cfg);
    fs_line--;
  }
  fs_line = l;
  while (len) {
    l += u8_dec_to_string(*p_cfg, &rd_buf[l], 2);
    p_cfg++;
    len--;
  }
  f_lseek( &fnew, fs_line );
  f_write( &fnew, &rd_buf[fs_line], l - fs_line, &fnum );

  f_close(&fnew);

  fs_write_kbd_cfg_end:
  f_mount( NULL, "0:", 1 );                                    // 卸载文件系统
}

/*******************************************************************************
* Function Name  : HAL_Fs_Read_keyboard_cfg
* Description    : 读取keyboard_cfg.txt参数
* Input          : fs_line - 开始行数(0为第1行); len - 读取长度, 单位: 行数(至少为1); p_cfg - 参数指针
* Return         : None
*******************************************************************************/
void HAL_Fs_Read_keyboard_cfg(uint8_t fs_line, uint8_t len, uint8_t* p_cfg)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t l = 0;
  uint8_t rd_buf[512];

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, "0:keyboard_cfg.txt", FA_OPEN_EXISTING | FA_READ );
  if ( res_flash != FR_OK ) goto fs_read_kbd_cfg_end;    // 文件不存在

  res_flash = f_read(&fnew, rd_buf, sizeof(rd_buf), &fnum);
  while (fs_line) {
    l += string_dec_to_u8(&rd_buf[l], p_cfg);
    fs_line--;
  }
  while (len) {
    l += string_dec_to_u8(&rd_buf[l], p_cfg);
    p_cfg++;
    len--;
  }

  f_close(&fnew);

  fs_read_kbd_cfg_end:
  f_mount( NULL, "0:", 1 );                                    // 卸载文件系统
}

/*******************************************************************************
* Function Name  : HAL_Fs_Write_keyboard_mat
* Description    : 写入键盘布局
* Input          : fp - 文件系统路径: 如"0:keyboard_mat.txt", "0:keyboard_ext_mat.txt"
*                  keyarr - RAM zone按键信息矩阵: 如CustomKey, Extra_CustomKey
* Return         : None
*******************************************************************************/
void HAL_Fs_Write_keyboard_mat(const uint8_t* fp, const uint8_t* key_arr)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t i, j, len;
  uint8_t wr_buf[512];  // 最大4*COL_SIZE*ROW_SIZE

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open( &fnew, fp, FA_CREATE_ALWAYS | FA_WRITE );
  if ( res_flash != FR_OK ) goto fs_write_kbd_mat_end;

  len = 0;
  for (i = 0; i < ROW_SIZE; i++) {
    for (j = 0; j < COL_SIZE; j++) {
      if (j == COL_SIZE - 1) len += u8_dec_to_string(key_arr[i + j * ROW_SIZE], &wr_buf[len], 2);
      else len += u8_dec_to_string(key_arr[i + j * ROW_SIZE], &wr_buf[len], 1);
    }
  }

  f_write( &fnew, wr_buf, len, &fnum );

  f_close(&fnew);

  fs_write_kbd_mat_end:
  f_mount( NULL, "0:", 1 );                                    // 卸载文件系统
}

/*******************************************************************************
* Function Name  : HAL_Fs_Read_keyboard_mat
* Description    : 读取键盘布局
* Input          : fp - 文件系统路径: 如"0:keyboard_mat.txt", "0:keyboard_ext_mat.txt"
*                  keyarr - RAM zone按键信息矩阵: 如CustomKey, Extra_CustomKey
* Return         : None
*******************************************************************************/
void HAL_Fs_Read_keyboard_mat(const uint8_t* fp, uint8_t* key_arr)
{
  FATFS fs;
  FIL fnew;
  FRESULT res_flash;
  FILINFO fnow;
  UINT fnum;

  uint8_t i, j, len;
  uint8_t rd_buf[512];  // 最大4*COL_SIZE*ROW_SIZE

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, fp, FA_OPEN_EXISTING | FA_READ );
  if ( res_flash != FR_OK ) goto fs_read_kbd_mat_end;    // 文件不存在

  res_flash = f_read(&fnew, rd_buf, sizeof(rd_buf), &fnum);
  if ( res_flash == FR_OK ) {
    for (i = 0; i < ROW_SIZE; i++) {
      for (j = 0; j < COL_SIZE; j++) {
        len += string_dec_to_u8(&rd_buf[len], &key_arr[i + j * ROW_SIZE]);
      }
    }
  }

  f_close(&fnew);

  fs_read_kbd_mat_end:
  f_mount( NULL, "0:", 1 );                                    // 卸载文件系统
}
