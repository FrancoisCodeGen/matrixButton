#ifndef MATRIX_BUTTON_H__
#define MATRIX_BUTTON_H__

#define SIZE_ROW_PINS       4
#define SIZE_COLUMN_PINS    4
#define NB_BUTTONS          SIZE_ROW_PINS * SIZE_COLUMN_PINS
//pull up
#define APPUYE              LOW
#define RELACHE             HIGH

void joypiButtonMatrix(int edgeType, void (*function)(int));

#endif