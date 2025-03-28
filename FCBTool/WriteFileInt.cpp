/*!
 * @file WriteFileInt.cpp
 * @brief ファイル書込(数値)
 * @date 2025
 */
#include "WriteFileInt.h"
#include <windows.h>
#include <sstream>
#include <iomanip>

int WriteFileInt(const wchar_t* _p_file, const int _num)
{
	HANDLE handle = ::CreateFileW(_p_file, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(4) << _num;
	if (WriteFile(handle, reinterpret_cast<LPCVOID>(ss.str().c_str()), 4, NULL, NULL) == 0)
	{
		(void)::CloseHandle(handle);
		return 2;
	}
	(void)::CloseHandle(handle);
	return 0;
}
