#include <iostream>
#include <time.h>
#include <Windows.h>
#include <conio.h>
#include <chrono>
#include <memory>
#include <winuser.h>
#include <io.h>

using namespace std;
using namespace std::chrono;

enum Color
{
	Black, Blue, Green, Cyan, Red, Magenta, Brown,
	LightGray, DarkGray, LightBlue, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White
};
HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
static const char* const g_filename = "Users.txt";
void setColor(Color text, Color background)
{
	SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}
enum Direction { Up = 72, Left = 75, Right = 77, Down = 80, Enter = 13, esc = 27 };
void setCursor(int x, int y)
{
	COORD myCoords = { x,y };
	SetConsoleCursorPosition(hStdOut, myCoords);
}

const int H = 22, W = 12;//Высота, ширина массива состояния 
int StatusArray[H][W] = {};//Массив состояния
int field_X = 2, field_Y = 7;//Координаты поля

//Структура фигуры
struct Figure
{
	int rowsize = 0;
	int colsize = 0;
	int** fig;
	//конструктор - функция, которая вызывается при создании объекта
	Figure(int row, int col)
	{
		rowsize = row;
		colsize = col;
		fig = new int* [rowsize];
		for (size_t i = 0; i < rowsize; i++)
		{
			fig[i] = new int[colsize];
		}
	}
};
//Структура пользователя
struct User
{
	char login[20];
	char password[20];
	int score = 0;
};
//***********************************************************************
//**********************Игровые функции**********************************
//***********************************************************************
//Игровое поле
void GameField(int X, int Y)
{
	for (size_t i = 0; i < H; i++)
	{
		setCursor(X, Y + i);
		for (size_t j = 0; j < W; j++)
		{
			if (j == 0 || j == W - 1 || i == H - 1)
				StatusArray[i][j] = 1;
			setColor(Color(StatusArray[i][j]), Color(StatusArray[i][j]));
			cout << StatusArray[i][j];
		}
	}
	setColor(White, Black);
}
//Функция для заполнения массивов фигур
void setState(Figure& fig, int* mas)
{
	int index = 0;
	for (size_t i = 0; i < fig.rowsize; i++)
	{
		for (size_t j = 0; j < fig.colsize; j++)
		{
			fig.fig[i][j] = mas[index++];
		}
	}
}
//Функция отрисовывает фигуру на поле
void printFigure(const Figure& obj, int paint, int X, int Y)
{
	for (size_t i = 0; i < obj.rowsize; i++)
	{
		setCursor(X, Y + i);
		for (size_t j = 0; j < obj.colsize; j++)
		{
			if (obj.fig[i][j] == 1)
			{
				setColor(Color(paint), Color(paint));
			}
			else
				setColor(Black, Black);
			cout << obj.fig[i][j];
		}
	}
	setColor(White, Black);
}
//Удаление рядов
void Row_Destroy(int Y)
{
	for (size_t y = Y; y > 2; y--)
	{
		for (size_t x = 1; x < W - 1; x++)
		{
			StatusArray[y][x] = StatusArray[y - 1][x];
		}
	}
}
//Функция проверяет какие ряды можно удалить
int CheckLine()
{
	int x, y;
	int rows = 0;
	bool full;
	for (x = 1; x < H - 1; x++)
	{
		full = true;
		for (y = 1; y < W - 1; y++)
		{
			if (!StatusArray[x][y])
			{
				full = false;
				break;
			}
		}
		if (full)
		{
			setCursor(field_X, x);
			Sleep(100);
			Row_Destroy(x);
			rows++;
		}
	}
	return rows;
}
//Условие проигрыша
bool IsLost()
{
	bool check = true;
	for (size_t j = 1; j < W - 1; j++)
	{
		if (!StatusArray[2][j])
		{
			check = true;
		}
		else
		{
			check = false;
			break;
		}
	}
	return check;
}
//Показывает следующую фигуру
void ShowNextFigure(int index, int array)
{
	const int size = 4;
	int arr[size][size];
	setCursor(15, 7);
	cout << "Next Figure:\n";
	setColor(White, White);
	for (size_t i = 0; i < size; i++)
	{
		setCursor(15, 8 + i);
		for (size_t j = 0; j < size; j++)
		{
			arr[i][j] = 0;
			cout << arr[i][j];
		}
	}
	setColor(Black, White);
}

//***********************************************************************
//**********************Менюшные функции*********************************
//***********************************************************************

