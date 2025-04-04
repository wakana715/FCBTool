/*!
 * @file ReadFileMap.h
 * @brief ファイル読込(マップ)
 * @date 2025
 */
#pragma once
#include <string>
#include <map>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief ファイル読込(マップ)
 * @param[in]	_p_file	ファイル名
 * @param[in]	_size	読込バッファのバイト数
 * @param[out]	_p_buf	読込バッファ
 * @param[out]	_map	マップ
 * @return 0:成功, 1:CreateFileW失敗, 2:ReadFile失敗
 */
extern int ReadFileMap(const wchar_t* _p_prm, const size_t _size, char* _p_buf, std::map<std::wstring, std::wstring>& _map);

#ifdef __cplusplus
}
#endif
