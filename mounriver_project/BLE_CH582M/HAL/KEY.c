/********************************** (C) COPYRIGHT *******************************
* File Name          : KEY.c
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2021/12/7
* Description        : 
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include "CH58x_common.h"
#include "HAL.h"


/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/

static uint8 halKeySavedKeys;     /* ������������״̬�����ڲ�ѯ�Ƿ��м�ֵ�仯 */


/**************************************************************************************************
 *                                        FUNCTIONS - Local
 **************************************************************************************************/
static halKeyCBack_t pHalKeyProcessFunction;	    /* callback function */


/**************************************************************************************************
 * @fn      HALKeyInverseLED
 *
 * @brief   ����1���Ʒ�תLED״̬
 *
 * @param   none
 *
 * @return  None
 **************************************************************************************************/
void HALKeyInverseLED( uint8 keys )
{
  if (keys & HAL_KEY_SW_1) {
    HAL_INVERSE_LED1();
  }
}

/**************************************************************************************************
 * @fn      HAL_KeyInit
 *
 * @brief   Initilize Key Service
 *
 * @param   none
 *
 * @return  None
 **************************************************************************************************/
void HAL_KeyInit( void )
{
  /* Initialize previous key to 0 */
  halKeySavedKeys = 0;
  /* Initialize callback function */
  pHalKeyProcessFunction  = NULL;
  KEY1_DIR;
  KEY1_PD;
  HalKeyConfig( HALKeyInverseLED );
}

/**************************************************************************************************
 * @fn      HalKeyConfig
 *
 * @brief   Configure the Key serivce
 *
 * @param   interruptEnable - TRUE/FALSE, enable/disable interrupt
 *          cback - pointer to the CallBack function
 *
 * @return  None
 **************************************************************************************************/
void HalKeyConfig ( halKeyCBack_t cback)
{
  /* Register the callback fucntion */
  pHalKeyProcessFunction = cback;
	tmos_start_task( halTaskID, KEY_EVENT, HAL_KEY_POLLING_VALUE);    /* Kick off polling */
}

/**************************************************************************************************
 * @fn      HalKeyRead
 *
 * @brief   Read the current value of a key
 *
 * @param   None
 *
 * @return  keys - current keys status
 **************************************************************************************************/
uint8 HalKeyRead ( void )
{
  uint8 keys = 0;

  if (HAL_PUSH_BUTTON1()){ //������1
    keys |= HAL_KEY_SW_1;
  }
  if (HAL_PUSH_BUTTON2()){ //������1
    keys |= HAL_KEY_SW_2;
  }
  if (HAL_PUSH_BUTTON3()){ //������1
    keys |= HAL_KEY_SW_3;
  }
  if (HAL_PUSH_BUTTON4()){ //������1
    keys |= HAL_KEY_SW_4;
  }
  return keys;
}


/**************************************************************************************************
 * @fn      HAL_KeyPoll
 *
 * @brief   Called by hal_driver to poll the keys
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HAL_KeyPoll (void)
{
  uint8 keys = 0;
  if( HAL_PUSH_BUTTON1() ){
    keys |= HAL_KEY_SW_1;
  }
  if( HAL_PUSH_BUTTON2() ){
    keys |= HAL_KEY_SW_2;
  }
  if( HAL_PUSH_BUTTON3() ){
    keys |= HAL_KEY_SW_3;
  }
  if( HAL_PUSH_BUTTON4() ){
    keys |= HAL_KEY_SW_4;
  }
	if(keys == halKeySavedKeys){         /* Exit - since no keys have changed */
		return;
	}
	halKeySavedKeys = keys;	       /* Store the current keys for comparation next time */
  /* Invoke Callback if new keys were depressed */
  if (keys && (pHalKeyProcessFunction)){
    (pHalKeyProcessFunction) (keys);
  }
}


/******************************** endfile @ key ******************************/
