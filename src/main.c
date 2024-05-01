/*Найти максимальный среди минимальных значений по строке.*/
#include "main.h"

int main(void)
{
	int errCode;
	double MaxOfMin;

	Matrix m1;						//Объявление матрицы
	char fname[BUFF];

	//Блок ввода
	switch (SelectSource())			//Выбор способа заполнения
	{
	default:						//Завершение работы по желанию пользователя
		puts("Завершение работы");
		return NOERROR;
	case 'r':						//Заполнение случайными числами
	case 'R':
		if (errCode = RandomSolution(&m1))
			return errCode;				//Завершение работы из-за неправильно заданного размера или диапазона
		break;
	case 'a':
	case 'A':						//Заполение согласно формуле
		if (errCode = FormulaSolution(&m1))
			return errCode;				//Завершение работы из-за неправильно заданного размера
		break;
	case 'f':						//Заполнение из бинарного файла
	case 'F':
		printf("Введите название файла для чтения: ");
		scanf("%32s", fname);
		errCode = ReadFromBinary(&m1, fname);
		switch (errCode)
		{
		case BAD_FILE:
			printf("Не удалось открыть файл %s! Завершение работы\n", fname);
			return BAD_FILE;
		case MALLOC_FAIL:
			puts("Не удалось выделить память! Завершение работы");
			return MALLOC_FAIL;
		default:
			break;
		}
		break;
	case 't':						//Заполнение из текстового файла
	case 'T':
		printf("Введите название файла для чтения: ");
		scanf("%32s", fname);
		errCode = ReadFromTextFile(&m1, fname);
		switch (errCode)
		{
		case BAD_SYMBOL:
			puts("Файл содержит лишние символы! Завершение работы!");
			free(m1.nums);
			return BAD_SYMBOL;
		case BAD_FILE:
			printf("Не удалось открыть файл %s! Завершение работы\n", fname);
			return BAD_FILE;
		case MALLOC_FAIL:
			puts("Не удалось выделить память! Завершение работы");
			return MALLOC_FAIL;
		default:
			break;
		}
		break;
	case 'k':						//Заполнение с клавиатуры
	case 'K':
		if (ReadFromKeyboard(&m1))
		{
			puts("Введены лишние символы! Завершение работы");
			free(m1.nums);
			return BAD_SYMBOL;
		}
		break;
	}

	//Блок вывода
	char outputFlag = SelectOutput();

	if (!outputFlag)	//Завершение работы при неправильном флаге
	{
		puts("Завершение работы");
		free(m1.nums);
		return NOERROR;
	}

	if (outputFlag & FIND_ELEMENT)	//Вывод произведения при соответствующем флаге
	{
		errCode = GetMaxOfMins(&m1, &MaxOfMin);
		if (errCode)
		{
			free(m1.nums);
			return errCode;
		}
		printf("Максимальный элемент среди минимальных элементов каждой строки равен: %.2lf\n", MaxOfMin);
	}
		
	if (outputFlag & PRINT_TO_SCREEN)	//Вывод на экран при соответствующем флаге
	{
		printf("\nПолученная матрица:\n");
		PrintMatrix(&m1, stdout);
	}

	if (outputFlag & TXT_FILE)	//Вывод в текстовый файл при соответствующем флаге
	{
		printf("Введите название файла для записи: ");
		scanf("%32s", fname);
		if (PrintIntoFile(&m1, fname))
		{
			printf("Не удалось открыть файл %s! Завершение работы\n", fname);
			free(m1.nums);
			return BAD_FILE;
		}
	}

	if (outputFlag & BIN_FILE)	//Вывод в бинарный файл при соответствующем флаге
	{
		printf("Введите название файла для записи: ");
		scanf("%32s", fname);
		if (WriteIntoBinary(&m1, fname))
		{
			printf("Не удалось открыть файл %s! Завершение работы\n", fname);
			free(m1.nums);
			return BAD_FILE;
		}
	}
		
	//Корректное завершение работы
	free(m1.nums);
	return NOERROR;
}

