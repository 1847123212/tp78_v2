/********************************** (C) COPYRIGHT *******************************
 * File Name          : RF_PHY.h
 * Author             : ChnMasterOG
 * Version            : V1.0
 * Date               : 2022/11/13
 * Description        : RF 2.4GӦ�ó���
 *******************************************************************************/

#ifndef CENTRAL_H
#define CENTRAL_H

#ifdef __cplusplus
extern "C" {
#endif

#define SBP_RF_START_DEVICE_EVT      0x1
#define SBP_RF_PERIODIC_EVT          0x2
#define SBP_RF_RF_RX_EVT             0x4
#define SBP_RF_KEYBOARD_REPORT_EVT   0x8
#define SBP_RF_MOUSE_REPORT_EVT      0x10
#define SBP_RF_VOL_REPORT_EVT        0x20
#define SBP_RF_JUMPBOOT_REPORT_EVT   0x40
#define SBP_RF_CHANNEL_HOP_TX_EVT    (1 << 3)
#define SBP_RF_CHANNEL_HOP_RX_EVT    (1 << 4)

#define LLE_MODE_ORIGINAL_RX         (0x80) //�������LLEMODEʱ���ϴ˺꣬����յ�һ�ֽ�Ϊԭʼ���ݣ�ԭ��ΪRSSI��

extern void RF_Init(void);
extern void DATAFLASH_Read_RForBLE(void);
extern void DATAFLASH_Write_RForBLE(uint8_t rf_ready);

extern tmosTaskID RFTaskId;

#ifdef __cplusplus
}
#endif

#endif
