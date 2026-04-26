// MathEdit.cpp : implementation file
//

#include "stdafx.h"
#include "TargetNumberGame.h"
#include "MathEdit.h"

// CMathEdit

IMPLEMENT_DYNAMIC(CMathEdit, CEdit)

BEGIN_MESSAGE_MAP(CMathEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CMathEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	static char allowed[] = "()+-*/ \b";
 	if( nChar >= '0' && nChar <= '9' || strchr( allowed, nChar ) != NULL )
		CEdit::OnChar(nChar, nRepCnt, nFlags);
}
