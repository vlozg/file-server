// CMFCSignUpDlg.cpp : implementation file
//

#include "pch.h"
#include "MFCCLient.h"
#include "CMFCSignUpDlg.h"
#include "afxdialogex.h"

#include "CMFCSignInDlg.h"

// CMFCSignUpDlg dialog

IMPLEMENT_DYNAMIC(CMFCSignUpDlg, CDialogEx)

CMFCSignUpDlg::CMFCSignUpDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIGNUP_DIALOG, pParent)
	, Username(_T(""))
	, Password(_T(""))
	, conPassword(_T(""))
{

}

CMFCSignUpDlg::~CMFCSignUpDlg()
{
}

void CMFCSignUpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERNAMESU_EDIT, Username);
	DDX_Text(pDX, IDC_PASSWORDSU_EDIT, Password);

	DDX_Text(pDX, IDC_CONPASSWORD_EDIT, conPassword);
}


BEGIN_MESSAGE_MAP(CMFCSignUpDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SIGNUPRET_BUTTON, &CMFCSignUpDlg::OnBnClickedSignupretButton)
	ON_BN_CLICKED(IDC_SIGNUP1_BUTTON, &CMFCSignUpDlg::OnBnClickedSignup1Button)
END_MESSAGE_MAP()


// CMFCSignUpDlg message handlers


void CMFCSignUpDlg::OnBnClickedSignupretButton()
{
	// TODO: Add your control notification handler code here
	CMFCSignInDlg newDlg;
	this->~CMFCSignUpDlg();
	newDlg.DoModal();
}

//check Name and Password
bool checkValidName(CString s) {
	//username can't be empty
	if (s.IsEmpty()) {
		return false;
	}

	int isNum = 0, isChar = 0;					// variable count numbers and charaters
	for (int i = 0; i < s.GetLength(); i++) {
		if (s[i] >= 48 && s[i] <= 57) {			//0 -> 9
			++isNum;
		}
		else if ((s[i] >= 65 && s[i] <= 90) || (s[i] >= 97 && s[i] <= 122)) {		//a->z, A->Z
			++isChar;
		}
		else {
			return false;						// is special charater
		}
	}
	return (isNum > 0 && isChar > 0);			//username must be have both number and character
}


void CMFCSignUpDlg::OnOK() {
	OnBnClickedSignup1Button();
}

void CMFCSignUpDlg::OnBnClickedSignup1Button()
{
	//get data from boxees
	UpdateData(TRUE);
	//check string
	if (!checkValidName(Username)||!checkValidName(Password)) {
		MessageBox(_T("Username and Password must have digit, letters and no special character"));
		return;
	}
	if (Password.Compare(conPassword) != 0) {
		MessageBox(_T("Those password does not match"));
		return;
	}

	//handle signup
	string str_username(CW2A(Username.GetString()));
	string str_password(CW2A(Password.GetString()));
	client.SetUsername(str_username);
	client.SetPassword(str_password);
	if (!client.SignUp()) {
		MessageBox(
		(LPCWSTR)L"Username is existed!!\n        Try again!!",
			(LPCWSTR)L"Notification",
			MB_ICONWARNING);
		return;
	}


	//return to signin menu
	MessageBox(_T("Sign Up Success!!"));
	CMFCSignInDlg newDlg;
	this->~CMFCSignUpDlg();
	newDlg.DoModal();
}