//Заполнение матрицы m с клавиатуры
int8_t ReadFromKeyboard(Matrix* m)
{
	char buff[BUFF];				//Объявление буфера текста
	do
	{
		puts("Введите размер матрицы, количество строк и столбцов:");
		fgets(buff, BUFF - 1, stdin);	//Чтение размеров матрицы
	} while (SetMatrixSize(m, buff));		//Ввод размера будет прододжаться, пока не будет введёно корректное значение

	printf("Поочерёдно введите %d значений\n", (m->hsize * m->vsize));
	ReadFromText(m, stdin);			//Чтение

	//Проверка на лишний ввод
	char g;
	while (g = getchar())
	{
		if (g == '\n')				//Если после чисел был нажат Enter, проверку можно прекратить
			return NOERROR;
		if (g != ' ' && g != '\t')	//Если после чисел был мусор, можно прекратить проверку и отправить флаг ошибки
			return BAD_SYMBOL;
		//Если был встречен пробел, проверка продолжается по новой
	}
}

//Чтение из бинарного файла под названием fname в матрицу m
int8_t ReadFromBinary(Matrix* m, const char fname[])
{
	FILE* file = fopen(fname, "rb");		//Открытие файла; проверка, открылся ли он
	if (file == NULL)
		return BAD_FILE;				//Возврат флага при неудачной попытке открыть файл

	//Перемотка в конец файла, получение его размера, возврат в начало
	fseek(file, SEEK_END, SEEK_SET);
	int ByteSizeOfArray = ftell(file) - 1;
	rewind(file);

	//Выделение памяти под него, проверка
	m->nums = (double*)malloc(sizeof(double) * ByteSizeOfArray);
	if(!m->nums)
	{
		fclose(file);
		return MALLOC_FAIL;
	}

	//Чтение размеров матрицы из файла, сохранение
	double hsize_tmp;	
	fread(&hsize_tmp, sizeof(double), 1, file);
	m->hsize = hsize_tmp;
	m->vsize = ByteSizeOfArray / m->hsize;

	//Чтение элементов матрицы
	fread(m->nums, sizeof(double), m->hsize * m->vsize, file);

	fclose(file);
	return NOERROR;
}

//Запись матрицы m в бинарный файл под названием fname 
int8_t WriteIntoBinary(Matrix* m, const char fname[])
{
	FILE* out = fopen(fname, "wb");	//Открытие файла; проверка, открылся ли он
	if (out == NULL)
		return BAD_FILE;			//Возврат флага при неудачной попытке открыть файл

	double TMP_hsize = (double)m->hsize;

	//Запись размера строки
	fwrite(&TMP_hsize, sizeof(double), 1, out);
	fwrite(m->nums, sizeof(double), m->hsize * m->vsize, out);

	fclose(out);
	return NOERROR;
}

//Вывод матрицы m в текстовый файл под названием fname 
int8_t PrintIntoFile(Matrix* m, const char fname[])
{
	FILE* out = fopen(fname, "wt");	//Открытие файла; проверка, открылся ли он
	if (out == NULL)
		return BAD_FILE;			//Возврат флага при неудачной попытке открыть файл

	fprintf(out, "%d %d\n", m->hsize, m->vsize);	//Вывод размера матрицы в файл
	PrintMatrix(m, out);						//Вывод самой матрицы в файл
	fclose(out);
	return NOERROR;
}

//Чтение элементов в матрицу m из файла source
void ReadFromText(Matrix* m, FILE* source)
{
	for (int i = 0; i < m->vsize; ++i)				//Перебор строк
		for (int j = 0; j < m->hsize; ++j)			//Перебор столбцов в строках
			fscanf(source, "%lf", &m->nums[i * m->hsize + j]);	//Чтение каждого значения
}

