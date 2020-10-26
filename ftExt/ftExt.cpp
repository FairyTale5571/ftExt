// ftExt.cpp : Defines the exported functions for the DLL.

#include "pch.h" 

/*
	Входящие инпуты в RVExtension
		version - просто возврат версии и линковка к процессу армы
		id - генерирует рандомный набор букв и цифр для идентификации объекта
		time - возврат времени в виде [2020,8,10,14,52,12]; ГГГГ, ММ, ДД, ЧЧ, ММ, СС

		tsCheck - проверить запущен ли тимспик
		tsPCheck - проверить есть ли плагин
		tsInstall - установить плагин тс автоматом, если не нашелся плагин

	Входящие инпуты в RVExtensionArgs
	Радио:
		create,[id, url, volume, posX, posY, posZ]
		pos, [id, posX, posY, posZ]
		orient [[eyeDirection player], [vectorUp player]]
		vol, [id, volume]
		destroy [id]
		urlInfo [id, url]

	Скриншоты:
		initScreen[cloudApi]
			Инициализирует функцию скриншотов, можно было бы сделать и со статичными параметрами, но т.к. чтобы dll можно было выпустить на прод, ее нужно добавить в вайтлист батлая, а это минимум две недели, 
			в случае если какое то облако накроется, можно будет без проблем его поменять
		createScreen[uid player, name player]
			делает скриншот экрана(ов) и сразу же выгружает в облако, сейчас буду использовать OneDrive

	Hardware:
		getHardware[тут много параметров будет]
			будет возвращать данные о железе в игру в виде текста, который уже игра будет парсить в массив

	Прочее:
		vac[uid]
			чекает стим аккаунт на наличие блокировок и нарушений

	Возврат каких либо результатов в игру через RVExtensionRegisterCallback
				
*/

#pragma comment (lib, "winmm.lib")

using namespace std;
#define CURRENT_VERSION "1.0.0 by FT5571 / rimasrp.life copyright"
#define MACROS_STRNCPY(x) strncpy_s(output, outputSize, x, _TRUNCATE)


void getTime(char* output, int outputSize)
{
	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);
	int year = now->tm_year + 1900;
	string y = to_string(year);
	int month = now->tm_mon + 1;
	string m = to_string(month);
	int day = now->tm_mday;
	string d = to_string(day);
	int hour = now->tm_hour;
	string h = to_string(hour);
	int min = now->tm_min;
	string M = to_string(min);
	int sec = now->tm_sec;
	string S = to_string(sec);
	string s = "[";
	s += y + "," + m + "," + d + "," + h + "," + M + "," + S;
	s += "]";
	strncpy(output, s.c_str(), outputSize);
}

string random_string(int len)
{
	std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	std::random_device rd;
	std::mt19937 generator(rd());
	std::shuffle(str.begin(), str.end(), generator);
	return str.substr(0, len);
}

void strcopyN(char* string1, char* string2, int n) {
	for (int i = 0; i < n; i++) {
		string1[i] = string2[i];
	}  
}

//--- name callExtension function
void __stdcall RVExtension(char* output, int outputSize, const char* function)
{
	setlocale(LC_ALL, "russian");
	std::string str = function;

	if (!str.compare("version"))
	{
		std::string vers = CURRENT_VERSION;
		MACROS_STRNCPY(vers.c_str());
	} else	if (!str.compare("id"))
	{
		MACROS_STRNCPY(random_string(9).c_str());
	} else	if (!str.compare("time"))
	{
		getTime(output, outputSize);
	}
	else
	{
		std::string err = ("Command '" + str + "' is undefined");
		MACROS_STRNCPY(err.c_str());
	}
}

//--- Extension version information shown in .rpt file
void __stdcall RVExtensionVersion(char* output, int outputSize)
{
	//--- max outputSize is 32 bytes
	MACROS_STRNCPY(CURRENT_VERSION);
}


int __stdcall RVExtensionArgs(char* output, int outputSize, const char* function, const char** args, int argsCnt)
{
	//Проверка на вак бан
	if (!strcmp(function, "vac"))
	{
		return 200;
	} else 
		//Загрузка железа
	if (!strcmp(function, "getHardware"))
	{
		return 200;
	}else 
	// Скриншоты
	if (!strcmp(function, "initScreen"))
	{
		return 200;
	}else if (!strcmp(function, "createScreen"))
	{
		return 200;
	}
	else
		// Радио
		if (!strcmp(function, "create"))
		{
			int i = 0;
			std::string str[8];

			while (i < argsCnt)
			{
				std::string s = args[i];
				str[i] = s.substr(1, s.length() - 2);
				i++;
			}
			string track = str[0];

			
		}
		else if (!strcmp(function, "pos"))
		{
			return 200;
		}
		else if (!strcmp(function, "orient"))
		{
			return 200;
		}
		else if (!strcmp(function, "vol"))
		{
			return 200;
		}
		else if (!strcmp(function, "destroy"))
		{
			//BASS_StreamFree(stream);
			return 200;
		}
		else if (!strcmp(function, "urlInfo"))
		{
			return 200;
		};
	return -1;
};


