/********************************** (C) COPYRIGHT *******************************
 * File Name          : FATFS.c
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2023/2/5
 * Description        : FatFsӦ�ò�����
 * SPDX-License-Identifier: GPL-3.0
 *******************************************************************************/

#include "diskio.h"
#include "string.h"
#include "HAL.h"

const BYTE WriteBuffer[] = {
    "---TP78�ļ�˵��---\n"
    "�ļ�����Ҫ����������Ч(Fn+R)\n"
    "---�ļ�Ŀ¼˵��(���־�Ϊʮ����)---\n"
    "keyboard_cfg.txt---��ż�������\n"
    "����˳��ֱ��ʾ:\n"
    "Ĭ�������豸(1~4)\n"
    "Ĭ��LEDģʽ(0~4)\n"
    "�Ƿ�����RF(0~1)\n"
    "MPR_alg_magic\n"
    "MPR_touchbar_tou_thr\n"
    "MPR_touchbar_rel_thr\n"
    "MPR_capmouse_tou_thr\n"
    "MPR_capmouse_rel_thr\n"
    "keyboard_mat.txt---��ż��̰���ӳ��\n"
    "keyboard_ext_mat.txt---��ż��̲�2����ӳ��\n"
};

/*******************************************************************************
* Function Name  : HAL_Fs_Init
* Description    : ��ʼ���ļ�ϵͳ
* Input          : debug_info - ������Ϣ
* Return         : None
*******************************************************************************/
void HAL_Fs_Init(char* debug_info)
{
  FATFS fs;                                                   // FatFs�ļ�ϵͳ����
  FIL fnew;                                                   // �ļ�����
  FRESULT res_flash;                                          // �ļ��������
  FILINFO fnow;                                               // ���徲̬�ļ���Ϣ�ṹ����
  BYTE work[FF_MAX_SS];
  UINT fnum;                                                  // �ļ��ɹ���д����

  res_flash = f_mount( &fs, "0:", 1 );                        // �����ļ�ϵͳ������ 0

  if( res_flash == FR_NO_FILESYSTEM )                         // û���ļ�ϵͳ
  {
      res_flash = f_mkfs( "0:", 0, work, sizeof(work) );      // ��ʽ�� ���������ļ�ϵͳ
      res_flash = f_mount( NULL, "0:", 1 );                   // ��ʽ������ȡ������
      res_flash = f_mount( &fs,  "0:", 1 );                   // ���¹���
  }
  if( res_flash == FR_OK ) {                                  // ���سɹ�
#ifdef FIRST_USED // ��������
      res_flash = f_open( &fnew, "0:readme.txt", FA_CREATE_ALWAYS | FA_WRITE );               // ��д�뷽ʽ���ļ�����δ�����ļ����½��ļ�
      if( res_flash == FR_OK ) {
        res_flash = f_write( &fnew, WriteBuffer, strlen(WriteBuffer), &fnum );                // д���ļ���д��ָ������
        f_close(&fnew);                                                                       // �ر��ļ�
        g_Ready_Status.fatfs = TRUE;
      }
#else
      g_Ready_Status.fatfs = TRUE;
#endif
  } else {
      strcpy(debug_info, "FATFS-FAIL");
      g_Ready_Status.fatfs = FALSE;
  }
  f_mount( NULL, "0:", 1 );                                    // ж���ļ�ϵͳ
}

/*******************************************************************************
* Function Name  : u8_dec_to_string
* Description    : u8����ת��Ϊʮ�����ַ���
* Input          : num - u8����; buff - charָ�����ַ���;
*                  type - 0Ĭ��, 1ĩβ���ÿո�, 2ĩβ���û���
* Return         : �ַ�������
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
* Description    : ʮ�����ַ���ת��Ϊu8����
* Input          : buff - charָ�����ַ���; num - u8���ʹ�ŵ�ָ��
* Return         : ָ������ĩβ+1���ַ�
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
* Description    : ��ȡkeyboard_cfg.txt����
* Input          : len - д�볤��, ��λ: ����(����Ϊ1); p_cfg - ����ָ��
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
  f_mount( NULL, "0:", 1 );                                    // ж���ļ�ϵͳ
}

/*******************************************************************************
* Function Name  : HAL_Fs_Write_keyboard_cfg
* Description    : ��keyboard_cfg.txtд����
* Input          : fs_line - ��ʼ����(0Ϊ��1��); len - д�볤��, ��λ: ����(����Ϊ1); p_cfg - ����ָ��
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
  if ( res_flash != FR_OK ) goto fs_write_kbd_cfg_end;    // �ļ�������

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
  f_mount( NULL, "0:", 1 );                                    // ж���ļ�ϵͳ
}

/*******************************************************************************
* Function Name  : HAL_Fs_Read_keyboard_cfg
* Description    : ��ȡkeyboard_cfg.txt����
* Input          : fs_line - ��ʼ����(0Ϊ��1��); len - ��ȡ����, ��λ: ����(����Ϊ1); p_cfg - ����ָ��
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
  if ( res_flash != FR_OK ) goto fs_read_kbd_cfg_end;    // �ļ�������

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
  f_mount( NULL, "0:", 1 );                                    // ж���ļ�ϵͳ
}

/*******************************************************************************
* Function Name  : HAL_Fs_Write_keyboard_mat
* Description    : д����̲���
* Input          : fp - �ļ�ϵͳ·��: ��"0:keyboard_mat.txt", "0:keyboard_ext_mat.txt"
*                  keyarr - RAM zone������Ϣ����: ��CustomKey, Extra_CustomKey
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
  uint8_t wr_buf[512];  // ���4*COL_SIZE*ROW_SIZE

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
  f_mount( NULL, "0:", 1 );                                    // ж���ļ�ϵͳ
}

/*******************************************************************************
* Function Name  : HAL_Fs_Read_keyboard_mat
* Description    : ��ȡ���̲���
* Input          : fp - �ļ�ϵͳ·��: ��"0:keyboard_mat.txt", "0:keyboard_ext_mat.txt"
*                  keyarr - RAM zone������Ϣ����: ��CustomKey, Extra_CustomKey
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
  uint8_t rd_buf[512];  // ���4*COL_SIZE*ROW_SIZE

  if (g_Ready_Status.fatfs == FALSE) return;

  res_flash = f_mount( &fs, "0:", 1 );
  if ( res_flash != FR_OK ) return;

  res_flash = f_open(&fnew, fp, FA_OPEN_EXISTING | FA_READ );
  if ( res_flash != FR_OK ) goto fs_read_kbd_mat_end;    // �ļ�������

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
  f_mount( NULL, "0:", 1 );                                    // ж���ļ�ϵͳ
}
