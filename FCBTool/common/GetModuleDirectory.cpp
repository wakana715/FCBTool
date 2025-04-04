/*!
 * @file GetModuleDirectory.cpp
 * @brief モジュールファイル(exe)のディレクトリ名取得
 * @date 2025
 */
#include "GetModuleDirectory.h"
#include <windows.h>

int GetModuleDirectory(const size_t _size, wchar_t* _p_dir)
{
	DWORD dwRet;
	DWORD dwLen = static_cast<DWORD>(_size / sizeof(wchar_t));
	dwRet = ::GetModuleFileNameW(NULL, reinterpret_cast<LPWSTR>(_p_dir), dwLen);
	if (dwRet == 0)
	{
		return 1;
	}
	if (dwRet >= dwLen)
	{
		return 2;
	}
	_p_dir[dwRet] = L'\0';
	// 最後の \ を削除
	wchar_t* pwpos = wcsrchr(_p_dir, L'\\');
	if (pwpos != NULL)
	{
		pwpos[0] = L'\0';
	}
	return 0;
}
