/*!
 * @file ReadFileInt.cpp
 * @brief ファイル読込
 * @date 2025
 */
#include "ReadFileInt.h"
#include <windows.h>
#include <ctype.h>
#include <sstream>

int ReadFileInt(const wchar_t* _p_file, int* _p_num)
{
	HANDLE handle = ::CreateFileW(_p_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	char aryRead[5]{};
	if (ReadFile(handle, aryRead, 4, NULL, NULL) == 0)
	{
		return 2;
	}
	if (!isxdigit(aryRead[0]) || !isxdigit(aryRead[1]) || !isxdigit(aryRead[2]) || !isxdigit(aryRead[3]))
	{
		return 3;
	}
	int num;
	std::stringstream ss;
	ss << aryRead;
	ss >> std::hex >> num;
	*_p_num = num;
	(void)::CloseHandle(handle);
	return 0;
}
