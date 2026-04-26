
// TargetNumberGameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TargetNumberGame.h"
#include "TargetNumberGameDlg.h"
#include "afxdialogex.h"

#include "GameResultsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTargetNumberGameDlg::CTargetNumberGameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTargetNumberGameDlg::IDD, pParent), _game(GAME_TIME, &_prgTime, &_edtFormula, &_edtResult, &_btnDone, &_txtInputStatus, &_txtCpuStatus, &_lstPopulation, this),
	_closing(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTargetNumberGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_NUM1, _btnNum1);
	DDX_Control(pDX, IDC_BTN_NUM2, _btnNum2);
	DDX_Control(pDX, IDC_BTN_NUM3, _btnNum3);
	DDX_Control(pDX, IDC_BTN_NUM4, _btnNum4);
	DDX_Control(pDX, IDC_BTN_NUM5, _btnNum5);
	DDX_Control(pDX, IDC_BTN_NUM6, _btnNum6);
	DDX_Control(pDX, IDC_BTN_TARGET_NUM, _btnTargetNum);
	DDX_Control(pDX, IDC_PRG_TIME, _prgTime);
	DDX_Control(pDX, IDC_EDT_FORMULA, _edtFormula);
	DDX_Control(pDX, IDC_EDT_RESULT, _edtResult);
	DDX_Control(pDX, IDC_BTN_DONE, _btnDone);
	DDX_Control(pDX, IDC_TXT_SYNTAX, _txtInputStatus);
	DDX_Control(pDX, IDC_TXT_CPU_STATUS, _txtCpuStatus);
	DDX_Control(pDX, IDC_LST_POPULATION, _lstPopulation);
	DDX_Control(pDX, IDC_SHOW_CPU, _lbShowSolutions);
}

BEGIN_MESSAGE_MAP(CTargetNumberGameDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BTN_NUM1, &CTargetNumberGameDlg::OnBnClickedBtnNum1)
ON_BN_CLICKED(IDC_BTN_NUM2, &CTargetNumberGameDlg::OnBnClickedBtnNum2)
ON_BN_CLICKED(IDC_BTN_NUM3, &CTargetNumberGameDlg::OnBnClickedBtnNum3)
ON_BN_CLICKED(IDC_BTN_NUM4, &CTargetNumberGameDlg::OnBnClickedBtnNum4)
ON_BN_CLICKED(IDC_BTN_NUM5, &CTargetNumberGameDlg::OnBnClickedBtnNum5)
ON_BN_CLICKED(IDC_BTN_NUM6, &CTargetNumberGameDlg::OnBnClickedBtnNum6)
ON_BN_CLICKED(IDC_BTN_TARGET_NUM, &CTargetNumberGameDlg::OnBnClickedBtnTargetNum)
ON_EN_CHANGE(IDC_EDT_FORMULA, &CTargetNumberGameDlg::OnEnChangeEdtFormula)
ON_BN_CLICKED(IDC_BTN_DONE, &CTargetNumberGameDlg::OnBnClickedBtnDone)
ON_MESSAGE(WM_GAME_FINISHED, &CTargetNumberGameDlg::OnShowResults)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_SHOW_CPU, &CTargetNumberGameDlg::OnBnClickedShowCpu)
END_MESSAGE_MAP()


// CTargetNumberGameDlg message handlers

BOOL CTargetNumberGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	OnBnClickedShowCpu();
	_lstPopulation.ShowWindow( FALSE );

	_lstPopulation.InsertColumn( 0, _T( "Chromosome" ), 0, 240 );
	_lstPopulation.InsertColumn( 1, _T( "Fitness" ), 0, 47 );

	_game.GetNumberGenerator().BindButton( &_btnNum1, 0 );
	_game.GetNumberGenerator().BindButton( &_btnNum2, 1 );
	_game.GetNumberGenerator().BindButton( &_btnNum3, 2 );
	_game.GetNumberGenerator().BindButton( &_btnNum4, 3 );
	_game.GetNumberGenerator().BindButton( &_btnNum5, 4 );
	_game.GetNumberGenerator().BindButton( &_btnNum6, 5 );
	_game.GetNumberGenerator().BindButton( &_btnTargetNum, 6 );

	_game.StartGame();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTargetNumberGameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTargetNumberGameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTargetNumberGameDlg::OnBnClickedBtnNum1() { _game.GetNumberGenerator().NextUser(); }

