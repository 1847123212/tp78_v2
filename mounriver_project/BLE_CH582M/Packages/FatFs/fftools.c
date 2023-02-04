/*
 * fftools.c
 *
 *  Created on: 2022��5��11��
 *      Author: Administrator
 */
#include "fftools.h"
#include "string.h"
#include "stdio.h"

FRESULT scan_files( char* path )
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

    res = f_opendir( &dir, path );                              // ����Ŀ¼
    if (res == FR_OK)
    {
        while( 1 )
        {
            res = f_readdir( &dir, &fno );                      // ��ȡĿ¼��Ϣ
            if( ( res != FR_OK ) || ( fno.fname[0] == 0 ) )
            {
                break;                                          // �������δ�ҵ��ļ�
            }
            if( fno.fattrib & AM_DIR )                          // Ŀ��Ϊ�ļ���
            {
                i = strlen(path);                               // �����·������
                sprintf( &path[i], "/%s", fno.fname );          // �ļ����ӵ�·������
                res = scan_files( path );                       // ����Ŀ¼
                if (res != FR_OK) break;
                path[i] = 0;
            }
            else
            {
                printf("%s/%s\r\n", path, fno.fname);           // ��ӡ�ļ�
            }
        }
        f_closedir(&dir);
    }

    return res;
}
