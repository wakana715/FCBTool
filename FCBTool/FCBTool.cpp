/*!
 * @file FCBTool.cpp
 * @brief FC /B で出力された差分を適用する
 * @date 2025
 */
#include "TemporaryFile.h"
#include "ReadLine.h"
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <map>

#define LONG_PRE_PATH	L"\\\\?\\"
#define LONG_MAX_PATH	(32767)
#define LONG_MAX_CMD	(LONG_MAX_PATH*2+100)
#define FCB_HEAD_LIMIT	(9+LONG_MAX_PATH+4+LONG_MAX_PATH+17+2)
#define FCB_DATA_LIMIT	(8+2+2+1+2+2)
#define BUF_SIZE		(1024*1024*256)

#if (LONG_MAX_PATH * 6 + LONG_MAX_CMD) > BUF_SIZE
#error "LONG_MAX_PATH > BUF_SIZE"
#endif

static byte	s_buf[BUF_SIZE]{};
static bool	s_stop = false;

/*!
 * @brief テンポラリファイル名取得
 * @param[out]	waFileName	テンポラリファイル名
 * @return true:成功, false:失敗
 */
bool GetTemporaryFileName(std::wstring& _wsFileName)
{
	int ret = GetTemporaryFile(L"FCB", reinterpret_cast<wchar_t*>(s_buf));
	if (ret != 0)
	{
		DWORD dwError = ::GetLastError();
		std::wstringstream wss;
		wss << L"Failed GetTemporaryFileName " << ret << L" ";
		wss << " 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(dwError);
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
		return false;
	}
	_wsFileName = reinterpret_cast<wchar_t*>(s_buf);
	return true;
}

/*!
 * @brief パラメータファイル読込
 * @param[in]	_p_prm	パラメータファイル名
 * @param[out]	_map	取得したパラメータ
 * @return なし
 */
