/*!
 * @file TemporaryFile.h
 * @brief テンポラリファイル名取得
 * @date 2025
 */
#include "TemporaryFile.h"

int GetTemporaryFile(const wchar_t* _p_prefix, wchar_t* _p_file)
{
	wchar_t wsTempPath[MAX_PATH]{};
	DWORD dwRet = ::GetTempPathW(MAX_PATH, reinterpret_cast<LPWSTR>(wsTempPath));
	if (dwRet > MAX_PATH || dwRet == 0)
	{
		return 1;
	}
    UINT uRet = ::GetTempFileNameW(wsTempPath, _p_prefix, 0, reinterpret_cast<LPWSTR>(_p_file));
	if (uRet == 0)
	{
		return 2;
	}
	return 0;
}
