#pragma once

#include "game\Game.h"

// GameResultsDialog dialog

class GameResultsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(GameResultsDialog)

private:

	Game::Results _results;

public:
	GameResultsDialog(const Game::Results& results,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~GameResultsDialog();

// Dialog Data
	enum { IDD = IDD_RESULTS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	int _edtTarget;
	int _edtReported;
	CString _edtEval;
public:
	CString _edtCpuExpress;
private:
	int _edtCpuResult;
	CString _userMiss;
	int _edtCpuMiss;
	CString _edtWinner;
};
