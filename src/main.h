#ifndef _MAIN_H_
#define _MAIN_H_ 

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#define BUFF 64

#define FIND_ELEMENT 1
#define PRINT_TO_SCREEN 2
#define TXT_FILE 4
#define BIN_FILE 8

typedef struct
{
	double* nums;		//Сама матрица чисел
	uint8_t hsize;		//Размер по горизонтали
	uint8_t vsize;		//Размер по вертикали
}
Matrix;

typedef struct
{
	int a;	//Верхний предел диапазона
	int b;	//Нижний предел диапазона
}
Range;

enum Errors { NOERROR, BAD_RANGE, BAD_SIZE, MALLOC_FAIL, BAD_FILE, BAD_SYMBOL };

int GetMaxOfMins(const Matrix* Matrix, double* Answer);
double GetMinFromArray(double* arr, int size);
double GetMaxFromArray(double* arr, int size);
int8_t FormulaSolution(Matrix* m);
int8_t RandomSolution(Matrix* m);
char SetMatrixSize(Matrix* Matrix, const char text[]);
void FillWithFormula(Matrix* Matrix);
void FillWithRandom(Matrix* Matrix, Range* Range);
void PrintMatrix(Matrix* Matrix, FILE* sink);
char SelectSource(void);
uint8_t SelectOutput(void);
int8_t ReadFromTextFile(Matrix* m, const char fname[]);
int8_t PrintIntoFile(Matrix* m, const char fname[]);
int8_t WriteIntoBinary(Matrix* m, const char fname[]);
void ReadFromText(Matrix* m, FILE* source);
int8_t ReadFromKeyboard(Matrix* m);
int8_t ReadFromBinary(Matrix* m, const char fname[]);


#endif // !_MAIN_H_

