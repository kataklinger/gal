// GameResultsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TargetNumberGame.h"
#include "GameResultsDialog.h"
#include "afxdialogex.h"


// GameResultsDialog dialog

IMPLEMENT_DYNAMIC(GameResultsDialog, CDialogEx)

GameResultsDialog::GameResultsDialog(const Game::Results& results,
	CWnd* pParent /*=NULL*/)
	: CDialogEx(GameResultsDialog::IDD, pParent)
	, _results(results)
	, _edtTarget(0)
	, _edtReported(0)
	, _edtEval(_T(""))
	, _edtCpuExpress(_T(""))
	, _edtCpuResult(0)
	, _userMiss(_T(""))
	, _edtCpuMiss(0)
	, _edtWinner(_T(""))
{

}

GameResultsDialog::~GameResultsDialog()
{
}

void GameResultsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_TARGET, _edtTarget);
	DDX_Text(pDX, IDC_EDT_USER_REPORTED, _edtReported);
	DDX_Text(pDX, IDC_EDT_EPRESSION_VALUE, _edtEval);
	DDX_Text(pDX, IDC_EDT_CPU_EXPRESSION, _edtCpuExpress);
	DDX_Text(pDX, IDC_EDT_CPU_RESULT, _edtCpuResult);
	DDX_Text(pDX, IDC_EDT_USER_MISS, _userMiss);
	DDX_Text(pDX, IDC_EDT_CPU_MISS, _edtCpuMiss);
	DDX_Text(pDX, IDC_EDT_WINNER, _edtWinner);
}


BEGIN_MESSAGE_MAP(GameResultsDialog, CDialogEx)
END_MESSAGE_MAP()


// GameResultsDialog message handlers

BOOL GameResultsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_edtTarget = _results._target;
	_edtReported = _results._userReported;

	_results._inputCorrect ? _edtEval.Format( _T( "%d" ), _results._userResult ) : _edtEval = _T( "Error!" );

	_edtCpuExpress = _results._cpuFormula;
	_edtCpuResult = _results._cpuResult;

	int userMiss = abs( _results._userResult - _results._target );

	_results._inputCorrect 
		? ( _results._userResult == _results._userReported ? _userMiss.Format( _T( "%d" ), userMiss ) : _userMiss = _T( "Mismatch!" ) )
		: _userMiss = _T( "Error!" );

	_edtCpuMiss = abs( _results._cpuResult - _results._target );

	if( !_results._inputCorrect || _results._userReported != _results._userResult || _edtCpuMiss < userMiss || ( !_results._userTurn && _edtCpuMiss == userMiss ) )
		_edtWinner = _T( "CPU" );
	else
		_edtWinner = _T( "YOU" );

	UpdateData( FALSE );

	return TRUE;
}