//Сортировка массива
void Sort(User* array, int length)
{
	bool key;
	int count = 0;
	do
	{
		key = false;
		for (size_t i = 0; i < length - 1 - count; i++)
		{
			if (array[i].score < array[i + 1].score)
			{
				swap(array[i], array[i + 1]);
				key = true;
			}
		}
		count++;
	} while (key);
}
//Функция выводит на экран топ-10 лучших игроков
void ShowLeader(User* array, int length)
{
	if (length > 10)
	{
		length = 10;
	}
	for (size_t i = 0; i < length; i++)
	{
		if (i < 9)
		{
			cout << " ";
		}
		cout << i + 1 << ":  " << array[i].login << "\t" << array[i].score << "\n";
	}
}
//Рамка меню
void MenuBorder()
{
	const int a = 9;
	const int b = 21;
	char arr[a][b];
	int X = 7, Y = 8;
	for (size_t i = 0; i < a; i++)
	{
		for (size_t j = 0; j < b; j++)
		{
			if (i == 0 || j == 0 || i == a - 1 || j == 20)
				arr[i][j] = '*';
			else
				arr[i][j] = ' ';
		}
	}
	for (size_t i = 0; i < a; i++)
	{
		setCursor(X, Y + i);
		for (size_t j = 0; j < b; j++)
		{
			if (arr[i][j] == '*')
			{
				setColor(White, White);
			}
			else
				setColor(White, Black);
			cout << arr[i][j];
		}
	}
	setColor(White, Black);
}
//Функция для вывода меню и подсветки активного пункта
void ShowMenu(char array[][30], int str_amount, int selected_str)
{
	MenuBorder();
	int X = 10, Y = 10;
	for (size_t i = 0; i < str_amount; i++)
	{
		setCursor(X, Y + i);
		if (i == selected_str)
		{
			setColor(Blue, Black);
		}
		else
			setColor(Green, Black);
		cout << array[i];
	}
	setColor(White, Black);
}
//Функция для пермещения по меню
int MotionMenu(char array[5][30], int str_amount)
{
	int key, item = 0;
	do {
		ShowMenu(array, str_amount, item);
		key = _getch();
		switch (key)
		{
		case Up:
			if (item == 0)
			{
				item = str_amount - 1;
			}
			else if (item > 0)
			{
				item--;
			}
			break;
		case Down:
			if (item == str_amount - 1)
			{
				item = 0;
			}
			else if (item < str_amount)
			{
				item++;
			}
			break;
		case esc:
			return -1;
			break;
		case Enter:
			return item;
			break;
		default:
			break;
		}
	} while (true);
}
//Открытия пункта "Авторизация"
void Authorization(bool& auth, User& user)
{
	FILE* file = nullptr;
	User temp/*, user*/;
	bool key = false;
	int size;
	Sleep(300);
	system("cls");
	cout << "Выбрана авторизация\n";
	fopen_s(&file, "Users.txt", "r");
	if (file != nullptr)//проверяем, что открылся поток
	{
		size = _filelength(_fileno(file)) / sizeof(User);//Количество записей
		if (size)
		{
			cout << "Введите свой логин: ";
			cin.getline(user.login, 20);
			cout << "Введите свой пароль: ";
			cin.getline(user.password, 20);
			for (size_t i = 0; i < size; i++)
			{
				fread(&temp, sizeof(User), 1, file);//куда будет осуществляться считывание, размер блока данных, которые нужно считать
				//количество считываемых объектов, откуда считывается
				if (_stricmp(temp.login, user.login) == 0)
				{
					if (strcmp(temp.password, user.password) == 0)
					{
						cout << "Удачная авторизация\n";
						auth = true;
						key = true;
						break;
					}
					else
					{
						cout << "Неверный пароль\n";
						key = true;
						break;
					}
				}
			}
			if (!key)
			{
				cout << "Пользователь не найден\n";
				auth = false;
			}
		}
		fclose(file);
	}
	else
	{
		cout << "Нет зарегистрированных пользователей\n";
	}
}
//Открытия пункта "Регистрация"
void Registration(bool& auth, User& user)
{
	FILE* file = nullptr;
	User temp;
	int size;
	bool key = false;
	Sleep(300);
	system("cls");
	cout << "Выбрана регистрация\n";
	cout << "Для регистрации введите логин и пароль:\n";
	cout << "login: ";
	cin >> user.login;
	cout << "pass: ";
	cin >> user.password;
	user.score = 0;
	fopen_s(&file, "Users.txt", "r");
	if (file != nullptr)
	{
		size = _filelength(_fileno(file)) / sizeof(User);//Количество записей					 
		for (size_t i = 0; i < size; i++)
		{
			fread(&temp, sizeof(User), 1, file);
			if (_stricmp(temp.login, user.login) == 0)//Если логин совпадает с тем, что мы только что считали
			{
				cout << "Такой пользователь уже существует\n";
				key = true;
				break;
			}
		}
		fclose(file);
		if (!key)
		{
			auth = true;
			fopen_s(&file, "Users.txt", "a");
			if (file != nullptr)
			{				
				fwrite(&user, sizeof(User), 1, file);
				fclose(file);
				cout << "Регистрация прошла успешно\n";
			}
		}
	}
	else
	{
		fopen_s(&file, "Users.txt", "w");
		{
			fwrite(&user, sizeof(User), 1, file);
			fclose(file);
			cout << "Регистрация прошла успешно\n";
		}
	}
}
//Таблица лидеров
void Leaderboard()
{
	FILE* file = nullptr;
	int size;
	Sleep(200);
	system("cls");
	cout << "Таблица лидеров:\n\n\n";
	fopen_s(&file, "Users.txt", "r");
	if (file != nullptr)
	{
		size = _filelength(_fileno(file)) / sizeof(User);
		if (size)
		{
			std::unique_ptr<User[]> arr(new User[size]);
			fread(arr.get(), sizeof(User), size, file);
			fclose(file);
			Sort(arr.get(), size);
			ShowLeader(arr.get(), size);
		}
		else
		{
			cout << "Таблица лидеров пуста\n";
		}
	}
}

