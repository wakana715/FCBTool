/*!
 * @file ReadFileInt.h
 * @brief ファイル読込(数値)
 * @date 2025
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief ファイル読込(数値)
 * @param[in]	_p_file	ファイル名
 * @param[out]	_p_num	数値
 * @return 0:成功, 1:CreateFileW失敗, 2:ReadFile失敗, 3:異常値
 */
extern int ReadFileInt(const wchar_t* _p_file, int* _p_num);

#ifdef __cplusplus
}
#endif
