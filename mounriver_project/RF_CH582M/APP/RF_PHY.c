/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "HAL.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
tmosTaskID RFtaskID;
uint8_t TX_DATA[2] = {0xff, 0xff};
uint8_t CAPSLOCK_DATA[2] = {3, 0};

/*********************************************************************
 * @fn      RF_2G4StatusCallBack
 *
 * @brief   RF ״̬�ص���ע�⣺�����ڴ˺�����ֱ�ӵ���RF���ջ��߷���API����Ҫʹ���¼��ķ�ʽ����
 *
 * @param   sta     - ״̬����
 * @param   crc     - crcУ����
 * @param   rxBuf   - ����bufָ��
 *
 * @return  none
 */
void RF_2G4StatusCallBack(uint8_t sta, uint8_t crc, uint8_t *rxBuf)
{
    switch(sta)
    {
        case TX_MODE_TX_FINISH:
        {
            break;
        }
        case TX_MODE_TX_FAIL:
        {
            break;
        }
        case TX_MODE_RX_DATA:
        {
            RF_Shut();
            if(crc == 1)
            {
                PRINT("crc error\n");
            }
            else if(crc == 2)
            {
                PRINT("match type error\n");
            }
            else
            {
                uint8_t i;
                PRINT("tx recv,rssi:%d\n", (int8_t)rxBuf[0]);
                PRINT("len:%d-", rxBuf[1]);
                if (rxBuf[1] <= sizeof(HID_dat)) {
                    for(i = 0; i < rxBuf[1]; i++) {
                        PRINT("%x ", rxBuf[i + 2]);
                        *((uint8_t*)&HID_dat + i) = rxBuf[i + 2];
                    }
                    if (USB_Ready == TRUE) {
                        tmos_set_event(usbTaskID, 1<<HID_dat.label);
                    }
                    PRINT("\n");
                } else {
                    PRINT("receive err!\n");
                }
            }
            break;
        }
        case TX_MODE_RX_TIMEOUT: // Timeout is about 200us
        {
            break;
        }
        case RX_MODE_RX_DATA:
        {
            if(crc == 1)
            {
                PRINT("crc error\n");
            }
            else if(crc == 2)
            {
                PRINT("match type error\n");
            }
            else
            {
                uint8_t i;
                PRINT("rx recv, rssi: %d\n", (int8_t)rxBuf[0]);
                PRINT("len:%d-", rxBuf[1]);
                if (rxBuf[1] <= sizeof(HID_dat)) {
                    for(i = 0; i < rxBuf[1]; i++) {
                        PRINT("%x ", rxBuf[i + 2]);
                        *((uint8_t*)&HID_dat + i) = rxBuf[i + 2];
                    }
                    if (USB_Ready == TRUE) {
                        tmos_set_event(usbTaskID, 1<<HID_dat.label);
                    }
                    PRINT("\n");
                } else {
                    PRINT("receive err!\n");
                }
            }
            tmos_set_event(RFtaskID, SBP_RF_RF_RX_EVT);
            break;
        }
        case RX_MODE_TX_FINISH:
        {
            tmos_set_event(RFtaskID, SBP_RF_RF_RX_EVT);
            break;
        }
        case RX_MODE_TX_FAIL:
        {
            break;
        }
    }
    PRINT("STA: %x\n", sta);
}

/*********************************************************************
 * @fn      RF_ProcessEvent
 *
 * @brief   RF �¼�����
 *
 * @param   task_id - ����ID
 * @param   events  - �¼���־
 *
 * @return  δ����¼�
 */
uint16_t RF_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(task_id)) != NULL)
        {
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
//    if(events & SBP_RF_START_DEVICE_EVT)
//    {
//        tmos_start_task(RFtaskID, SBP_RF_PERIODIC_EVT, 1000);
//        return events ^ SBP_RF_START_DEVICE_EVT;
//    }
//    if(events & SBP_RF_PERIODIC_EVT)
//    {
//        RF_Shut();
//        RF_Tx(TX_DATA, 2, 0xFF, 0xFF);
//        tmos_start_task(RFtaskID, SBP_RF_PERIODIC_EVT, 1000);
//        return events ^ SBP_RF_PERIODIC_EVT;
//    }
    if(events & SBP_RF_RF_RX_EVT)
    {
        uint8_t state;
        RF_Shut();
        state = RF_Rx(TX_DATA, 2, 0xFF, 0xFF);
        PRINT("RX mode.state = %x\n", state);
        return events ^ SBP_RF_RF_RX_EVT;
    }
    if(events & SBP_RF_TX_CAPSLOCK_EVT)
    {
        uint8_t state;
        RF_Shut();
        CAPSLOCK_DATA[1] = USB_CapsLock_LEDOn;
        state = RF_Rx(TX_DATA, 2, 0xFF, 0xFF);
        PRINT("RX mode.state = %x\n", state);
        return events ^ SBP_RF_TX_CAPSLOCK_EVT;
    }
    return 0;
}

/*********************************************************************
 * @fn      RF_Init
 *
 * @brief   RF ��ʼ��
 *
 * @return  none
 */
void RF_Init(void)
{
    uint8_t    state;
    rfConfig_t rfConfig;

    tmos_memset(&rfConfig, 0, sizeof(rfConfig_t));
    RFtaskID = TMOS_ProcessEventRegister(RF_ProcessEvent);
    rfConfig.accessAddress = 0x71764129; // ��ֹʹ��0x55555555�Լ�0xAAAAAAAA ( ���鲻����24��λ��ת���Ҳ�����������6��0��1 )
    rfConfig.CRCInit = 0x555555;
    rfConfig.Channel = 8;
    rfConfig.Frequency = 2480000;
    rfConfig.LLEMode = LLE_MODE_BASIC | LLE_MODE_EX_CHANNEL; // ʹ�� LLE_MODE_EX_CHANNEL ��ʾ ѡ�� rfConfig.Frequency ��Ϊͨ��Ƶ��
    rfConfig.rfStatusCB = RF_2G4StatusCallBack;
    rfConfig.RxMaxlen = 251;
    state = RF_Config(&rfConfig);
    PRINT("rf 2.4g init: %x\n", state);
    { // RX mode
        state = RF_Rx(TX_DATA, 2, 0xFF, 0xFF);
        PRINT("RX mode.state = %x\n", state);
    }

    //	{ // TX mode
    //		tmos_set_event( RFtaskID , SBP_RF_PERIODIC_EVT );
    //	}
}

/******************************** endfile @ main ******************************/
