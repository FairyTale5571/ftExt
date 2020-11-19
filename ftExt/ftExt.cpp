// ftExt.cpp : Defines the exported functions for the DLL.

#include "pch.h" 

/*
	Входящие инпуты в RVExtension
		version - просто возврат версии и линковка к процессу армы
		id - генерирует рандомный набор букв и цифр для идентификации объекта
		time - возврат времени в виде [2020,8,10,14,52,12]; ГГГГ, ММ, ДД, ЧЧ, ММ, СС

	Входящие инпуты в RVExtensionArgs
	Радио:
		create,[url, volume, posX, posY, posZ]
		pos, [posX, posY, posZ, | eyeX, eyeY, eyeZ, | velX, velY, velZ]
		orient [[eyeDirection player], [vectorUp player]]
		vol, [volume]
		destroy [id]
		urlInfo [id, url]

	Возврат каких либо результатов в игру через RVExtensionRegisterCallback
				
*/

#pragma comment (lib, "winmm.lib")

using namespace std;
#define CURRENT_VERSION "1.0.0 by FT5571 / rimasrp.life copyright"
#define MACROS_STRNCPY(x) strncpy_s(output, outputSize, x, _TRUNCATE)
#define CBK(x,y) callbackPtr("ftExt",x,y)
//#define Check(X) CheckError(X, __FILE__, __LINE__)

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

char logs[256];

BASS_3DVECTOR* pos;
BASS_3DVECTOR* ori;
BASS_3DVECTOR* vel;

void 
getTime(char* output, int outputSize)
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

string 
random_string(int len)
{
	std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	std::random_device rd;
	std::mt19937 generator(rd());
	std::shuffle(str.begin(), str.end(), generator);
	return str.substr(0, len);
}

void 
loadLibs()
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
void __stdcall 
RVExtensionRegisterCallback(int(*callbackProc)(char const* name, char const* function, char const* data))
{
	callbackPtr = callbackProc;
}

//--- Тут мелочь всякая
void __stdcall 
RVExtension(char* output, int outputSize, const char* function)
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
void __stdcall 
RVExtensionVersion(char* output, int outputSize)
{
	//--- max outputSize is 32 bytes
	MACROS_STRNCPY(CURRENT_VERSION);
}

//--- Можно скачать поток
void __stdcall 
DownloadProc(const void* buffer, DWORD length, void *user) {
	if (!file) file = fopen("temp.mp3","wb");
	if (!buffer) fclose(file); else fwrite(buffer, 1, length, file);
}

/*
void
bvtod(const BASS_3DVECTOR& v, float* d) {
	if (!d) return;
	d[0] = v.x;
	d[1] = v.y;
	d[2] = v.z;
}

BASS_3DVECTOR*
dtobv(float* d, BASS_3DVECTOR& v) {
	if (!d) return;
	v.x = d[0];
	v.y = d[1];
	v.z = d[2];
	return &v;
}

BOOL
Set3DPos(float* pos, float* ori, float* vel) {
	BASS_3DVECTOR vpos, vori, vvel,
	*fpos = dtobv(pos,vpos), *fori = dtobv(ori,vori), *fvel = dtobv(vel,vvel);
	return Check(BASS_ChannelSet3DPosition(stream, fpos, fori, fvel));
}

BOOL 
Get3DPos(float* pos, float* ori, float* vel) {
	BASS_3DVECTOR vpos, vori, vvel;
	BOOL re = Check(BASS_ChannelGet3DPosition(stream, &vpos, &vori, &vvel));
	bvtod(vpos, pos);
	bvtod(vori, ori);
	bvtod(vvel, vel);
	return re;
}
*/

