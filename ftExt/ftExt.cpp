// ftExt.cpp : Defines the exported functions for the DLL.

#include "pch.h" 

/*
	Входящие инпуты в RVExtension
		version - просто возврат версии и линковка к процессу армы
		id - генерирует рандомный набор букв и цифр для идентификации объекта
		time - возврат времени в виде [2020,8,10,14,52,12]; ГГГГ, ММ, ДД, ЧЧ, ММ, СС

	Входящие инпуты в RVExtensionArgs
	Радио:
		create,[id, url, volume, posX, posY, posZ]
		pos, [id, posX, posY, posZ]
		orient [[eyeDirection player], [vectorUp player]]
		vol, [id, volume]
		destroy [id]
		urlInfo [id, url]

	Возврат каких либо результатов в игру через RVExtensionRegisterCallback
				
*/

#pragma comment (lib, "winmm.lib")

using namespace std;
#define CURRENT_VERSION "1.0.0 by FT5571 / rimasrp.life copyright"
#define MACROS_STRNCPY(x) strncpy_s(output, outputSize, x, _TRUNCATE)
#define CBK(x,y) callbackPtr("ftExt",x,y)


extern "C"
{
	__declspec (dllexport) void __stdcall RVExtensionRegisterCallback(int(*callbackProc)(char const* name, char const* function, char const* data));
	//--- Вызывается только движком, просто возврат версии
	__declspec (dllexport) void __stdcall RVExtensionVersion(char* output, int outputSize);
	//--- STRING вызывает екстеншн STRING
	__declspec (dllexport) void __stdcall RVExtension(char* output, int outputSize, const char* function);
	//--- STRING вызывает екстеншн ARRAY
	__declspec (dllexport) int __stdcall RVExtensionArgs(char* output, int outputSize, const char* function, const char** args, int argsCnt);
}

//--- Общие глобальные переменные
BOOL RadioInited = false;
HSTREAM stream;
std::string buf;
FILE* file = NULL;
BASS_3DVECTOR* pos;

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

void loadLibs()
{
	HINSTANCE hModule = NULL;
#ifdef _WIN64
	hModule = ::LoadLibrary(L"bass_x64.dll");

#else
	hModule = ::LoadLibrary(L"bass.dll");
#endif
	if (hModule != NULL) {
		std::cout << "Lib Loaded" << endl;
		::FreeLibrary(hModule);
	}
	else {
		std::cout << "Error load library" << endl;
	}
}

//--- Обработчик для игры
int(*callbackPtr)(char const* name, char const* function, char const* data) = nullptr;
void __stdcall RVExtensionRegisterCallback(int(*callbackProc)(char const* name, char const* function, char const* data))
{
	callbackPtr = callbackProc;
}

//--- Тут мелочь всякая
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

//--- Вернет версию расширения в логи игры
void __stdcall RVExtensionVersion(char* output, int outputSize)
{
	//--- max outputSize is 32 bytes
	MACROS_STRNCPY(CURRENT_VERSION);
}

//--- Можно скачать поток
void __stdcall DownloadProc(const void* buffer, DWORD length, void *user) {
	if (!file) file = fopen("temp.mp3","wb");
	if (!buffer) fclose(file); else fwrite(buffer, 1, length, file);
}