void ReadParameter(const wchar_t* _p_prm, std::map<std::wstring, std::wstring>& _map)
{
	HANDLE h_prm = ::CreateFileW(_p_prm, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (h_prm == INVALID_HANDLE_VALUE)
	{
		std::wstringstream wss;
		wss << L"Failed CreateFile " << _p_prm;
		wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
		return;
	}
	DWORD dwRead;
	if (ReadFile(h_prm, s_buf, static_cast<DWORD>(sizeof(s_buf)), &dwRead, NULL) == 0)
	{
		std::wstringstream wss;
		wss << L"Failed ReadFile " << _p_prm;
		wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
	}
	(void)::CloseHandle(h_prm);
	s_buf[dwRead] = '\0';
	if (s_buf[0] != '\0')
	{
		std::wstringstream wss;
		wss << reinterpret_cast<wchar_t*>(s_buf);
		std::wstring wsLine;
		do
		{
			std::getline(wss, wsLine);
			size_t lastPos = wsLine.length() - 1;
			if (wsLine.c_str()[lastPos] == L'\x0d')
			{
				wsLine = wsLine.substr(0, lastPos);
			}
			size_t findPos = wsLine.find(L'=', 0);
			if (findPos > 0)
			{
				std::wstring key = wsLine.substr(0, findPos);
				std::wstring val = wsLine.substr(findPos + 1);
				_map[key] = val;
			}
		}while(wsLine.length() > 0);
	}
}

/*!
 * @brief ステータスファイル読込
 * @param[in]	_p_sts	ステータスファイル名
 * @param[out]	_p_sts	ステータス 0:NOP, 1:実行要求, 2:実行中, 3:中断要求
 * @return なし
 */
void ReadStatus(const wchar_t* _p_sts, short int* _p_status)
{
	HANDLE h_sts = ::CreateFileW(_p_sts, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (h_sts == INVALID_HANDLE_VALUE)
	{
		std::wstringstream wss;
		wss << L"Failed CreateFile " << _p_sts;
		wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
		return;
	}
	char aryRead[5]{};
	DWORD dwRead;
	if (ReadFile(h_sts, aryRead, 4, &dwRead, NULL) == 0)
	{
		std::wstringstream wss;
		wss << L"Failed ReadFile " << _p_sts;
		wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
	}
	short int status;
	std::stringstream ss;
	ss << aryRead;
	ss >> std::hex >> status;
	*_p_status = status;
	(void)::CloseHandle(h_sts);
}

/*!
 * @brief ステータスファイル更新
 * @param[in]	_p_sts	ステータスファイル名
 * @param[in]	_sts	ステータス 0:NOP, 1:実行要求, 2:実行中, 3:中断要求
 * @return なし
 */
void WriteStatus(const wchar_t* _p_sts, const short int _sts)
{
	HANDLE h_sts = ::CreateFileW(_p_sts, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_sts == INVALID_HANDLE_VALUE)
	{
		std::wstringstream wss;
		wss << L"Failed CreateFile " << _p_sts;
		wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
		return;
	}
	std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(4) << _sts;
	ss << "\r\n";
	if (WriteFile(h_sts, ss.str().c_str(), 6, NULL, NULL) == 0)
	{
		std::wstringstream wss;
		wss << L"Failed WriteFile " << _p_sts;
		wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
	}
	(void)::CloseHandle(h_sts);
}

/*!
 * @brief プログレスファイル更新
 * @param[in]	_p_prg	プログレスファイル名
 * @param[in]	_max	プログレス最大値
 * @param[in]	_val	プログレス現在値
 * @return なし
 */
void WriteProgress(const wchar_t* _p_prg, LONGLONG _max, const LONGLONG _val)
{
	HANDLE h_prg = ::CreateFileW(_p_prg, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_prg == INVALID_HANDLE_VALUE)
	{
		std::wstringstream wss;
		wss << L"Failed CreateFile " << _p_prg;
		wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
		return;
	}
	std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(8) << _max;
	ss << "\r\n";
	ss << std::hex << std::setfill('0') << std::setw(8) << _val;
	ss << "\r\n";
	if (WriteFile(h_prg, ss.str().c_str(), 20, NULL, NULL) == 0)
	{
		std::wstringstream wss;
		wss << L"Failed WriteFile " << _p_prg;
		wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
	}
	(void)::CloseHandle(h_prg);
}

/*!
 * @brief FC /B で出力したファイルを１行読込する
 * @param[in]		_handle		ファイルハンドル(FC /B で出力したファイル)
 * @param[in,out]	_p_pos		ファイル読込位置
 * @param[out]		_p_adr		アドレス部
 * @param[out]		_p_patch1	差分1(左)
 * @param[out]		_p_patch2	差分2(右)
 * @return 処理結果 0:未読込, 1:読込, 2:EOF
 */
static int read_fcb(HANDLE _handle, LONGLONG* _p_pos, LONGLONG* _p_adr, byte* _p_patch1, byte* _p_patch2)
{
	LONGLONG llPos = *_p_pos;
	char *p_read;
	int	size;
	int ret = 0;
	while (ret == 0)
	{
		if (ReadLine(_handle, &llPos, FCB_DATA_LIMIT, &p_read, &size) == false)
		{
			ret = 2;
			break;
		}
		if (size < 15)
		{
			continue;
		}
		for (int i = 0; i < 8; i++)
		{
			if (isxdigit(static_cast<int>(p_read[i])) == 0)
			{
				continue;
			}
		}
		if (p_read[8] != ':' || p_read[9] != ' ' || p_read[12] != ' ')
		{
			continue;
		}
		if (isxdigit(static_cast<byte>(p_read[10])) == 0 ||
			isxdigit(static_cast<byte>(p_read[11])) == 0 ||
			isxdigit(static_cast<byte>(p_read[13])) == 0 ||
			isxdigit(static_cast<byte>(p_read[14])) == 0)
		{
			continue;
		}
		std::stringstream ss;
		ss << p_read;
		LONGLONG llAdr;
		char chColon;
		unsigned int uPatch1, uPatch2;
		ss >> std::hex >> llAdr >> chColon >> std::hex >> uPatch1 >> std::hex >> uPatch2;
		*_p_adr = llAdr;
		*_p_patch1 = static_cast<byte>(uPatch1);
		*_p_patch2 = static_cast<byte>(uPatch2);
		ret = 1;
	}
	return ret;
}

/*!
 * @brief FC /B で出力された差分を適用する
 * @param[in]		_p_src		パッチ元ファイル名
 * @param[in]		_p_dst		パッチ先ファイル名
 * @param[in]		_p_fcb		FC /B の表示内容を記録したファイル名
 * @param[in]		_opt		FC /B の差異表示の左右のどちらを適用するかの区分
 *								1:左、2:右
 * @param[in]		_p_sts		ステータスファイル名
 * @param[in]		_p_prg		プログレスファイル名
例)
FC /B FILE1 FILE2 の実行結果
ファイル FILE1 と FILE2 を比較しています
0000038D: 2D 30
          <>    1:左
             <> 2:右
 */
static void FCBTool(const wchar_t* _p_src, const wchar_t* _p_dst, const wchar_t* _p_fcb, const int _opt, const wchar_t* _p_sts, const wchar_t* _p_prg)
{
	HANDLE h_src = ::CreateFileW(_p_src, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (h_src == INVALID_HANDLE_VALUE)
	{
		s_stop = true;
		return;
	}
	HANDLE h_dst = ::CreateFileW(_p_dst, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_dst == INVALID_HANDLE_VALUE)
	{
		s_stop = true;
		(void)::CloseHandle(h_src);
		return;
	}
	HANDLE h_fcb = ::CreateFileW(_p_fcb, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (h_fcb == INVALID_HANDLE_VALUE)
	{
		s_stop = true;
		(void)::CloseHandle(h_src);
		(void)::CloseHandle(h_dst);
		return;
	}
	LONGLONG llSize = 0;
	{
		LARGE_INTEGER	liSize;
		if (::GetFileSizeEx(h_src, &liSize) != 0)
		{
			llSize = liSize.QuadPart;
		}
	}
	LONGLONG llPos = 0;
	DWORD dwRead = 0;
	if (::ReadFile(h_fcb, s_buf, 8, &dwRead, NULL) != 0)
	{
		if (dwRead > 7 && memcmp(s_buf, "ファイル", 8) == 0)
		{
			if (ReadLine(h_fcb, &llPos, FCB_HEAD_LIMIT, nullptr, nullptr) == false)
			{
				llPos = 0;
			}
		}
	}
	LONGLONG llPatch;
	LONGLONG llAdr = 0;
	BYTE bLeft, bRight;
	int result_read_fcb = 0;
	WriteProgress(_p_prg, llSize, llAdr);
	do
	{
		short int status;
		ReadStatus(_p_sts, &status);
		if (status == 3)	// 3:中断要求
		{
			s_stop = true;
			break;
		}
		if (::ReadFile(h_src, s_buf, BUF_SIZE, &dwRead, NULL) == 0)
		{
			dwRead = 0;
		}
		if (dwRead > 0)
		{
			if (result_read_fcb == 0)
			{
				result_read_fcb = read_fcb(h_fcb, &llPos, &llPatch, &bLeft, &bRight);
			}
			if (result_read_fcb == 1 && llAdr <= llPatch && (llAdr + dwRead) > llPatch)
			{
				switch(_opt)
				{
				case 1:
					s_buf[llPatch - llAdr] = bLeft;
					break;
				case 2:
					s_buf[llPatch - llAdr] = bRight;
					break;
				default:
					break;
				}
				result_read_fcb = 0;
			}
			(void)::WriteFile(h_dst, s_buf, dwRead, NULL, NULL);
			llAdr += dwRead;
		}
		WriteProgress(_p_prg, llSize, llAdr);
	} while (dwRead != 0);
	(void)::CloseHandle(h_src);
	(void)::CloseHandle(h_dst);
	(void)::CloseHandle(h_fcb);
	WriteProgress(_p_prg, llSize, llSize);
}

/*!
 * @brief メイン
 * @return 0:成功, 1:失敗
 */
int APIENTRY wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	DWORD dwRet;
	// スクリプト(.ps1)のパスを取得する
	dwRet = ::GetModuleFileNameW(NULL, reinterpret_cast<LPWSTR>(s_buf), LONG_MAX_PATH);
	if (dwRet > LONG_MAX_PATH || dwRet == 0)
	{
		std::wstringstream wss;
		wss << L"Failed GetModuleFileName";
		wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
		(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}
	else
	{
		// 最後の \ を削除
		wchar_t* pwpos = wcsrchr(reinterpret_cast<wchar_t*>(s_buf), L'\\');
		if (pwpos != NULL)
		{
			pwpos[1] = L'\0';
		}
	}
	std::wstring wsModuleFileName = reinterpret_cast<wchar_t*>(s_buf);

	// テンポラリファイル名(パラメータ用)を取得する
	std::wstring wsTempFileParameter;
	if (GetTemporaryFileName(wsTempFileParameter) == false)
	{
		return 0;
	}
	// テンポラリファイル名(ステータス用)を取得する
	std::wstring wsTempFileStatus;
	if (GetTemporaryFileName(wsTempFileStatus) == false)
	{
		return 0;
	}
	// テンポラリファイル名(プログレス用)を取得する
	std::wstring wsTempFileProgress;
	if (GetTemporaryFileName(wsTempFileProgress) == false)
	{
		return 0;
	}

	// コマンドを作成する
	std::wstring wsCmd;
	{
		std::wstringstream wss;
		wss << L"powershell -NoProfile -ExecutionPolicy Unrestricted -WindowStyle Hidden \"";
		wss << wsModuleFileName;
		wss << L"FCBTool.ps1\" \"";
		wss << wsTempFileParameter;
		wss << L"\" \"";
		wss << wsTempFileStatus;
		wss << L"\" \"";
		wss << wsTempFileProgress;
		wss << L"\"";
		wsCmd = wss.str();
	}

	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	if (CreateProcessW(nullptr, &wsCmd[0], nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
	{
		do
		{
			dwRet = ::WaitForSingleObject(pi.hProcess, 500);
			if (dwRet == WAIT_TIMEOUT)
			{
				short int status;
				ReadStatus(wsTempFileStatus.c_str(), &status);
				if (status == 1)				// 1:実行要求
				{
					std::map<std::wstring, std::wstring> map;
					ReadParameter(wsTempFileParameter.c_str(), map);
					int opt;
					{
						std::wstringstream wss;
						wss << map[L"opt"];
						wss >> opt;
					}
					std::wstring wsSrc = map[L"src"];
					std::wstring wsDst = map[L"dst"];
					std::wstring wsFcb = map[L"fcb"];
					if (wsSrc.size() > MAX_PATH)
					{
						wsSrc = LONG_PRE_PATH + wsSrc;
					}
					if (wsDst.size() > MAX_PATH)
					{
						wsDst = LONG_PRE_PATH + wsDst;
					}
					if (wsFcb.size() > MAX_PATH)
					{
						wsFcb = LONG_PRE_PATH + wsFcb;
					}
					WriteStatus(wsTempFileStatus.c_str(), 2);		// 2:実行中
					FCBTool(wsSrc.c_str(), wsDst.c_str(), wsFcb.c_str(), opt, wsTempFileStatus.c_str(), wsTempFileProgress.c_str());
					WriteStatus(wsTempFileStatus.c_str(), 0);		// 0:NOP
				}
			}
		} while (dwRet != WAIT_OBJECT_0);	// プロセス終了まで繰り返し
	}
	else
	{
		{
			std::wstringstream wss;
			wss << L"FCBTool Failed CreateProcess " << wsCmd.c_str();
			wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
			wss << L"\x0d\x0a"; 
			::OutputDebugStringW(wss.str().c_str());
		}
		{
			std::wstringstream wss;
			wss << L"Failed CreateProcess";
			wss << L" 0x" << std::hex << std::setfill(L'0') << std::setw(4) << static_cast<long>(::GetLastError());
			(void)::MessageBoxW(NULL, wss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
		}
	}
	WriteStatus(wsTempFileStatus.c_str(), 0);		// 0:NOP
	(void)::DeleteFile(wsTempFileParameter.c_str());
	(void)::DeleteFile(wsTempFileStatus.c_str());
	(void)::DeleteFile(wsTempFileProgress.c_str());
	return 0;
}
