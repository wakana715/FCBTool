/*!
 * @file GetModuleDirectory.h
 * @brief モジュールファイル(exe)のディレクトリ名取得
 * @date 2025
 */
#pragma once
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief モジュールファイル(exe)のディレクトリ名取得
 * @param[in]	_size		ディレクトリ名のサイズ
 * @param[out]	_p_dir		ディレクトリ名
 * @return 0:成功, 1:GetModuleFileNameW失敗, 2:メモリ(_size)不足
 */
extern int GetModuleDirectory(const size_t _size, wchar_t* _p_dir);

#ifdef __cplusplus
}
#endif
