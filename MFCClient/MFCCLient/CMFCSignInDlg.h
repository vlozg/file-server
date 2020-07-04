#pragma once


// CMFCSignInDlg dialog

class CMFCSignInDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMFCSignInDlg)

public:
	CMFCSignInDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMFCSignInDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIGNIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString Username;									
	CString Password;
	afx_msg void OnBnClickedSignupButton();
	afx_msg void OnBnClickedSigninButton();
};
