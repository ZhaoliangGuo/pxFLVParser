#ifndef _PX_COMMON_DEF_H
#define _PX_COMMON_DEF_H

#include <map>
#include <vector>
#include <fstream>
using namespace std;

#include <afxdisp.h>
#include "tlhelp32.h"
#include <afxole.h>

#include "pxLogFile.h"
#include "pxReportCtrl.h"

#include "pxFLVCommonfDef.h"

extern CLogFile g_logFile;
extern CString  g_strMsg;

extern CString  g_strConfFile;

extern CString  g_strAppTitle;
extern HWND     g_hAppWnd;         // 主界面句柄

const int WM_ADD_TAG_TO_LIST                    = WM_USER + 1001; // 添加日志到运行日志

#endif