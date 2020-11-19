// Приведенный ниже блок ifdef — это стандартный метод создания макросов, упрощающий процедуру
// экспорта из библиотек DLL. Все файлы данной DLL скомпилированы с использованием символа FTEXT_EXPORTS
// Символ, определенный в командной строке. Этот символ не должен быть определен в каком-либо проекте,
// использующем данную DLL. Благодаря этому любой другой проект, исходные файлы которого включают данный файл, видит
// функции FTEXT_API как импортированные из DLL, тогда как данная DLL видит символы,
// определяемые данным макросом, как экспортированные.
#ifdef FTEXT_EXPORTS
#define FTEXT_API __declspec(dllexport)
#else
#define FTEXT_API __declspec(dllimport)
#endif

#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <string>
#include <cstring>
#include <sstream>
#include <utility>
#include <limits.h>
#include <fstream>
#include <comdef.h>
#include <Wbemidl.h>
#include <vector>
#include <string>
#include <sysinfoapi.h>
#include <windows.h>
#include <thread>
#include <wincrypt.h>
#include <iterator>
#include <sstream>
#include <cstring>
#include <chrono>
#include <ctime>
#include <time.h>
#include <random>
#include <future>
#include <conio.h>
#include <shlobj.h> //SHLDialog


#include "bass\bass.h"
