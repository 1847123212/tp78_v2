/********************************** (C) COPYRIGHT *******************************
 * File Name          : KEYBOARD.c
 * Author             : ChnMasterOG
 * Version            : V1.3
 * Date               : 2022/1/17
 * Description        : ��е��������Դ�ļ�
 *******************************************************************************/

#include "HAL.h"

const uint32_t Row_Pin[] = {GPIO_Pin_7, GPIO_Pin_6, GPIO_Pin_5, GPIO_Pin_4, GPIO_Pin_3, GPIO_Pin_2};   //row 6 - �������̲������޸Ĵ˴�
const uint32_t Colum_Pin[] = {GPIO_Pin_9, GPIO_Pin_7, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_4, GPIO_Pin_5,
                              GPIO_Pin_6, GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_15, GPIO_Pin_14};   //colum 14 - �������̲������޸Ĵ˴�
//row*colum = 6*14 = 84
const uint8_t KeyArrary[sizeof(Colum_Pin)/sizeof(uint32_t)][sizeof(Row_Pin)/sizeof(uint32_t)] = {
        { KEY_ESCAPE,   KEY_GraveAccent,    KEY_TAB,        KEY_CapsLock,   KEY_LeftShift,  KEY_LeftCTRL }, //1
        { KEY_F1,       KEY_1,              KEY_Q,          KEY_A,          KEY_None,       KEY_LeftGUI  }, //2
        { KEY_F2,       KEY_2,              KEY_W,          KEY_S,          KEY_Z,          KEY_LeftAlt  }, //3
        { KEY_F3,       KEY_3,              KEY_E,          KEY_D,          KEY_X,          KEY_MouseL   }, //4
        { KEY_F4,       KEY_4,              KEY_R,          KEY_F,          KEY_C,          KEY_MouseM   }, //5
        { KEY_F5,       KEY_5,              KEY_T,          KEY_G,          KEY_V,          KEY_MouseR   }, //6
        { KEY_F6,       KEY_6,              KEY_Y,          KEY_H,          KEY_B,          KEY_SPACEBAR }, //7
        { KEY_F7,       KEY_7,              KEY_U,          KEY_J,          KEY_N,          KEY_None     }, //8
        { KEY_F8,       KEY_8,              KEY_I,          KEY_K,          KEY_M,          KEY_None     }, //9
        { KEY_F9,       KEY_9,              KEY_O,          KEY_L,          KEY_Comma,      KEY_None     }, //10
        { KEY_F10,      KEY_0,              KEY_P,          KEY_Semicolon,  KEY_FullStop,   KEY_RightAlt }, //11
        { KEY_F11,      KEY_Subtraction,    KEY_LSbrackets, KEY_Quotation,  KEY_Slash,      KEY_RightGUI }, //12
        { KEY_F12,      KEY_Equal,          KEY_RSbrackets, KEY_None,       KEY_None,       KEY_Fn       }, //13
        { KEY_Delete,   KEY_BACKSPACE,      KEY_NonUS_WS,   KEY_ENTER,      KEY_RightShift, KEY_RightCTRL}, //14
};  // Ĭ�ϼ��̲��� - �������̲������޸Ĵ˴�
uint8_t CustomKey[sizeof(Colum_Pin)/sizeof(uint32_t)][sizeof(Row_Pin)/sizeof(uint32_t)];  //�Զ��尴����
uint8_t Extra_CustomKey[sizeof(Colum_Pin)/sizeof(uint32_t)][sizeof(Row_Pin)/sizeof(uint32_t)];   //�Զ�����ⰴ����
uint8_t KeyMatrix[sizeof(Colum_Pin)/sizeof(uint32_t)][sizeof(Row_Pin)/sizeof(uint32_t)] = { 0 };  //��������-��ǰ��º�δ����
uint8_t Extra_KeyMatrix[sizeof(Colum_Pin)/sizeof(uint32_t)][sizeof(Row_Pin)/sizeof(uint32_t)] = { 0 };  //����㰴������-��ǰ��º�δ����
uint32_t Row_Pin_ALL = 0, Colum_Pin_ALL = 0;

