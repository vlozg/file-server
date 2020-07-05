#pragma once


// CMFCSignUpDlg dialog

class CMFCSignUpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMFCSignUpDlg)

public:
	CMFCSignUpDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMFCSignUpDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIGNUP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSignupretButton();
	afx_msg void OnBnClickedSignup1Button();
	CString Username;
	CString Password;
	CString conPassword;
	afx_msg void OnOK();
};
