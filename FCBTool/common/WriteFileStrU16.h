/*!
 * @file WriteFileStrU16.h
 * @brief ファイル書込(UTF16文字列)
 * @date 2025
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief ファイル書込(UTF16文字列)
 * @param[in]	_p_file	ファイル名
 * @param[in]	_size	UTF16文字列のバイト数
 * @param[in]	_p_str	UTF16文字列
 * @return 0:成功, 1:CreateFileW失敗, 2:wcsnlen_s失敗, 3:WriteFile失敗
 */
extern int WriteFileStrU16(const wchar_t* _p_file, const size_t _size, const wchar_t* _p_str);

#ifdef __cplusplus
}
#endif
