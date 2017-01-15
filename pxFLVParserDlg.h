
// pxFLVParserDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "pxHexEditCtrl.h"
#include "afxeditbrowsectrl.h"
#include "afxcmn.h"
#include "pxCommonDef.h"


// CpxFLVParserDlg �Ի���
class CPxFLVParserDlg : public CDialogEx
{
// ����
public:
	CPxFLVParserDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CPxFLVParserDlg();

// �Ի�������
	enum { IDD = IDD_PXFLVPARSER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	CHexEdit m_editHex;
	CMFCEditBrowseCtrl m_editFLVPath;
	afx_msg void OnBnClickedButtonStartAnalyze();
	void Init();
	void SaveConfig();
	LRESULT AddTag2ListCtrl( WPARAM wParam, LPARAM lParam );
	CReportCtrl m_lcPackage;
	bool m_bStop;
	CRITICAL_SECTION  m_csListCtrl; 

	unsigned int m_uiLastVideoTimestamp;
	unsigned int m_uiLastAudioTimestamp;
	afx_msg void OnBnClickedButtonTest();
	void TestTag2ListCtrl();
	afx_msg void OnBnClickedCheckShowVideoInfo();
	afx_msg void OnBnClickedCheckShowAudioInfo();

	bool m_bShowVideo;
	bool m_bShowAudio;
	afx_msg void OnBnClickedButtonStopAnalyze();
};
