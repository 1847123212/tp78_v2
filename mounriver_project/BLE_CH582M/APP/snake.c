/********************************** (C) COPYRIGHT *******************************
* File Name          : snake.c
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2022/1/27
* Description        : ̰������Ϸ
*******************************************************************************/

#include <stdlib.h>
#include "HAL.h"
#include "snake.h"

SnakeDir BodyDir[MAX_SNAKE_LENGTH];             // ���峯��
static SnakePos BodyPos[MAX_SNAKE_LENGTH];      // ����λ��(LED���)
static SnakePos FoodPos;                        // ʳ��λ��(LED���)
static uint8_t SnakeLength;                     // �߳�

static void ProduceFood( void );
static BOOL ComparePos(SnakePos pos1, SnakePos pos2);
static SnakePos PosDirToPos(SnakePos target, SnakeDir target_dir);
static BOOL CheckOverlap( SnakePos target );
static void ShowSnake( void );

const uint8_t SnakeMatrix[KEYBOARD_SNAKE_COLUMCNT][KEYBOARD_SNAKE_ROWCNT] = {
  { 61,   47,   33,   20,   8,    0   }, //1
  { 62,   48,   34,   21,   9,    1   }, //2
  { 63,   49,   35,   22,   9,    2   }, //3
  { 64,   50,   36,   23,   10,   2   }, //4
  { 65,   51,   37,   24,   11,   3   }, //5
  { 66,   52,   38,   25,   12,   3   }, //6
  { 67,   53,   39,   26,   13,   3   }, //7
  { 68,   54,   40,   27,   14,   3   }, //8
  { 69,   55,   41,   28,   15,   3   }, //9
  { 70,   56,   42,   29,   16,   4   }, //10
  { 71,   57,   43,   30,   17,   4   }, //11
  { 72,   58,   44,   31,   18,   5   }, //12
  { 73,   59,   45,   31,   18,   6   }, //13
  { 74,   60,   46,   32,   19,   7   }, //14
};

/*******************************************************************************
* Function Name  : Snake_Init
* Description    : ��ʼ��̰����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Snake_Init( void )
{
	uint16_t i;
  srand(BAT_adcVal);  // �õ�ص�����Ϊ�������
	for ( i = 0; i < MAX_SNAKE_LENGTH; i++ ) {
	  BodyPos[i].PosX = 0;
	  BodyPos[i].PosY = 0;
		BodyDir[i] = DirUp;
	}
	SnakeLength = 2;
	BodyPos[0].PosX = 3; BodyPos[0].PosY = 1; BodyDir[0] = DirRight; // ��ʼ��ͷλ��/����
	BodyPos[1].PosX = 2; BodyPos[1].PosY = 1; BodyDir[1] = DirRight; // ��ʼ������λ��/����
	ProduceFood();  // ����ʳ��
}

/*******************************************************************************
* Function Name  : MoveSnake
* Description    : ���ƶ���һ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MoveSnake( void )
{
  uint16_t i;
  SnakePos tail;
  tail.PosX = BodyPos[SnakeLength-1].PosX;
  tail.PosY = BodyPos[SnakeLength-1].PosY;
  // �������ǰһ��״̬
  for (i = SnakeLength-1; i > 0; i--) {
    BodyPos[i].PosX = BodyPos[i-1].PosX;
    BodyPos[i].PosY = BodyPos[i-1].PosY;
  }
  // ���ƶ���ͷ
  BodyPos[0] = PosDirToPos(BodyPos[0], BodyDir[0]);
  if ( ComparePos( BodyPos[0], FoodPos ) ) { // �߳�ʳ��
    if ( SnakeLength < MAX_SNAKE_LENGTH ) { // �߲�������󳤶�
      ++SnakeLength;
      BodyPos[SnakeLength-1].PosX = tail.PosX;
      BodyPos[SnakeLength-1].PosY = tail.PosY;
      ProduceFood();
    }
  } else {
    // �����ͷײ���������¿�ʼ
    if ( CheckOverlap(BodyPos[0]) ) {
      Snake_Init( );
    }
  }
  ShowSnake( );
}

/*******************************************************************************
* Function Name  : CheckOverlap
* Description    : ���Ŀ���Ƿ�������ص�
* Input          : ����������
* Output         : None
* Return         : 1=TRUE or 0=FALSE
*******************************************************************************/
static BOOL CheckOverlap( SnakePos target )
{
	uint8_t i;
	for ( i = 1; i < SnakeLength; i++ ) {
	  if ( ComparePos( target, BodyPos[i] ) ) {
	    return 1;
	  }
	}
	return 0;
}

