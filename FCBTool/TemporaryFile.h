/*!
 * @file TemporaryFile.h
 * @brief テンポラリファイル名取得
 * @date 2025
 */
#pragma once
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief テンポラリファイル名取得
 * @param[in]	_p_prefix	テンポラリファイルのプレフィックス(3文字まで)
 * @param[out]	_p_file		テンポラリファイル名(MAX_PATH文字分メモリ確保)
 * @return 0:成功, 1:GetTempPath失敗, 2:GetTempFileName失敗
 */
extern int GetTemporaryFile(const wchar_t* _p_prefix, wchar_t* _p_file);

#ifdef __cplusplus
}           /* closing brace for extern "C" */
#endif
