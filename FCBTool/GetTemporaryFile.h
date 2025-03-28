/*!
 * @file GetTemporaryFile.h
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
 * @param[in]	_p_pre	テンポラリファイルのプレフィックス(3文字まで有効)
 * @param[in]	_size	ファイル名のバイト数
 * @param[out]	_p_file	ファイル名
 * @return 0:成功, 1:GetTempPath失敗, 2:メモリ(_size)不足, 3:GetTempFileName失敗
 */
extern int GetTemporaryFile(const wchar_t* _p_pre, const size_t _size, wchar_t* _p_file);

#ifdef __cplusplus
}
#endif
