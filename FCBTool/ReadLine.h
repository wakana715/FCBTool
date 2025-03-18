/*!
 * @file ReadLine.h
 * @brief 行単位のファイル読込
 * @date 2025
 */
#pragma once
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief ファイルから１行読込する
 * @param[in]		_handle		ファイルハンドル
 * @param[in,out]	_p_pos		読込位置
 * @param[in]		_limit		読込最大サイズ
 * @param[out]		_pp_read	読込データ(改行含まない)
 * @param[out]		_p_size		読込データサイズ
 * @return 処理結果 1:１行読込した、2:EOF
 */
extern bool ReadLine(HANDLE _handle, LONGLONG* _p_pos, LONGLONG _limit, char** _pp_read, int *_p_size);

#ifdef __cplusplus
}           /* closing brace for extern "C" */
#endif
