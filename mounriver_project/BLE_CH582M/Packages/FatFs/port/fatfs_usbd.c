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
  FATFS fs;                                                   // FatFs文件系统对象
  FIL fnew;                                                   // 文件对象
  FRESULT res_flash;                                          // 文件操作结果
  DIR dire;                                                   // 目录对象
  FILINFO fnow;                                               // 定义静态文件信息结构对象
  BYTE work[FF_MAX_SS];
  UINT fnum;                                                  // 文件成功读写数量

  res_flash = f_mount( &fs, "0:", 1 );                        // 挂载文件系统到分区 0

  if( res_flash == FR_NO_FILESYSTEM )                         // 没有文件系统
  {
      res_flash = f_mkfs( "0:", 0, work, sizeof(work) );      // 格式化 创建创建文件系统
      res_flash = f_mount( NULL, "0:", 1 );                   // 格式化后，先取消挂载
      res_flash = f_mount( &fs,  "0:", 1 );                   // 重新挂载
      OLED_PRINT("res:%d", res_flash);
  }
  if( res_flash == FR_OK ) {                           // 挂载成功
      res_flash = f_open( &fnew, "0:TestFile.txt", FA_CREATE_ALWAYS | FA_WRITE );             // 以写入方式打开文件，若未发现文件则新建文件
      if( res_flash == FR_OK ) {
        res_flash = f_write( &fnew, WriteBuffer, strlen(WriteBuffer), &fnum );                // 写向文件内写入指定数据
        f_close(&fnew);                                                                       // 关闭文件
      }
  }
  f_mount( NULL, "0:", 1 );                                    // 卸载文件系统
}
