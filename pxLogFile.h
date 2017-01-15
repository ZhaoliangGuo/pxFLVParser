/*********************************************************************
Author      : Zhaoliang Guo      
Date        : May. 20th 2014
Description : ����д��־����.    
***********************************************************************/
#ifndef _PX_LOG_H
#define _PX_LOG_H

#include <windows.h>
#include <atltime.h>
#include <sys/timeb.h>
#include <atlstr.h>
#include <iostream>
#include <stdarg.h>
using namespace std;

#define LOG_BUF_SIZE 32
#define WRITE_LOG_STATE 1// �Ƿ�����д��־ 1��д��־, 0����д��־

BOOL CreateDir(LPCTSTR pszPath);
BOOL IsDirExists(LPCTSTR pszPath);


class CLogFile
{
public:
	CLogFile();
	~CLogFile();

public:
	void WriteLogInfo(const char* kszFormat, ...); // �ɱ������ʽд��־
	void WriteLog(CString in_strText, const char *in_szFile, const unsigned long in_ulLine);         // ��CString��������д����־
	void GetCurTime();                         // ��ȡ��ǰʱ��

private:
	enum { MAX_BUF_LENGTH = 1024 };     // ������
	char m_szBuf[MAX_BUF_LENGTH];       // ���ڱ���ɱ����
	CRITICAL_SECTION  m_cs;             // �ٽ���
	CString m_strLogPath;               // ��־Ŀ¼
	char    m_szLogFileName[_MAX_PATH]; // ��ǰ��־�ļ�����
	char    m_szCurTime[LOG_BUF_SIZE];      // ��ǰʱ��
};


#endif