/*******************************************************************************
* Function Name  : PosDirToPos
* Description    : ��Ŀ��λ�úͶ�Ӧ�������һ��λ�÷���
* Input          : ����������ʹ����Ŀ��ĳ���
* Output         : None
* Return         : ��һ��λ��
*******************************************************************************/
static SnakePos PosDirToPos(SnakePos target, SnakeDir target_dir)
{
  switch (target_dir) {
    case DirUp:
      if ( target.PosY == 0 ) target.PosY = KEYBOARD_SNAKE_ROWCNT-1;  // �������һ��
      else --target.PosY;
      break;
    case DirDown:
      if ( target.PosY == KEYBOARD_SNAKE_ROWCNT-1 ) target.PosY = 0;  // ���ص�һ��
      else ++target.PosY;
      break;
    case DirLeft:
      if ( target.PosX == 0 ) target.PosX = KEYBOARD_SNAKE_COLUMCNT-1;  // �������һ��
      else {
        while ( SnakeMatrix[target.PosX-1][target.PosY] == SnakeMatrix[target.PosX][target.PosY] ) --target.PosX;
        --target.PosX;  // ��һ�к͵ڶ��е�LEDһ����һ�� �ʴ����
      }
      break;
    case DirRight:
      if ( target.PosX == KEYBOARD_SNAKE_COLUMCNT-1 ) target.PosX = 0;  // ���ص�һ��
      else {
        while ( SnakeMatrix[target.PosX+1][target.PosY] == SnakeMatrix[target.PosX][target.PosY] ) ++target.PosX;
        ++target.PosX;  // ���һ�к͵����ڶ��е�LEDһ����һ�� �ʴ����
      }
      break;
  }
  return target;
}

/*******************************************************************************
* Function Name  : ProduceFood
* Description    : ����һ���µ�ʳ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void ProduceFood( void )
{
	do {
	  FoodPos.PosX = (int)(rand() % KEYBOARD_SNAKE_COLUMCNT);
	  FoodPos.PosY = (int)(rand() % KEYBOARD_SNAKE_ROWCNT);
	}	while( CheckOverlap( FoodPos ) || ComparePos(FoodPos, BodyPos[0]) );
}

/*******************************************************************************
* Function Name  : ComparePos
* Description    : �Ƚ�����λ���Ƿ���ͬ
* Input          : ����λ����Ϣ
* Output         : None
* Return         : 1-TRUE or 0-FALSE
*******************************************************************************/
static BOOL ComparePos(SnakePos pos1, SnakePos pos2)
{
  if ( SnakeMatrix[pos1.PosX][pos1.PosY] == SnakeMatrix[pos2.PosX][pos2.PosY] ) return 1;
  else return 0;
}

/*******************************************************************************
* Function Name  : ShowSnake
* Description    : �ڱ�������ʾ��ͼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void ShowSnake( void )
{
  uint8_t i;
  tmos_memset(LED_BYTE_Buffer, 0, LED_Number*3);
  // ʳ����ɫ - ��
  LED_BYTE_Buffer[SnakeMatrix[FoodPos.PosX][FoodPos.PosY]][RED_INDEX] = 0x7F;
  // ��ͷ��ɫ - ��
  LED_BYTE_Buffer[SnakeMatrix[BodyPos[0].PosX][BodyPos[0].PosY]][BLUE_INDEX] = 0x7F;
  // ������ɫ - ��
  for ( i = 1; i < SnakeLength; i++ ) {
    LED_BYTE_Buffer[SnakeMatrix[BodyPos[i].PosX][BodyPos[i].PosY]][GREEN_INDEX] = 0x7F;
  }
}

/*********************************************END OF FILE**********************/