uint8_t KEYBOARD_data_index = 2,
        KEYBOARD_data_ready = 0,
        KEYBOARD_mouse_ready = 0,
        LED_Change_flag = 0,
        Fn_state = 0;
Keyboardstate* const Keyboarddat = (Keyboardstate*)HIDKey;

/*******************************************************************************
* Function Name  : FLASH_Read_KeyArray
* Description    : ��Flash��ȡ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_Read_KeyArray( void )
{
  EEPROM_READ( 0, CustomKey, sizeof(Colum_Pin)/sizeof(uint32_t)*sizeof(Row_Pin)/sizeof(uint32_t) );
}

/*******************************************************************************
* Function Name  : FLASH_Write_KeyArray
* Description    : ����������д��Flash
* Input          : None
* Output         : None
* Return         : ����ɹ�����0
*******************************************************************************/
UINT8 FLASH_Write_KeyArray( void )
{
  UINT8 s;
  s = EEPROM_WRITE( 0, CustomKey, sizeof(Colum_Pin)/sizeof(uint32_t)*sizeof(Row_Pin)/sizeof(uint32_t) );
  return s;
}

/*******************************************************************************
* Function Name  : KEYBOARD_ResetKey
* Description    : ���ð�������ΪĬ�ϲ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void KEYBOARD_ResetKey( void )
{
  memcpy(CustomKey, KeyArrary, sizeof(Colum_Pin)/sizeof(uint32_t)*sizeof(Row_Pin)/sizeof(uint32_t));
  FLASH_Write_KeyArray( );
}

/*******************************************************************************
* Function Name  : KEYBOARD_ChangeKey
* Description    : ���̽���2������
* Input          : dst_key - Ŀ���, src_key - ԭʼ��
* Output         : None
* Return         : None
*******************************************************************************/
void KEYBOARD_ChangeKey( uint8_t dst_key, uint8_t src_key )
{
  uint8_t i = sizeof(Colum_Pin)/sizeof(uint32_t)*sizeof(Row_Pin)/sizeof(uint32_t);
  uint8_t* memaddr = &CustomKey[0][0];
  while (i) {
    --i;
    /* ����src����dst�� */
    if ( *(memaddr + i) == src_key ) *(memaddr + i) = dst_key;
    else if ( *(memaddr + i) == dst_key ) *(memaddr + i) = src_key;
  }
  FLASH_Write_KeyArray( );
}

