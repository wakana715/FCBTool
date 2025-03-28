/*!
 * @file ReadFileMap.cpp
 * @brief ファイル読込(マップ)
 * @date 2025
 */
#include "ReadFileMap.h"
#include <windows.h>
#include <sstream>

int ReadFileMap(const wchar_t* _p_file, const size_t _size, char* _p_buf, std::map<std::wstring, std::wstring>& _map)
{
	HANDLE h_prm = ::CreateFileW(_p_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (h_prm == INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	DWORD dwRead;
	_p_buf[0] = '\0';
	if (ReadFile(h_prm, _p_buf, static_cast<DWORD>(_size), &dwRead, NULL) == 0)
	{
		return 2;
	}
	(void)::CloseHandle(h_prm);
	_p_buf[dwRead] = '\0';
	if (_p_buf[0] != '\0')
	{
		std::wstringstream wss;
		wss << reinterpret_cast<wchar_t*>(_p_buf);
		std::wstring wsLine;
		do
		{
			std::getline(wss, wsLine);
			size_t lastPos = wsLine.length() - 1;
			if (wsLine.c_str()[lastPos] == L'\x0d')
			{
				wsLine = wsLine.substr(0, lastPos);
			}
			size_t findPos = wsLine.find(L'=', 0);
			if (findPos > 0)
			{
				std::wstring key = wsLine.substr(0, findPos);
				std::wstring val = wsLine.substr(findPos + 1);
				if (key.size() > 0)
				{
					_map[key] = val;
				}
			}
		}while(wsLine.length() > 0);
	}
	return 0;
}
