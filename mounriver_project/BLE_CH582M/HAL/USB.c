/********************************** (C) COPYRIGHT *******************************
 * File Name          : USB.c
 * Author             : ChnMasterOG
 * Version            : V1.1
 * Date               : 2022/2/24
 * Description        : USB����Դ�ļ�
 *******************************************************************************/

#include "HAL.h"
#include "CH58x_common.h"

// �豸������
const UINT8 MyDevDescr[] = { 0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, DevEP0SIZE,
                             0x4A, 0x43,  // idVender=0x434A
                             0x40, 0x55,  // idProduct=0x5540
                             0x01, 0x01,  // bcdDevice=0x0101
                             0x01, 0x02, 0x00, 0x01
                           };

// ����������
const UINT8 MyCfgDescr[] = { 0x09, 0x02, 0x52, 0x00, 0x03, 0x01, 0x00, 0x80, 0x32,            //����������: 3���ӿ�
                             0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,            //�ӿ�������,���̣��ӿڱ��0
                             0x09, 0x21, 0x11, 0x01, 0x00, 0x01, 0x22, 0x3e, 0x00,            //HID�����������¼�������KeyRepDesc
                             0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0x0A,                        //�˵����������˵��ַ0x81 ���˵�1
                             0x09, 0x04, 0x01, 0x00, 0x01, 0x03, 0x01, 0x02, 0x00,            //�ӿ�������,��꣺�ӿڱ��1
                             0x09, 0x21, 0x10, 0x01, 0x00, 0x01, 0x22, 0x34, 0x00,            //HID�����������¼�������MouseRepDesc
                             0x07, 0x05, 0x82, 0x03, 0x04, 0x00, 0x0A,                        //�˵����������˵��ַ0x82 ���˵�2
                             0x09, 0x04, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,            //�ӿ�������,�Զ��壺�ӿڱ��2
                             0x07, 0x05, 0x83, 0x02, 0x40, 0x00, 0x00,                        //�˵����������˵��ַ0x83
                             0x07, 0x05, 0x03, 0x02, 0x40, 0x00, 0x00,                        //�˵����������˵��ַ0x03
                             //�˵��ַ Bit 3��0: The endpoint number Bit 6��4: Reserved, reset to zero
                           };
// ����������
const UINT8 MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// ������Ϣ
const UINT8 MyManuInfo[] = { 0x0E, 0x03, 'l', 0, 'a', 0, 'b', 0, '4', 0, '1', 0, '8', 0 };
// ��Ʒ��Ϣ
const UINT8 MyProdInfo[] = { 0x0A, 0x03, 'T', 0, 'P', 0, '7', 0, '8', 0 };
/*HID�౨��������*/
const UINT8 KeyRepDesc[] = { 0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00, 0x25,
                             0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x95, 0x03,
                             0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02, 0x95, 0x05, 0x75, 0x01, 0x91,
                             0x01, 0x95, 0x06, 0x75, 0x08, 0x26, 0xff, 0x00, 0x05, 0x07, 0x19, 0x00, 0x29, 0x91, 0x81,
                             0x00, 0xC0 };
const UINT8 MouseRepDesc[] = { 0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x09, 0x01, 0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29,
                               0x03, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x03, 0x81, 0x02, 0x75, 0x05, 0x95, 0x01,
                               0x81, 0x01, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x09, 0x38, 0x15, 0x81, 0x25, 0x7f, 0x75,
                               0x08, 0x95, 0x03, 0x81, 0x06, 0xC0, 0xC0 };

