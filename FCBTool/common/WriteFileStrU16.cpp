/*!
 * @file WriteFileStrU16.cpp
 * @brief ファイル書込(UTF16文字列)
 * @date 2025
 */
#include "WriteFileStrU16.h"
#include <windows.h>
#include <string.h>

int WriteFileStrU16(const wchar_t* _p_file, const size_t _size, const wchar_t* _p_str)
{
	HANDLE handle = ::CreateFileW(_p_file, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	size_t max = _size / sizeof(wchar_t);
	size_t len = wcsnlen_s(_p_str, max);
	if (len == max)
	{
		(void)::CloseHandle(handle);
		return 2;
	}
	size_t size = len * sizeof(wchar_t);
	if (WriteFile(handle, reinterpret_cast<LPCVOID>(_p_str), static_cast<DWORD>(size), NULL, NULL) == 0)
	{
		(void)::CloseHandle(handle);
		return 3;
	}
	(void)::CloseHandle(handle);
	return 0;
}
