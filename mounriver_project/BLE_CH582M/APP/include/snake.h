/********************************** (C) COPYRIGHT *******************************
* File Name          : snake.h
* Author             : ChnMasterOG
* Version            : V1.0
* Date               : 2022/1/28
* Description        :
*******************************************************************************/

#ifndef SNAKE_H
  #define SNAKE_H

  #define MAX_SNAKE_LENGTH              30    // ��߳�
  #define KEYBOARD_SNAKE_ROWCNT         6     // �������� - �������̲������޸Ĵ˴�
  #define KEYBOARD_SNAKE_COLUMCNT       14    // �������� - �������̲������޸Ĵ˴�

  typedef enum {
    DirUp = 1,
    DirDown,
    DirLeft,
    DirRight,
  }SnakeDir;

  typedef struct {
    uint8_t PosX;
    uint8_t PosY;
  }SnakePos;

  SnakeDir BodyDir[MAX_SNAKE_LENGTH];

  void Snake_Init( void );
  void MoveSnake( void );

#endif

