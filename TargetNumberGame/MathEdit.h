#pragma once

// CMathEdit

class CMathEdit : public CEdit
{
	DECLARE_DYNAMIC(CMathEdit)

public:
	CMathEdit() { }
	virtual ~CMathEdit() { }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


