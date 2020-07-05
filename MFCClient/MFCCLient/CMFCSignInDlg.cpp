// CMFCSignInDlg.cpp : implementation file
//

#include "pch.h"
#include "MFCCLient.h"
#include "CMFCSignInDlg.h"
#include "afxdialogex.h"

#include "CMFCSignUpDlg.h"
#include "CMFCMainDlg.h"

// CMFCSignInDlg dialog

IMPLEMENT_DYNAMIC(CMFCSignInDlg, CDialogEx)

CMFCSignInDlg::CMFCSignInDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIGNIN_DIALOG, pParent)
	, Username(_T(""))
	, Password(_T(""))
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

CMFCSignInDlg::~CMFCSignInDlg()
{
}

void CMFCSignInDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERNAME_EDIT, Username);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, Password);
}


BEGIN_MESSAGE_MAP(CMFCSignInDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SIGNUP_BUTTON, &CMFCSignInDlg::OnBnClickedSignupButton)
	ON_BN_CLICKED(IDC_SIGNIN_BUTTON, &CMFCSignInDlg::OnBnClickedSigninButton)
END_MESSAGE_MAP()


// CMFCSignInDlg message handlers


void CMFCSignInDlg::OnBnClickedSignupButton()
{
	// TODO: Add your control notification handler code here
	CMFCSignUpDlg newDlg;
	this->~CMFCSignInDlg();
	newDlg.DoModal();
}

void CMFCSignInDlg::OnOK() {
	OnBnClickedSigninButton();
}


void CMFCSignInDlg::OnBnClickedSigninButton()
{
	//update data from textbox
	UpdateData(TRUE);

	//check empty username and password
	if (Username.IsEmpty()) {
		MessageBox(_T("Username is empty!!"));
		return;
	}
	if (Password.IsEmpty()) {
		MessageBox(_T("Password is empty!!"));
		return;
	}
	
	//handle sign in
	string str_username(CW2A(Username.GetString()));
	string str_password(CW2A(Password.GetString()));
	client.SetUsername(str_username);
	client.SetPassword(str_password);
	if (!client.SignIn()) {
		MessageBox(
		(LPCWSTR)L"Username or Password is incorrect!!\nTry again!!",
		(LPCWSTR)L"Notification",
		MB_ICONWARNING );
		return;
	}
	//notification listen thread;
	MessageBox(_T("Sign in Success!!"));
	CMFCMainDlg newDlg;
	thread NotiListen;
	NotiListen = thread(&Client::NotiHandle, &client, &newDlg);
	this->~CMFCSignInDlg();
	newDlg.DoModal();
}