/*******************************************************************************
* Function Name  : KEYBOARD_Custom_Function
* Description    : ���̶��ƻ�����(���Fn����������ع���)
* Input          : None
* Output         : None
* Return         : �������Ҫ���Ͱ�����Ϣ�򷵻�0
*******************************************************************************/
UINT8 KEYBOARD_Custom_Function( void )
{
  static uint8_t Fn_Mode = 0;
  static uint8_t last_state = 0;
  static uint8_t Reset_cnt = 0, ChangeKey_cnt = 0;
  static uint8_t src_key = 0, dst_key = 0;
  if ( Fn_state != 0 && Keyboarddat->Key2 == KEY_None ) { // ����Fn+1��������ֻ��Fn��
    last_state = 1;
    if ( Keyboarddat->Key1 == KEY_None ) { // ���̲��ָ�λ
      if ( Fn_Mode == Fn_Mode_None ) {
        Fn_Mode = Fn_Mode_Reset;
        ChangeKey_cnt = 0;
      }
    } else if ( Keyboarddat->Key1 == KEY_C && Fn_Mode != Fn_Mode_ChangeKey ) { // ���øļ� - �Ȱ�Fn+C
      Fn_Mode = Fn_Mode_ChangeKey;
      Reset_cnt = 0;
    } else if ( Fn_Mode == Fn_Mode_ChangeKey ) {  // ���øļ� - ��Fn+src��Fn+dst
      if ( ChangeKey_cnt == 1 ) src_key = Keyboarddat->Key1;
      else if ( ChangeKey_cnt == 2 ) dst_key = Keyboarddat->Key1;
    } else if ( Keyboarddat->Key1 == KEY_Delete && Fn_Mode != Fn_Mode_PaintedEgg ) { // �ʵ�ģʽ
      Fn_Mode = Fn_Mode_PaintedEgg;
    } else if ( Keyboarddat->Key1 == KEY_1 && Fn_Mode != Fn_Mode_LED_Style1 ) { // ������ģʽ1
      Fn_Mode = Fn_Mode_LED_Style1;
    } else if ( Keyboarddat->Key1 == KEY_2 && Fn_Mode != Fn_Mode_LED_Style2 ) { // ������ģʽ2
      Fn_Mode = Fn_Mode_LED_Style2;
    } else if ( Keyboarddat->Key1 == KEY_3 && Fn_Mode != Fn_Mode_LED_Style3 ) { // ������ģʽ3
      Fn_Mode = Fn_Mode_LED_Style3;
    } else {  // ��ʶ���ָ��
      Fn_Mode = Fn_Mode_None;
      Reset_cnt = ChangeKey_cnt = 0;
    }
    return 0;
  } else if ( last_state == 1 && Fn_state == 0 ) {  // �ɿ�Fnִ��ָ��
    last_state = 0;
    switch ( Fn_Mode ) {
      case Fn_Mode_Reset: // ����5��Fn���ð���
        ++Reset_cnt;
        if ( Reset_cnt == 5 ) {
          Reset_cnt = 0;
          Fn_Mode = Fn_Mode_None;
          OLED_PRINT("Reset Keyboard OK!");
          KEYBOARD_ResetKey( );
        }
        break;
      case Fn_Mode_ChangeKey:
        ++ChangeKey_cnt;
        if ( ChangeKey_cnt == 3 ) {
          ChangeKey_cnt = 0;
          Fn_Mode = Fn_Mode_None;
          OLED_PRINT("Change Key: %d<->%d", src_key, dst_key);
          KEYBOARD_ChangeKey( dst_key, src_key );
        }
        break;
      case Fn_Mode_PaintedEgg:
        OLED_DrawBMP(0, 0, 128, 4, (uint8_t*)PaintedEgg_Bmp);
        break;
      case Fn_Mode_LED_Style1:
        LED_Change_flag = 1;
        OLED_PRINT("LED Off");
        led_style_func = WS2812_Style_Off;  // Fn+1 - �رձ���
        break;
      case Fn_Mode_LED_Style2:
        LED_Change_flag = 1;
        OLED_PRINT("LED Style 1");
        led_style_func = WS2812_Style_Breath;  // Fn+2 - ����ʹ�ú�����ģʽ
        break;
      case Fn_Mode_LED_Style3:
        LED_Change_flag = 1;
        OLED_PRINT("LED Style 2");
        led_style_func = WS2812_Style_Waterful;  // Fn+3 - ����ʹ����ˮ��ģʽ
        break;
      default:  // δʶ���ָ��
        ;
    }
    return 0;
  }
  return 1;
}