//Чтение в матрицу m из некоторого текстового файла под названием fname 
int8_t ReadFromTextFile(Matrix* m, const char fname[])
{
	char buff[BUFF];
	int errCode;
	FILE* source = fopen(fname, "rt");
	if (!source)
		return BAD_FILE; //Возврат флага, если не открылся файл

	fgets(buff, BUFF - 1, source);	//Чтение заголовка файла с размерами матрицы
	
	errCode = SetMatrixSize(m, buff);

	switch (errCode)
	{
	case BAD_SIZE:
		puts("Неправильно указан размер матрицы. Завершение работы");
		return BAD_SIZE;
	case MALLOC_FAIL:
		puts("Не удалось выделить память! Завершение работы.");
		return MALLOC_FAIL;
	default:
		break;
	}
	ReadFromText(m, source);			//Чтение

	//Проверка на лишний ввод
	char g;
	while (g = fgetc(source))
	{
		if (g == EOF)				//Если после чисел был встречен конец файла, проверку можно прекратить
		{
			fclose(source);
			return NOERROR;
		}
		if (g != ' ' && (g != '\t' && g != '\n'))	//Если после чисел был мусор, можно прекратить проверку и отправить флаг ошибки
		{
			fclose(source);
			return BAD_SYMBOL;
		}
		//Если был встречен пробел, табуляция, или новая строка, проверка продолжается по новой
	}
}

//Заполнение матрицы m по формуле
int8_t FormulaSolution(Matrix* m)
{
	int errCode;
	char buff[BUFF];//Объявление буфера текста
	puts("Введите размер матрицы, количество строк и столбцов:");
	fgets(buff, BUFF - 1, stdin);

	errCode = SetMatrixSize(m, buff);

	switch (errCode)
	{
	case BAD_SIZE:
		puts("Неправильно указан размер матрицы. Завершение работы");
		return BAD_SIZE;
	case MALLOC_FAIL:
		puts("Не удалось выделить память! Завершение работы.");
		return MALLOC_FAIL;
	default:
		FillWithFormula(m);
		return NOERROR;
	}
}

//Заполнение матрицы m случайными числами
int8_t RandomSolution(Matrix* m)
{
	int errCode;
	Range r;		//Объявление диапазона чисел
	char buff[BUFF];//Объявление буфера текста
	puts("Введите размер матрицы, количество строк и столбцов:");
	fgets(buff, BUFF - 1, stdin);
	errCode = SetMatrixSize(m, buff);

	switch (errCode)
	{
	case BAD_SIZE:
		puts("Неправильно указан размер матрицы. Завершение работы");
		return BAD_SIZE;
	case MALLOC_FAIL:
		puts("Не удалось выделить память! Завершение работы.");
		return MALLOC_FAIL;
	default:
		break;
	}

	puts("Введите диапазон, из которого будет заполнятся матрица:");
	if (scanf("%d %d", &r.a, &r.b) == 2)
	{
		FillWithRandom(m, &r);
		return NOERROR;
	}
	else
	{
		puts("Неправильно указан диапазон. Завершение работы");
		return BAD_RANGE;
	}
}

//Устанавливает размер матрицы m из текстового источника fname
char SetMatrixSize(Matrix* Matrix, const char text[])
{
	if (sscanf(text, "%d %d", &Matrix->hsize, &Matrix->vsize) != 2)
		return BAD_SIZE;
	if (Matrix->hsize <= 0 || Matrix->vsize <= 0)
		return BAD_SIZE;

	Matrix->nums = (double*) malloc (sizeof(double) * Matrix->vsize * Matrix->hsize);
	if (Matrix->nums == NULL)
		return MALLOC_FAIL;
	return NOERROR;
}

//Функция выбора способа заполнения
char SelectSource(void)
{
	printf("Выберите способ наполнения массива\n%s%s%s%s%s%s",
		"r -- для заполнения случайными числами\n",
		"a -- для автозаполнения согласно формуле\n",
		"f -- для чтения массива из бинарного файла\n",
		"k -- для ручного наполнения\n",
		"t -- для чтения массива из текстового файла\n",
		"любую другую клавишу для завершения работы: ");

	char buff[BUFF];
	fgets(buff, BUFF - 1, stdin);	//Запись в буфер с ввода
	return buff[0];					//Возврат символа
}

