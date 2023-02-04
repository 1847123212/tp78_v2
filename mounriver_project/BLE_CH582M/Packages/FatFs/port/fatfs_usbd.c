#include "diskio.h"
#include "string.h"
#include "HAL.h"

#define BLOCK_SIZE  512
#define BLOCK_COUNT 64

int USB_disk_status(void)
{
    return RES_OK;
}

int USB_disk_initialize(void)
{
    return RES_OK;
}

int USB_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    EEPROM_READ(sector * BLOCK_SIZE, buff, count);
    return 0;
}

int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    EEPROM_ERASE(sector * BLOCK_SIZE, count);
    EEPROM_WRITE(sector * BLOCK_SIZE, (BYTE*)buff, count);
    return 0;
}

int USB_disk_ioctl(BYTE cmd, void *buff)
{
    int result = 0;

    switch (cmd) {
        case CTRL_SYNC:
            result = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = BLOCK_SIZE;
            result = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = BLOCK_COUNT;
            result = RES_OK;
            break;

        default:
            result = RES_PARERR;
            break;
    }

    return result;
}

const BYTE WriteBuffer[] = "test ok!";

void HAL_Fs_init(void)
{
  FATFS fs;                                                   // FatFs�ļ�ϵͳ����
  FIL fnew;                                                   // �ļ�����
  FRESULT res_flash;                                          // �ļ��������
  DIR dire;                                                   // Ŀ¼����
  FILINFO fnow;                                               // ���徲̬�ļ���Ϣ�ṹ����
  BYTE work[FF_MAX_SS];
  UINT fnum;                                                  // �ļ��ɹ���д����

  res_flash = f_mount( &fs, "0:", 1 );                        // �����ļ�ϵͳ������ 0

  if( res_flash == FR_NO_FILESYSTEM )                         // û���ļ�ϵͳ
  {
      res_flash = f_mkfs( "0:", 0, work, sizeof(work) );      // ��ʽ�� ���������ļ�ϵͳ
      res_flash = f_mount( NULL, "0:", 1 );                   // ��ʽ������ȡ������
      res_flash = f_mount( &fs,  "0:", 1 );                   // ���¹���
      OLED_PRINT("res:%d", res_flash);
  }
  if( res_flash == FR_OK ) {                           // ���سɹ�
      res_flash = f_open( &fnew, "0:TestFile.txt", FA_CREATE_ALWAYS | FA_WRITE );             // ��д�뷽ʽ���ļ�����δ�����ļ����½��ļ�
      if( res_flash == FR_OK ) {
        res_flash = f_write( &fnew, WriteBuffer, strlen(WriteBuffer), &fnum );                // д���ļ���д��ָ������
        f_close(&fnew);                                                                       // �ر��ļ�
      }
  }
  f_mount( NULL, "0:", 1 );                                    // ж���ļ�ϵͳ
}