void CTargetNumberGameDlg::OnBnClickedBtnNum2() { _game.GetNumberGenerator().NextUser(); }

void CTargetNumberGameDlg::OnBnClickedBtnNum3() { _game.GetNumberGenerator().NextUser(); }

void CTargetNumberGameDlg::OnBnClickedBtnNum4() { _game.GetNumberGenerator().NextUser(); }

void CTargetNumberGameDlg::OnBnClickedBtnNum5() { _game.GetNumberGenerator().NextUser(); }

void CTargetNumberGameDlg::OnBnClickedBtnNum6() { _game.GetNumberGenerator().NextUser(); }

void CTargetNumberGameDlg::OnBnClickedBtnTargetNum() { _game.GetNumberGenerator().NextUser(); }

void CTargetNumberGameDlg::OnEnChangeEdtFormula() { _game.GetInputManager().CheckInput( _game.GetNumberGenerator().GetGenerated(), Game::NumberGenerator::NUMBER_COUNT ); }

void CTargetNumberGameDlg::OnBnClickedBtnDone() { _game.ReportReady( true ); }

LRESULT CTargetNumberGameDlg::OnShowResults(WPARAM wParam, LPARAM lParam)
{
	if( !_closing )
	{
		GameResultsDialog dlg( _game.GetGameResult() , this );
		if( dlg.DoModal() == IDOK )
			_game.StartGame();
		else
			DestroyWindow();
	}
	else
		DestroyWindow();

	return 0;
}

DWORD WINAPI ClosingThread(LPVOID param)
{
	( (Game::Master*)param )->StopGame();
	return 0;
}

void CTargetNumberGameDlg::OnClose()
{
	if( !_closing )
	{
		_closing = true;
		::CloseHandle( ::CreateThread( NULL, 0, ClosingThread, &_game, 0, NULL) );

		return;
	}

	CDialog::OnClose();
}


void CTargetNumberGameDlg::OnBnClickedShowCpu()
{
	_lstPopulation.ShowWindow( !_lstPopulation.IsWindowVisible() );

	MoveControl( &_txtCpuStatus );
	MoveControl( &_lbShowSolutions );
	ResizeControl( &_prgTime );
	ResizeControl( this );
}


void CTargetNumberGameDlg::MoveControl(CWnd* control)
{
	RECT wndRect;
	control->GetWindowRect( &wndRect );

	if( control->GetParent() )
		ScreenToClient( &wndRect );

	RECT lstRect;
	_lstPopulation.GetWindowRect( &lstRect );
	LONG size = lstRect.bottom - lstRect.top;

	if( _lstPopulation.IsWindowVisible() )
	{
		wndRect.top += size;
		wndRect.bottom += size;
	}
	else
	{
		wndRect.top -= size;
		wndRect.bottom -= size;
	}

	control->MoveWindow( &wndRect );
}


void CTargetNumberGameDlg::ResizeControl(CWnd* control)
{
	RECT wndRect;
	control->GetWindowRect( &wndRect );

	if( control->GetParent() )
		ScreenToClient( &wndRect );

	RECT lstRect;
	_lstPopulation.GetWindowRect( &lstRect );
	LONG size = lstRect.bottom - lstRect.top;

	if( _lstPopulation.IsWindowVisible() )
		wndRect.bottom += size;
	else
		wndRect.bottom -= size;

	control->MoveWindow( &wndRect );
}