//Заполнение матрицы Matrix случайными числами из диапазона Range
void FillWithRandom(Matrix* Matrix, Range* Range)
{
	srand(time(NULL)); //создаёт псевдослучайный сид
	if (Range->a > Range->b)	//"Переворачивание" диапазона, если он задан наоборот
	{
		int tmp = Range->b;
		Range->b = Range->a;
		Range->a = tmp;
	}
	for (int i = 0; i < Matrix->vsize; ++i)		//Перебор строк
		for (int j = 0; j < Matrix->hsize; ++j) //Перебор столбцов в строках
			Matrix->nums[i*Matrix->hsize + j] = rand() % (Range->b - Range->a) + Range->a;
}

//Заполнение ячеек матрицы Matrix согласно формуле
void FillWithFormula(Matrix* Matrix)
{
	for (int i = 0; i < Matrix->vsize; ++i)		//Перебор строк
		for (int j = 0; j < Matrix->hsize; ++j) //Перебор столбцов в строках
		{
			if (i > j)
				Matrix->nums[i * Matrix->hsize + j] = i + j;
			else if (i == j)
				Matrix->nums[i * Matrix->hsize + j] = 1;
			//Matrix->nums[i * Matrix->hsize + j] = i / (j + 1);
			else //if i < j
				Matrix->nums[i * Matrix->hsize + j] = i - j;
		}
}

//Вывод матрицы Matrix в текстовом виде в данный исток sink
void PrintMatrix(Matrix* Matrix, FILE* sink)
{
	for (int i = 0; i < Matrix->vsize; ++i)		//Перебор строк
	{
		for (int j = 0; j < Matrix->hsize; ++j)	//Перебор столбцов в строках
			fprintf(sink, "%6.2lf ", Matrix->nums[i * Matrix->hsize + j]);
		fputc('\n', sink);
	}
	fputc('\n', sink);
}

//Осуществление выбора, что и куда будет выводиться 
uint8_t SelectOutput(void) //UNDONE
{
	char c = '\n';
	uint8_t flag = 0;

	printf("Чтобы вывести наибольший элемент среди минимальных элементов каждой строки, введите y: ");
	while ((c = fgetc(stdin)) == '\n'); //Ввод будет игнорировать новые строки, оставшиеся с прошлого ввода
	if (c == 'y')
		flag |= FIND_ELEMENT;

	printf("Чтобы вывести матрицу на экран, введите y: ");
	while ((c = fgetc(stdin)) == '\n'); //Ввод будет игнорировать новые строки, оставшиеся с прошлого ввода
	if (c == 'y')
		flag |= PRINT_TO_SCREEN;

	printf("Чтобы вывести матрицу на текстовый файл, введите t.\n%s",
		"Чтобы вывести матрицу в бинарный файл, введите b: ");
	while ((c = fgetc(stdin)) == '\n');
	if (c == 't')
		return flag |= TXT_FILE;
	if (c == 'b')
		flag |= BIN_FILE;

	return flag;
}

/*Получение минимального числа из массива чисел arr размером size*/
double GetMinFromArray(double* arr, int size)
{
	double min = arr[0];
	for (int n = 1; n < size; n++)
		if (arr[n] < min)
			min = arr[n];
	return min;
}

/*Получение максимального числа из массива чисел arr размером size*/
double GetMaxFromArray(double* arr, int size)
{
	double min = arr[0];
	for (int n = 1; n < size; n++)
		if (arr[n] > min)
			min = arr[n];
	return min;
}

/*Получение максимального числа среди минимальных чисел каждой строки*/
int GetMaxOfMins(const Matrix* Matrix, double* Answer)
{
	double* mins = (double*)malloc(Matrix->vsize * sizeof(double));
	if (!mins)
		return MALLOC_FAIL;

	for (int i = 0; i < Matrix->vsize; ++i)
		mins[i] = GetMinFromArray((Matrix->nums + i * Matrix->vsize), Matrix->hsize);

	*Answer = GetMaxFromArray(mins, Matrix->vsize);

	free(mins);
	return NOERROR;
}