int __stdcall 
RVExtensionArgs(char* output, int outputSize, const char* function, const char** args, int argsCnt)
{
	//--- Обработаем входящие args
	int i = 0;
	std::string str[11];
	std::string fnStr = function;
	while (i < argsCnt)
	{
		std::string s = args[i];
		str[i] = s.substr(1, s.length() - 2);
		i++;
	}
	if (!callbackPtr)
		return 500;
	if (!strcmp(function, "create"))
	{

		if (!RadioInited)
		{
			//"http://air.radiorecord.ru:8102/rock_320", "100", "3170.38", "3350.12", "5.00144", "0", "0", "0", "0", "0", "0"
			string url = str[0], v = str[1], pX = str[2], pY = str[3], pZ = str[4];

			//--- Загружена ли библиотека
			loadLibs();

			//--- Замутим тут 3D
			float x = 0, y = 0, z = 0, vol = 0, oriX = 0, oriY = 0, oriZ = 0, velX = 0, velY = 0, velZ = 0;

			stringstream vS(v); vS >> vol; vol = (vol >= 0 and vol <= 100) ? vol /= 100 : 0.5;

			stringstream g_pX(pX); g_pX >> x;
			stringstream g_pY(pY); g_pY >> y;
			stringstream g_pZ(pZ); g_pZ >> z;

			//--- Конфиги

			//BASS_SetConfig(BASS_CONFIG_BUFFER, 100);
			//BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 10);
			//BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, 1);

			//--- Устройство 1, чтобы можно было регулировать громкость конкретно потока, а не всего устройства в целом
			if (!BASS_Init(1, 44100, BASS_DEVICE_3D | BASS_DEVICE_MONO, 0, 0)) {
				printf("Error init: %i", BASS_ErrorGetCode());
			};

			//--- Создаем поток
			stream = BASS_StreamCreateURL(url.c_str(), (BASS_SAMPLE_3D | BASS_SAMPLE_MONO), 0, NULL, 0);
			
			//--- Музяка играет
			BASS_ChannelPlay(stream,TRUE);

			//--- 3D собственно
			BASS_Set3DFactors(1.0, 2.0, 2.0);
			pos = new BASS_3DVECTOR(x, y, z);
			ori = new BASS_3DVECTOR(NULL, NULL, NULL);
			vel = new BASS_3DVECTOR(NULL, NULL, NULL);

			BASS_ChannelSet3DPosition(stream, pos, NULL, NULL);

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

	}else if (!strcmp(function, "refresh3d_pos")) {

		//--- Обновляем 3D
		float x = 0, y = 0, z = 0;
		stringstream g_pX(str[0]); g_pX >> x;
		stringstream g_pY(str[1]); g_pY >> y;
		stringstream g_pZ(str[2]); g_pZ >> z;
		
		pos->x = x;
		pos->y = y;
		pos->z = z;

		if (!BASS_ChannelSet3DPosition(stream, pos,NULL, NULL)) {
			buf = " Update pos: Error code:" + BASS_ErrorGetCode();
			CBK("RRPClient_radio_updatePos", buf.c_str());

		};
		BASS_Apply3D();
		CBK("RRPClient_radio_updatePos", "Position updated");

		return 200;
	}else if (!strcmp(function, "refresh3d_orient")) {
		float oriX = 0, oriY = 0, oriZ = 0, velX = 0, velY = 0, velZ = 0;

		stringstream g_oX(str[0]); g_oX >> oriX; ori->x = oriX;
		stringstream g_oY(str[1]); g_oY >> oriY; ori->y = oriY;
		stringstream g_oZ(str[2]); g_oZ >> oriZ; ori->z = oriZ;

		stringstream g_vX(str[3]); g_vX >> velX; vel->x = velX;
		stringstream g_vY(str[4]); g_vY >> velY; vel->y = velY;
		stringstream g_vZ(str[5]); g_vZ >> velZ; vel->z = velZ;

		if (!BASS_Set3DPosition(ori, vel, NULL, NULL)) {
			snprintf(logs, sizeof(logs), "Error Code %i",BASS_ErrorGetCode());
			CBK("RRPClient_radio_updatePos", logs);
		};
		BASS_Apply3D();

		snprintf(logs, sizeof(logs), "Orient updated O|X:%f,Y:%f,Z:%f V|X:%f,Y:%f,Z:%f ", ori->x, ori->y, ori->z, vel->x, vel->y, vel->z);
		CBK("RRPClient_radio_updatePos", logs);
		return 200;

	}else if (!strcmp(function, "vol")) {

		//--- Громкость в стринг, по этому конвертируем ее в int
		stringstream geek(str[0]);
		float vol = 0;
		geek >> vol;
		vol = (vol >= 0 and vol <= 100) ? vol /= 100 : 0.5;
		printf("Volume set to %f",vol);

		//--- Установим громкость
		BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, vol);

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