int __stdcall RVExtensionArgs(char* output, int outputSize, const char* function, const char** args, int argsCnt)
{
	//--- Обработаем входящие args
	int i = 0;
	std::string str[8];
	std::string fnStr = function;
	while (i < argsCnt)
	{
		std::string s = args[i];
		str[i] = s.substr(1, s.length() - 2);
		i++;
	}

	if (!strcmp(function, "create"))
	{
		string track = str[0];


		if (!RadioInited)
		{
			//--- Загружена ли библиотека
			loadLibs();

			//--- Замутим тут 3D
			std::cout << str[0] << "|" << str[1] << "|" << str[2] << endl;
			float x = 0, y = 0, z = 0, vol = 0;

			stringstream v(str[1]);
			v >> vol;
			vol = (vol >= 0 and vol <= 100) ? vol /= 100 : 0.5;

			stringstream g(str[2]);
			g >> x;
			stringstream s(str[3]);
			s >> y;
			stringstream a(str[4]);
			a >> z;
			std::cout << x << "|" << y << "|" << z << endl;

			//--- Конфиги
			BASS_SetConfig(BASS_CONFIG_BUFFER, 100);
			BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 10);
			BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, 1);

			//--- Устройство 1, чтобы можно было регулировать громкость конкретно потока, а не всего устройства в целом
			BASS_Init(1, 44100, BASS_DEVICE_3D | BASS_DEVICE_MONO, 0, 0);

			//--- Создаем поток
			stream = BASS_StreamCreateURL(track.c_str(), (BASS_SAMPLE_3D | BASS_SAMPLE_MONO), 0, NULL, 0);
			
			//--- Музяка играет
			BASS_ChannelPlay(stream,TRUE);

			//--- 3D собственно
			BASS_Set3DFactors(1.0, 2.0, 2.0);
			pos = new BASS_3DVECTOR(x, y, z);
			BASS_3DVECTOR* orient = new BASS_3DVECTOR(x, y, z);
			BASS_3DVECTOR* velocity = new BASS_3DVECTOR(x,y,z);
			BASS_ChannelSet3DPosition(stream,pos,NULL,NULL);

			//--- Установим громкость
			BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, vol);

			BASS_Apply3D();

			//--- Заплатка для предотвращения повторного запуска одного потока
			::RadioInited = true;
			CBK("RRPClient_radio_error", "Radio started");
		} else {

			//--- Перезапустим поток если он уже работает
			CBK("RRPClient_radio_error","Radio already started, try to restart");
			BASS_StreamFree(stream);
			BASS_Free();
			::RadioInited = false;
			RVExtensionArgs(output, outputSize, function, args, argsCnt);
			CBK("RRPClient_radio_error", "Radio started again");
		};
		return 200;
	}else if (!strcmp(function, "urlInfo")) {

		//--- Получаем то что сейчас играет
		const char* tag = BASS_ChannelGetTags(stream, BASS_TAG_META);
		string s = tag;

		//--- Обрежем лишнее
		s = s.substr(13, s.length() - 15); 
		s = "[\"Play\",\"" + s + "\"]";

		//--- Дадим игре понять что, что то там пришло
		CBK("RRPClient_radio_refresh",s.c_str());
		return 200;
	}else if (!strcmp(function, "pos")) {

		std::cout << str[0] << "|" << str[1] << "|" << str[2] << endl;
		float x = 0, y = 0, z = 0;
		stringstream g(str[0]);
		g >> x;
		stringstream s(str[1]);
		s >> y;
		stringstream a(str[2]);
		a >> z;
		std::cout << x << "|" << y << "|" << z << endl;
		
		pos->x = x;
		pos->y = y;
		pos->z = z;
		BASS_ChannelSet3DPosition(stream, pos, NULL, NULL);
		BASS_Apply3D();

		return 200;
	}else if (!strcmp(function, "orient")) {

		float xE = 0, yE = 0, zE = 0;
		stringstream g(str[0]);
		g >> xE;
		stringstream s(str[1]);
		s >> yE;
		stringstream a(str[2]);
		a >> zE;



		return 200;
	}else if (!strcmp(function, "vol")) {

		//--- Громкость в стринг, по этому конвертируем ее в int
		stringstream geek(str[0]);
		float vol = 0;
		geek >> vol;
		vol = (vol >= 0 and vol <= 100) ? vol /= 100 : 0.5;
		printf("Volume set to %f",vol);

		//--- Установим громкость
		BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL | BASS_ATTRIB_PAN, vol);

		CBK("RRPClient_radio_refresh", "Volume set");
		return 200;
	}else if (!strcmp(function, "destroy")) {

		//--- Остановим поток
		BASS_StreamFree(stream);
		BASS_Free();
		CBK("RRPClient_radio_stop", "Stopped");
		::RadioInited = false;
		return 200;
	} else {

		//--- Я не понимаю что ты тут хочешь
		buf = "Command: '" + fnStr + "' is undefined";
		CBK("RRPClient_radio_error", buf.c_str());
		return 404;
	};
	return -1;
};