void WriteUser(const User& user, FILE* file)
{
	if (file != nullptr)
	{
		fprintf(file, "%s %d\n", user.login, user.score);		
		fclose(file);
	}
}
void SavePoints(User& user)
{
	User temp;
	int size;
	FILE* file = nullptr;
	fopen_s(&file, "Users.txt", "r");
	if (file != nullptr)
	{
		size = _filelength(_fileno(file)) / sizeof(User);//Количество записей					 
		for (size_t i = 0; i < size; i++)
		{
			fread(&temp, sizeof(User), 1, file);
			if (_stricmp(temp.login, user.login) == 0)
			{
				if (temp.score < user.score)
				{						
					WriteUser(user, file);
				}
				break;
			}
		}
	}
}

int main()
{
	srand(time(NULL));
	setlocale(LC_ALL, "rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	const int figure_variatoin = 28;
	//набор 0 и 1 для всех фигур и для всех вариантов вращения
	int arr_figure[figure_variatoin][6] = {
				/*I*/		 /*O*/		   /*J*/			/*L*/			/*Z*/		  /*S*/		       /*T*/
		/*0 */{ 1,1,1,1 },{ 1,1,1,1 },{ 0,1,0,1,1,1 },{ 1,0,1,0,1,1 },{ 1,1,0,0,1,1 },{ 0,1,1,1,1,0 },{ 0,1,0,1,1,1 },
		/*7 */{ 1,1,1,1 },{ 1,1,1,1 },{ 1,1,1,0,0,1 },{ 0,0,1,1,1,1 },{ 0,1,1,1,1,0 },{ 1,0,1,1,0,1 },{ 0,1,1,1,0,1 },
		/*14*/{ 1,1,1,1 },{ 1,1,1,1 },{ 1,1,1,0,1,0 },{ 1,1,0,1,0,1 },{ 1,1,0,0,1,1 },{ 0,1,1,1,1,0 },{ 1,1,1,0,1,0 },
		/*21*/{ 1,1,1,1 },{ 1,1,1,1 },{ 1,0,0,1,1,1 },{ 1,1,1,1,0,0 },{ 0,1,1,1,1,0 },{ 1,0,1,1,0,1 },{ 1,0,1,1,1,0 }
	};
	//размер массива для каждой фигуры и для каждого варианта вращения
	Figure SetOfFigures[figure_variatoin] = {
					/*I*/		  /*O*/			/*J*/		  /*L*/			/*Z*/		  /*S*/			/*T*/
		/*rot1*/Figure(4, 1), Figure(2, 2), Figure(3, 2), Figure(3, 2), Figure(2, 3), Figure(2, 3), Figure(2, 3),
		/*rot2*/Figure(1, 4), Figure(2, 2), Figure(2, 3), Figure(2, 3), Figure(3, 2), Figure(3, 2), Figure(3, 2),
		/*rot3*/Figure(4, 1), Figure(2, 2), Figure(3, 2), Figure(3, 2), Figure(2, 3), Figure(2, 3), Figure(2, 3),
		/*rot4*/Figure(1, 4), Figure(2, 2), Figure(2, 3), Figure(2, 3), Figure(3, 2), Figure(3, 2), Figure(3, 2)
	};
	//Заполняем массивы фигур 
	for (size_t i = 0; i < figure_variatoin; i++)
	{
		setState(SetOfFigures[i], arr_figure[i]);
	}
	//Переменные для игры
	int X, Y;//глобальныек координаты фигуры
	int fig_x, fig_y;//координаты фигуры относительно поля
	int temp_x, temp_y;//координаты фигуры в предыдущей итерации
	int rand_fig = rand() % 7;
	bool possible_down, possible_left, possible_right, possible_rotate;
	int speed, var;
	int next_figure, paint;	
	//Переменные для меню
	User user, temp;
	int size;
	FILE* file = nullptr;
	int key = 0;	
	bool auth = false;
	bool main_menu = true;
	bool exit = false;
	const int length = 5;
	int item_num;
	char login[20], password[20];
	//Меню игры
	char menu[length][30]{ "Новая игра", "Авторизация", "Регистрация", "Таблица лидеров", "Выход" };
	if (main_menu)
	{
		do
		{
			setCursor(29, 10);
			cout << "Player: ";
			if (!auth)
			{
				cout << "none";
			}
			else
				cout << user.login;
			setCursor(29, 12);
			cout << "score: ";
			if (!auth)
			{
				cout << 0;
			}
			else
				cout << user.score;
			item_num = MotionMenu(menu, length);
			if (item_num == -1 || item_num == 4)
			{
				Sleep(250);
				system("cls");
				cout << "Конец\n";
				exit = true;
			}
			else
			{
				if (item_num == 0)//Новая игра
				{
					main_menu = false;
					system("cls");
					if (!auth)
					{
						cout << "Для игры необходимо зарегистрироваться или авторизоваться\n";
						do
						{
							cout << "\nНажмите Enter, чтобы вернуться в главное меню";
							key = _getch();
						} while (key != Enter);
						system("cls");
						main_menu = true;
					}
					else
					{
						Sleep(200);
						system("cls");
						do
						{
							GameField(field_X, field_Y);
							fig_x = 4;
							fig_y = 0;
							X = 6;
							Y = 7;
							next_figure = rand() % 7;
							setCursor(15, 7);
							cout << "Next Figure:\n";
							printFigure(SetOfFigures[next_figure], next_figure + 2, 18, 9);
							setCursor(15, 14);
							cout << "Player: " << user.login;
							setCursor(15, 15);
							cout << "Score: " << user.score;
							paint = rand_fig + 2;
							do
							{
								var = 0;
								possible_down = true;
								possible_left = true;
								possible_right = true;
								possible_rotate = true;
								temp_x = X;
								temp_y = Y;
								while (var < 100)
								{
									printFigure(SetOfFigures[rand_fig], paint, X, Y);
									if (GetAsyncKeyState(VK_LEFT) & 0x25)
									{
										//проверяем можно ли сместить влево
										for (size_t i = 0, j = 0; i < SetOfFigures[rand_fig].rowsize; i++)
										{
											if ((SetOfFigures[rand_fig].fig[i][j] && !StatusArray[fig_y + i][fig_x - 1]) || !SetOfFigures[rand_fig].fig[i][j])
											{
												possible_left = true;
											}
											else if (SetOfFigures[rand_fig].fig[i][j] && StatusArray[fig_y + i][fig_x - 1])
											{
												possible_left = false;
												break;
											}
										}
										if (possible_left)
										{
											X--;
											fig_x--;
										}
										printFigure(SetOfFigures[rand_fig], 0, temp_x, temp_y);
										temp_x = X;
									}
									if (GetAsyncKeyState(VK_RIGHT) & 0x27)
									{
										//проверяем можно ли сместить вправо
										for (size_t i = 0, j = SetOfFigures[rand_fig].colsize - 1; i < SetOfFigures[rand_fig].rowsize; i++)
										{
											if ((SetOfFigures[rand_fig].fig[i][j] && !StatusArray[fig_y + i][fig_x + SetOfFigures[rand_fig].colsize]) ||
												!SetOfFigures[rand_fig].fig[i][j])
											{
												possible_right = true;
											}
											else if (SetOfFigures[rand_fig].fig[i][j] && StatusArray[fig_y + i][fig_x + SetOfFigures[rand_fig].colsize])
											{
												possible_right = false;
												break;
											}
										}
										if (possible_right)
										{
											X++;
											fig_x++;
										}
										printFigure(SetOfFigures[rand_fig], 0, temp_x, temp_y);
										temp_x = X;
									}
									if (GetAsyncKeyState(VK_UP) /*& 0x26*/)
									{
										//проверяем есть ли место для вращения фигурки
										for (size_t i = fig_y; i < fig_y + SetOfFigures[rand_fig].colsize; i++)
										{
											for (size_t j = fig_x; j < fig_x + SetOfFigures[rand_fig].rowsize; j++)
											{
												if (StatusArray[i][j])
												{
													possible_rotate = false;
													break;
												}
												else
													possible_rotate = true;
											}
										}										
										if (possible_rotate)
										{											
											printFigure(SetOfFigures[rand_fig], 0, temp_x, temp_y);
											if (rand_fig >= 21)
											{
												rand_fig -= 21;												
											}
											else
											{
												rand_fig += 7;												
											}
										}
									}
									if (GetAsyncKeyState(VK_DOWN) /*& 0x28*/)
									{
										speed = 10;
									}
									else
										speed = 5;
									var += speed;
									Sleep(10);
								}
								printFigure(SetOfFigures[rand_fig], 0, temp_x, temp_y);
								setCursor(15, 2);							
								Y++;
								fig_y++;
								//проверка возможности смещения вниз								
								for (size_t i = SetOfFigures[rand_fig].rowsize - 1, j = 0; j < SetOfFigures[rand_fig].colsize;j++)
								{
									if ((SetOfFigures[rand_fig].fig[i][j] && !StatusArray[fig_y + SetOfFigures[rand_fig].rowsize - 1][fig_x + j]) ||
										!SetOfFigures[rand_fig].fig[i][j])
									{
										possible_down = true;
									}
									else if (SetOfFigures[rand_fig].fig[i][j] && StatusArray[fig_y + SetOfFigures[rand_fig].rowsize - 1][fig_x + j])
									{
										possible_down = false;
										break;
									}
								}
							} while (possible_down);
							//записываем фигуру в массив состояния
							for (size_t i = 0; i < SetOfFigures[rand_fig].rowsize; i++)
							{
								for (size_t j = 0; j < SetOfFigures[rand_fig].colsize; j++)
								{
									StatusArray[fig_y - 1 + i][fig_x + j] += SetOfFigures[rand_fig].fig[i][j] * paint;
								}
							}
							user.score += CheckLine() * 10;
							rand_fig = 0;
							rand_fig = next_figure;
							printFigure(SetOfFigures[next_figure], 0, 18, 9);
						} while (IsLost());
						//SavePoints(user);
						WriteUser(user, file);
						Sleep(300);
						system("cls");
						fopen_s(&file, "Users.txt", "r");
						fwrite(&user, sizeof(User), 1, file);
						cout << "Вы проиграли\n";
						cout << "\nНажмите Enter, чтобы вернуться в главное меню";
						do
						{						
							key = _getch();
						} while (key != Enter);
						system("cls");
						main_menu = true;
					}
				}
				else if (item_num == 1)//Авторизация
				{
					main_menu = false;
					Authorization(auth, user);
					Sleep(200);
					do
					{
						cout << "\nНажмите Enter, чтобы вернуться в главное меню";
						key = _getch();
					} while (key != Enter);
					system("cls");
					main_menu = true;
				}
				else if (item_num == 2)//Регистрация
				{
					main_menu = false;
					Registration(auth, user);
					Sleep(200);
					do
					{
						cout << "\nНажмите Enter, чтобы вернуться в главное меню";
						key = _getch();
					} while (key != Enter);
					system("cls");
					main_menu = true;
				}
				else if (item_num == 3)//Таблица лидеров
				{
					main_menu = false;
					Leaderboard();
					do
					{
						cout << "\nНажмите Enter, чтобы вернуться в главное меню";
						key = _getch();
					} while (key != Enter);
					system("cls");
					main_menu = true;
				}
			}
		} while (!exit);
	}
	return 0;
}

