/*!
 * @file GetTemporaryFile.h
 * @brief テンポラリファイル名取得
 * @date 2025
 */
#include "GetTemporaryFile.h"
#include <string>

int GetTemporaryFile(const wchar_t* _p_pre, const size_t _size, wchar_t* _p_file)
{
	DWORD dwRet = ::GetTempPathW(static_cast<DWORD>(_size), reinterpret_cast<LPWSTR>(_p_file));
	if (dwRet == 0)
	{
		return 1;
	}
	if (dwRet > _size)
	{
		return 2;
	}
	std::wstring wsDir = _p_file;
    UINT uRet = ::GetTempFileNameW(reinterpret_cast<LPCWSTR>(wsDir.c_str()), reinterpret_cast<LPCWSTR>(_p_pre), 0, reinterpret_cast<LPWSTR>(_p_file));
	if (uRet == 0)
	{
		return 3;
	}
	return 0;
}
