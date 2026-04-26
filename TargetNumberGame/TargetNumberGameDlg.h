
// TargetNumberGameDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "MathEdit.h"

#include "parsing\Parsing.h"
#include "game\Game.h"

// CTargetNumberGameDlg dialog
class CTargetNumberGameDlg : public CDialog
{
private:
	Game::Master _game;
	bool _closing;

// Construction
public:
	CTargetNumberGameDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TARGETNUMBERGAME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CButton _btnNum1;
	CButton _btnNum2;
	CButton _btnNum3;
	CButton _btnNum4;
	CButton _btnNum5;
	CButton _btnNum6;
	CButton _btnTargetNum;
	CProgressCtrl _prgTime;
	CMathEdit _edtFormula;
	CEdit _edtResult;
	CButton _btnDone;
	CStatic _txtInputStatus;
	CStatic _txtCpuStatus;
	CListCtrl _lstPopulation;
public:
	afx_msg void OnBnClickedBtnNum1();
	afx_msg void OnBnClickedBtnNum2();
	afx_msg void OnBnClickedBtnNum3();
	afx_msg void OnBnClickedBtnNum4();
	afx_msg void OnBnClickedBtnNum5();
	afx_msg void OnBnClickedBtnNum6();
	afx_msg void OnBnClickedBtnTargetNum();
	afx_msg void OnEnChangeEdtFormula();
	afx_msg void OnBnClickedBtnDone();
	afx_msg LRESULT OnShowResults(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnBnClickedShowCpu();
	void MoveControl(CWnd* control);
	void ResizeControl(CWnd* control);
	CButton _lbShowSolutions;
};
