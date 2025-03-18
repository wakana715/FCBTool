/*!
 * @file ReadLine.cpp
 * @brief 行単位のファイル読込
 * @date 2025
 */
#include "ReadLine.h"
#include <sstream>

#define READ_LINE_BUF_SIZE (64)
static std::string s_str;

bool ReadLine(HANDLE _handle, LONGLONG* _p_pos, LONGLONG _limit, char** _pp_read, int *_p_size)
{
	byte buf[READ_LINE_BUF_SIZE]{};
	LARGE_INTEGER	liPointer;
	LONGLONG llPos = *_p_pos;
	liPointer.QuadPart = llPos;
	(void)::SetFilePointerEx(_handle, liPointer, NULL, FILE_BEGIN);
	LONGLONG llCount = 0;
	DWORD dwRead = 0;
	bool isFind = false;
	byte bPrev, bChr;
	std::stringstream ss;
	ss.clear();
	do
	{
		if (::ReadFile(_handle, buf, READ_LINE_BUF_SIZE, &dwRead, NULL) == 0)
		{
			dwRead = 0;
			break;
		}
		for (int i = 0; i < static_cast<int>(dwRead); i++)
		{
			bChr = buf[i];
			if (isFind)
			{
				if (bPrev == 0x0d && bChr == 0x0a)
				{
					llPos++;
				}
				dwRead = 0;
				break;
			}
			else
			{
				if (bChr != 0x0d && bChr != 0x0a)
				{
					if (_pp_read != nullptr)
					{
						ss << bChr;
					}
					llPos++;
				}
				else
				{
					bPrev = bChr;
					isFind = true;
					llPos++;
				}
			}
		}
		llCount += dwRead;
		if (llCount > _limit)
		{
			dwRead = 0;
			break;
		}
	} while (dwRead != 0);
	*_p_pos = llPos;
	if (_pp_read != nullptr)
	{
		s_str = ss.str();
		*_pp_read = &s_str[0];
	}
	if (_p_size != nullptr)
	{
		*_p_size = static_cast<int>(ss.str().size());
	}
	return isFind;
}
