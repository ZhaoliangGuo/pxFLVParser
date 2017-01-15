
// pxFLVParserDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "pxHexEditCtrl.h"
#include "afxeditbrowsectrl.h"
#include "afxcmn.h"
#include "pxCommonDef.h"


// CpxFLVParserDlg 对话框
class CPxFLVParserDlg : public CDialogEx
{
// 构造
public:
	CPxFLVParserDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CPxFLVParserDlg();

// 对话框数据
	enum { IDD = IDD_PXFLVPARSER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