/**********************************************************/
UINT8 DevConfig;
UINT8 SetupReqCode;
UINT16 SetupReqLen;
const UINT8 *pDescr;
/*����������*/
UINT8 HIDMouse[4] = { 0x0, 0x0, 0x0, 0x0 };
UINT8 HIDKey[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

extern uint8_t CustomKey[14][6];  // �������̲������޸Ĵ˴�
extern uint8_t Extra_CustomKey[14][6];   // �������̲������޸Ĵ˴�

/******** �û��Զ������˵�RAM ****************************************/
__attribute__((aligned(4)))  UINT8 EP0_Databuf[64 + 64 + 64];   //ep0(64)+ep4_out(64)+ep4_in(64)
__attribute__((aligned(4)))  UINT8 EP1_Databuf[64 + 64];        //ep1_out(64)+ep1_in(64)
__attribute__((aligned(4)))  UINT8 EP2_Databuf[64 + 64];        //ep2_out(64)+ep2_in(64)
__attribute__((aligned(4)))  UINT8 EP3_Databuf[64 + 64];        //ep3_out(64)+ep3_in(64)

// Task id
tmosTaskID usbTaskID=INVALID_TASK_ID;
// USB ready flag
BOOL USB_Ready = FALSE;

/*******************************************************************************
* Function Name  : USB_ProcessEvent
* Description    : USB�����¼�
* Input          : task_id�� ����id, events: USB�¼�
* Output         : None
* Return         : tmosEvents
*******************************************************************************/
tmosEvents USB_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  if ( events & START_USB_EVENT )
  {
    PFIC_EnableIRQ( USB_IRQn );
    return events ^ START_USB_EVENT;
  }

  if ( events & USB_MOUSE_EVENT )
  {
    memcpy(pEP2_IN_DataBuf, HIDMouse, 4);
    DevEP2_IN_Deal( 4 );
    return events ^ USB_MOUSE_EVENT;
  }

  if ( events & USB_KEYBOARD_EVENT )
  {
    memcpy(pEP1_IN_DataBuf, HIDKey, 8);
    DevEP1_IN_Deal( 8 );
    return events ^ USB_KEYBOARD_EVENT;
  }

  if ( events & USB_TEST_EVENT )
  {
    HIDMouse[1] = HIDMouse[2] = 2;
    memcpy(pEP2_IN_DataBuf, HIDMouse, 4);
    DevEP2_IN_Deal( 4 );
//    pEP3_IN_DataBuf[0] = 'A';
//    DevEP3_IN_Deal( 1 );
    tmos_start_task(usbTaskID, USB_TEST_EVENT, MS1_TO_SYSTEM_TIME(500));
    return events ^ USB_TEST_EVENT;
  }

  return 0;
}

