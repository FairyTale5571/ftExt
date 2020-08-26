// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"

extension* mod;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		mod = new extension();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		mod->close();
		break;
	}
	return TRUE;
}

extern "C" __declspec (dllexport) void __stdcall RVExtension(char* output, int outputSize, const char* function) {
	mod->callExtension(output, outputSize, function);
}