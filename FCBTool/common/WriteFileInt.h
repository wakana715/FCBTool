/*!
 * @file WriteFileInt.h
 * @brief ファイル書込(数値)
 * @date 2025
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief ファイル書込(数値)
 * @param[in]	_p_file	ファイル名
 * @param[in]	_num	数値
 * @return 0:成功, 1:CreateFileW失敗, 2:WriteFile失敗
 */
extern int WriteFileInt(const wchar_t* _p_file, const int _num);

#ifdef __cplusplus
}
#endif