/*******************************************************************************
* Function Name  : HAL_USBInit
* Description    : USB��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HAL_USBInit( void )
{
  usbTaskID = TMOS_ProcessEventRegister( USB_ProcessEvent );
  pEP0_RAM_Addr = EP0_Databuf;
  pEP1_RAM_Addr = EP1_Databuf;
  pEP2_RAM_Addr = EP2_Databuf;
  pEP3_RAM_Addr = EP3_Databuf;
  USB_DeviceInit();
  tmos_start_task( usbTaskID, START_USB_EVENT, 0 );
}

/*******************************************************************************
* Function Name  : USB_DevTransProcess
* Description    : USB�жϴ��䴦��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_DevTransProcess( void )
{
  UINT8 len, chtype;
  UINT8 intflag, errflag = 0;

  intflag = R8_USB_INT_FG;
  if ( intflag & RB_UIF_TRANSFER )
  {
    if ( ( R8_USB_INT_ST & MASK_UIS_TOKEN ) != MASK_UIS_TOKEN )    // �ǿ���
    {
      switch ( R8_USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) )
      // �����������ƺͶ˵��
      {
        case UIS_TOKEN_IN : //USB������������PID 10b �˵�0
        {
          switch ( SetupReqCode )
          {
            case USB_GET_DESCRIPTOR :
              len = SetupReqLen >= DevEP0SIZE ?
                  DevEP0SIZE : SetupReqLen;    // ���δ��䳤��
              memcpy( pEP0_DataBuf, pDescr, len ); /* �����ϴ����� */
              SetupReqLen -= len;
              pDescr += len;
              R8_UEP0_T_LEN = len;
              R8_UEP0_CTRL ^= RB_UEP_T_TOG;                             // ��ת
              break;
            case USB_SET_ADDRESS :
              R8_USB_DEV_AD = ( R8_USB_DEV_AD & RB_UDA_GP_BIT ) | SetupReqLen;
              R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
              break;
            default :
              R8_UEP0_T_LEN = 0;                                      // ״̬�׶�����жϻ�����ǿ���ϴ�0�������ݰ��������ƴ���
              R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
              break;
          }
        }
          break;

        case UIS_TOKEN_OUT : //USB������������PID 00b �˵�0
        {
          len = R8_USB_RX_LEN;
          if ( SetupReqCode == 0x09 )
          {
          }
        }
          break;

        case UIS_TOKEN_OUT | 1 :    //USB������������PID 10b �˵�1
        {
          if ( R8_USB_INT_ST & RB_UIS_TOG_OK )
          {                       // ��ͬ�������ݰ�������
            len = R8_USB_RX_LEN;
            DevEP1_OUT_Deal( len );
          }
        }
          break;

        case UIS_TOKEN_IN | 1 :     //USB������������PID 00b �˵�1
          R8_UEP1_CTRL = ( R8_UEP1_CTRL & ~MASK_UEP_T_RES ) | UEP_T_RES_NAK;
          break;

        case UIS_TOKEN_OUT | 2 :    //USB������������PID 10b �˵�2
        {
          if ( R8_USB_INT_ST & RB_UIS_TOG_OK )
          {                       // ��ͬ�������ݰ�������
            len = R8_USB_RX_LEN;
            DevEP2_OUT_Deal( len );
          }
        }
          break;

        case UIS_TOKEN_IN | 2 :     //USB������������PID 00b �˵�2
          R8_UEP2_CTRL = ( R8_UEP2_CTRL & ~MASK_UEP_T_RES ) | UEP_T_RES_NAK;
          break;

        case UIS_TOKEN_OUT | 3 :    //USB������������PID 10b �˵�3
        {
          if ( R8_USB_INT_ST & RB_UIS_TOG_OK )
          {                       // ��ͬ�������ݰ�������
            len = R8_USB_RX_LEN;
            DevEP3_OUT_Deal( len );
          }
        }
          break;

        case UIS_TOKEN_IN | 3 :     //USB������������PID 00b �˵�3
          R8_UEP3_CTRL = ( R8_UEP3_CTRL & ~MASK_UEP_T_RES ) | UEP_T_RES_NAK;
          break;

        case UIS_TOKEN_OUT | 4 :    //USB������������PID 10b �˵�4
        {
          if ( R8_USB_INT_ST & RB_UIS_TOG_OK )
          {
            R8_UEP4_CTRL ^= RB_UEP_R_TOG;
            len = R8_USB_RX_LEN;
            DevEP4_OUT_Deal( len );
          }
        }
          break;

        case UIS_TOKEN_IN | 4 :     //USB������������PID 00b �˵�4
          R8_UEP4_CTRL ^= RB_UEP_T_TOG;
          R8_UEP4_CTRL = ( R8_UEP4_CTRL & ~MASK_UEP_T_RES ) | UEP_T_RES_NAK;
          break;

        default :
          break;
      }
      R8_USB_INT_FG = RB_UIF_TRANSFER;
    }
    if ( R8_USB_INT_ST & RB_UIS_SETUP_ACT )                  // Setup������
    {
      R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
      SetupReqLen = pSetupReqPak->wLength;
      SetupReqCode = pSetupReqPak->bRequest;
      chtype = pSetupReqPak->bRequestType;

      len = 0;
      errflag = 0;
      if ( ( pSetupReqPak->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
      {
        switch ( SetupReqCode )
        {
          case 0x0a :
            break;        //���һ��Ҫ��
          case 0x09 :
            break;
          default :
            errflag = 0xFF;
        }
      }
      else /* ��׼���� */
      {
        switch ( SetupReqCode )
        {
          case USB_GET_DESCRIPTOR :
          {
            switch ( ( ( pSetupReqPak->wValue ) >> 8 ) )
            {
              case USB_DESCR_TYP_DEVICE :
              {
                pDescr = MyDevDescr;
                len = MyDevDescr[0];
              }
                break;

              case USB_DESCR_TYP_CONFIG :
              {
                pDescr = MyCfgDescr;
                len = MyCfgDescr[2];
              }
                break;

              case USB_DESCR_TYP_REPORT :
              {
                if ( ( ( pSetupReqPak->wIndex ) & 0xff ) == 0 )         //�ӿ�0����������
                {
                  pDescr = KeyRepDesc;                                  //����׼���ϴ�
                  len = sizeof( KeyRepDesc );
                }
                else if ( ( ( pSetupReqPak->wIndex ) & 0xff ) == 1 )    //�ӿ�1����������
                {
                  pDescr = MouseRepDesc;                                //����׼���ϴ�
                  len = sizeof( MouseRepDesc );                         //����и���ӿڣ��ñ�׼λӦ�������һ���ӿ�������ɺ���Ч
                  USB_Ready = TRUE;
//                  tmos_start_task(usbTaskID, USB_TEST_EVENT, MS1_TO_SYSTEM_TIME(500));
                }
                else
                {
                  len = 0xff;                                           //������ֻ��3���ӿڣ���仰����������ִ��
                }
              }
                break;

              case USB_DESCR_TYP_STRING :
              {
                switch ( ( pSetupReqPak->wValue ) & 0xff )
                {
                  case 1 :
                    pDescr = MyManuInfo;
                    len = MyManuInfo[0];
                    break;
                  case 2 :
                    pDescr = MyProdInfo;
                    len = MyProdInfo[0];
                    break;
                  case 0 :
                    pDescr = MyLangDescr;
                    len = MyLangDescr[0];
                    break;
                  default :
                    errflag = 0xFF;                               // ��֧�ֵ��ַ���������
                    break;
                }
              }
                break;

              default :
                errflag = 0xff;
                break;
            }
            if ( SetupReqLen > len )
              SetupReqLen = len;      //ʵ�����ϴ��ܳ���
            len = ( SetupReqLen >= DevEP0SIZE ) ?
                DevEP0SIZE : SetupReqLen;
            memcpy( pEP0_DataBuf, pDescr, len );
            pDescr += len;
          }
            break;

          case USB_SET_ADDRESS :
            SetupReqLen = ( pSetupReqPak->wValue ) & 0xff;
            break;

          case USB_GET_CONFIGURATION :
            pEP0_DataBuf[0] = DevConfig;
            if ( SetupReqLen > 1 )
              SetupReqLen = 1;
            break;

          case USB_SET_CONFIGURATION :
            DevConfig = ( pSetupReqPak->wValue ) & 0xff;
            break;

          case USB_CLEAR_FEATURE :
          {
            if ( ( pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )    // �˵�
            {
              switch ( ( pSetupReqPak->wIndex ) & 0xff )
              {
                case 0x82 :
                  R8_UEP2_CTRL = ( R8_UEP2_CTRL & ~( RB_UEP_T_TOG | MASK_UEP_T_RES ) ) | UEP_T_RES_NAK;
                  break;
                case 0x02 :
                  R8_UEP2_CTRL = ( R8_UEP2_CTRL & ~( RB_UEP_R_TOG | MASK_UEP_R_RES ) ) | UEP_R_RES_ACK;
                  break;
                case 0x81 :
                  R8_UEP1_CTRL = ( R8_UEP1_CTRL & ~( RB_UEP_T_TOG | MASK_UEP_T_RES ) ) | UEP_T_RES_NAK;
                  break;
                case 0x01 :
                  R8_UEP1_CTRL = ( R8_UEP1_CTRL & ~( RB_UEP_R_TOG | MASK_UEP_R_RES ) ) | UEP_R_RES_ACK;
                  break;
                default :
                  errflag = 0xFF;                                 // ��֧�ֵĶ˵�
                  break;
              }
            }
            else
              errflag = 0xFF;
          }
            break;

          case USB_GET_INTERFACE :
            pEP0_DataBuf[0] = 0x00;
            if ( SetupReqLen > 1 )
              SetupReqLen = 1;
            break;

          case USB_GET_STATUS :
            pEP0_DataBuf[0] = 0x00;
            pEP0_DataBuf[1] = 0x00;
            if ( SetupReqLen > 2 )
              SetupReqLen = 2;
            break;

          default :
            errflag = 0xff;
            break;
        }
      }
      if ( errflag == 0xff )        // �����֧��
      {
//                  SetupReqCode = 0xFF;
        R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;    // STALL
      }
      else
      {
        if ( chtype & 0x80 )     // �ϴ�
        {
          len = ( SetupReqLen > DevEP0SIZE ) ?
              DevEP0SIZE : SetupReqLen;
          SetupReqLen -= len;
        }
        else
          len = 0;        // �´�
        R8_UEP0_T_LEN = len;
        R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;    // Ĭ�����ݰ���DATA1
      }

      R8_USB_INT_FG = RB_UIF_TRANSFER;
    }
  }
  else if ( intflag & RB_UIF_BUS_RST )
  {
    R8_USB_DEV_AD = 0;
    R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_USB_INT_FG = RB_UIF_BUS_RST;
  }
  else if ( intflag & RB_UIF_SUSPEND )
  {
    if ( R8_USB_MIS_ST & RB_UMS_SUSPEND )
    {
      USB_Ready = FALSE;
    }    // ����
    else
    {
      ;
    }               // ����
    R8_USB_INT_FG = RB_UIF_SUSPEND;
  }
  else
  {
    R8_USB_INT_FG = intflag;
  }
}

/*******************************************************************************
* Function Name  : DevEP1_OUT_Deal
* Description    : �˵�1���ݴ���(HID����-��ʹ��)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DevEP1_OUT_Deal( UINT8 l )
{
  UINT8 i;

  for ( i = 0; i < l; i++ )
  {
    pEP1_IN_DataBuf[i] = ~pEP1_OUT_DataBuf[i];
  }
  DevEP1_IN_Deal( l );
}

/*******************************************************************************
* Function Name  : DevEP2_OUT_Deal
* Description    : �˵�2���ݴ���(HID���-��ʹ��)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DevEP2_OUT_Deal( UINT8 l )
{
  UINT8 i;

  for ( i = 0; i < l; i++ )
  {
    pEP2_IN_DataBuf[i] = ~pEP2_OUT_DataBuf[i];
  }
  DevEP2_IN_Deal( l );
}

/*******************************************************************************
* Function Name  : DevEP3_OUT_Deal
* Description    : �˵�3���ݴ���(����λ��ͨ��)
* Input          : None
* Output         : None
* Return         : None
* Protocol       : address[0] �������ݳ���
*                  address[1] �������ͣ�E-���� S-�ɹ� C-�ı䰴���� X-�ı���ⰴ����
*                  address[2] ѡ������(address[1]ΪE��S����)
*                  address[3] ����1
*                  address[4] ����2
*                  ...
*******************************************************************************/
void DevEP3_OUT_Deal( UINT8 l )
{
  UINT8 i;
  UINT8 length = 0;

  if (pEP3_OUT_DataBuf[0] != l) { // ���ճ��ȴ���
    pEP3_IN_DataBuf[0] = 4;
    pEP3_IN_DataBuf[1] = 'E';
    pEP3_IN_DataBuf[3] = USB_ERR_LENGTH;
    DevEP3_IN_Deal( 4 );
    return;
  }

  switch (pEP3_OUT_DataBuf[1]) {
    case 'E':
      break;
    case 'S':
      break;
    case 'C':
      for (i = 0; i < l-3; i++) {
        CustomKey[pEP3_OUT_DataBuf[2]][i] = pEP3_OUT_DataBuf[i+3];
      }
      FLASH_Write_KeyArray( );
      pEP3_IN_DataBuf[0] = 2;
      pEP3_IN_DataBuf[1] = 'S';
      DevEP3_IN_Deal( 2 );    // ����-�ɹ�
      break;
    case 'X':
      for (i = 0; i < l-3; i++) {
        Extra_CustomKey[pEP3_OUT_DataBuf[2]][i] = pEP3_OUT_DataBuf[i+3];
      }
      FLASH_Write_KeyArray( );
      pEP3_IN_DataBuf[0] = 2;
      pEP3_IN_DataBuf[1] = 'S';
      DevEP3_IN_Deal( 2 );    // ����-�ɹ�
      break;
    default:
      pEP3_IN_DataBuf[0] = 4;
      pEP3_IN_DataBuf[1] = 'E';
      pEP3_IN_DataBuf[3] = USB_ERR_UNKNOWN;
      DevEP3_IN_Deal( 4 );
  }
}

/*******************************************************************************
* Function Name  : DevEP4_OUT_Deal
* Description    : �˵�4���ݴ���(����)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DevEP4_OUT_Deal( UINT8 l )
{
  UINT8 i;

  for ( i = 0; i < l; i++ )
  {
    pEP4_IN_DataBuf[i] = ~pEP4_OUT_DataBuf[i];
  }
  DevEP4_IN_Deal( l );
}

__INTERRUPT
__HIGH_CODE
void USB_IRQHandler( void )     //USB�жϷ������,ʹ�üĴ�����1
{
    USB_DevTransProcess();
}
