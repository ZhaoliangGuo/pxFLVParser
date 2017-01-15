#include "StdAfx.h"
#include "pxFileManager.h"
#include <fstream>
using namespace std;

__int64 GetFileLength(char *in_szFilePath)
{
	if (NULL == in_szFilePath)
	{
		CString strMsg("");
		strMsg.Format("CPxFLVParserDlg::GetFileLength in_szFilePath is NULL");
		AfxMessageBox(strMsg);

		return -1;
	}

	fstream oStreamFile;

	oStreamFile.open(in_szFilePath, ios_base::in);

	oStreamFile.seekg(0, ios_base::end);

	// 兼容文件超过2G时的处理
	__int64 size = *(__int64*) ( ((char*)&(oStreamFile.tellg())) +8);

	oStreamFile.close();

	return size;
}