/*******************************************************************************
* Function Name  : KEYBOARD_Init
* Description    : ���̳�ʼ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void KEYBOARD_Init( void )
{
    uint8_t i;
    FLASH_Read_KeyArray( );   // Flash���밴��
    CustomKey[12][5] = KEY_Fn;   // ��֤�ϵ�Fn���ڶ�Ӧλ�� - �������̲������޸Ĵ˴�
    for (i = 0; i < sizeof(Row_Pin)/sizeof(uint32_t); i++) {
        Row_Pin_ALL |= Row_Pin[i];
    }
    Row_GPIO_(SetBits)( Row_Pin_ALL );
    Row_GPIO_(ModeCfg)( Row_Pin_ALL, GPIO_ModeOut_PP_20mA );
    Row_GPIO_(SetBits)( Row_Pin_ALL & (~Row_Pin[0]) );

    for (i = 0; i < sizeof(Colum_Pin)/sizeof(uint32_t); i++) {
        Colum_Pin_ALL |= Colum_Pin[i];
    }
    Colum_GPIO_(SetBits)( Colum_Pin_ALL );
    Colum_GPIO_(ModeCfg)( Colum_Pin_ALL, GPIO_ModeIN_PU );
}

/*******************************************************************************
* Function Name  : KEYBOARD_detection
* Description    : ���̼�ⰴ����Ϣ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void KEYBOARD_detection( void )
{
    static uint8_t current_row = 0;
    uint8_t current_colum, key_idx;
    for (current_colum = 0; current_colum < sizeof(Colum_Pin)/sizeof(uint32_t); current_colum++) {    // ��ѯ��һ�иı�
        if (KeyMatrix[current_colum][current_row] == 0 && Colum_GPIO_(ReadPortPin)( Colum_Pin[current_colum] ) == 0) {  // ����
            if (KEYBOARD_data_index >= 8 && CustomKey[current_colum][current_row] < KEY_LeftCTRL) continue;    // ����6����ͨ��������
            KeyMatrix[current_colum][current_row] = 1;  // ����״̬��ɰ���
            KEYBOARD_data_ready = 1; // �����¼�
            if (CustomKey[current_colum][current_row] == KEY_Fn) {  // ���ܼ�
                Fn_state = 1;
            } else if (CustomKey[current_colum][current_row] >= KEY_MouseL) {    // ������
                PS2dat->data[0] |= 1 << CustomKey[current_colum][current_row] - KEY_MouseL;
                KEYBOARD_mouse_ready = 1;
            } else if (CustomKey[current_colum][current_row] >= KEY_LeftCTRL) {    // �����
                Keyboarddat->data[0] |= 1 << (CustomKey[current_colum][current_row] - KEY_LeftCTRL);
            } else {
                Keyboarddat->data[KEYBOARD_data_index++] = CustomKey[current_colum][current_row];
            }
        } else if (KeyMatrix[current_colum][current_row] != 0 && Colum_GPIO_(ReadPortPin)( Colum_Pin[current_colum] ) != 0) {   // ����
            KeyMatrix[current_colum][current_row] = 0;
            KEYBOARD_data_ready = 1; // �����¼�
            if (CustomKey[current_colum][current_row] == KEY_Fn) {  // ���ܼ�
                Fn_state = 0;
            } else if (CustomKey[current_colum][current_row] >= KEY_MouseL) {    // ������
                PS2dat->data[0] &= ~(1 << CustomKey[current_colum][current_row] - KEY_MouseL);
                KEYBOARD_mouse_ready = 1;
            } else if (CustomKey[current_colum][current_row] >= KEY_LeftCTRL) {    // �����
                Keyboarddat->data[0] &= ~(1 << (CustomKey[current_colum][current_row] - KEY_LeftCTRL));
            } else {
                for (key_idx = 2; key_idx < 8; key_idx++) {
                    if (Keyboarddat->data[key_idx] == CustomKey[current_colum][current_row]) {
                        memcpy(&Keyboarddat->data[key_idx], &Keyboarddat->data[key_idx] + 1, 7 - key_idx);
                        Keyboarddat->Key6 = 0;
                    }
                }
                KEYBOARD_data_index--;
            }
        }
    }
    Row_GPIO_(SetBits)( Row_Pin[current_row++] );
    if (current_row >= sizeof(Row_Pin)/sizeof(uint32_t)) {
        current_row = 0;
    }
    Row_GPIO_(ResetBits)( Row_Pin[current_row] );
}


