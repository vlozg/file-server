
// CMFCFirst.cpp : implementation file
//
#include "pch.h"
#include "framework.h"
#include "MFCCLient.h"
#include "CMFCFirst.h"
#include "afxdialogex.h"

#include "CMFCSignInDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCFirst dialog



CMFCFirst::CMFCFirst(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCCLIENT_DIALOG, pParent)
	, v_port(_T(""))
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCFirst::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, ipCtrl);
	DDX_Text(pDX, IDC_PORT, v_port);
}


BEGIN_MESSAGE_MAP(CMFCFirst, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMFCFirst::OnBnClickedButtonConnect)
END_MESSAGE_MAP()


// CMFCFirst message handlers

BOOL CMFCFirst::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCFirst::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCFirst::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCFirst::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

string convertBYTEtoIPString(BYTE IP[4]) {
	string IPAddress;
	for (int i = 3; i>= 0; i--) {
		string temp;
		if (IP[i] == 0) {
			IPAddress.insert(IPAddress.begin(), '0');
		}
		else {
			while (IP[i] > 0) {
				IPAddress.insert(IPAddress.begin(), IP[i] % 10 + 48);
				IP[i] = IP[i] / 10;
			}
		}
		IPAddress.insert(IPAddress.begin(), '.');
	}
	IPAddress.erase(IPAddress.begin());
	return IPAddress;
}


void CMFCFirst::OnBnClickedButtonConnect()
{
	//get IP from Dialog
	BYTE octet[4];
	ipCtrl.GetAddress(octet[0], octet[1], octet[2], octet[3]);
	string IPAddress = convertBYTEtoIPString(octet);
	
	//get port from dialog	
	UpdateData(TRUE);
	int port = _ttoi(v_port);
	
	//Handle connect to server
	client.Create(port);
	if (!client.Connect(IPAddress, port)) {
		int msgboxID = MessageBox(
			(LPCWSTR)L"Server is not currently available or wrong Server's IP!!!\n",
			(LPCWSTR)L"Notification",
			MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
			);

		switch (msgboxID)
		{
		//Exit 
		case IDCANCEL:
			OnCancel();
			this->~CMFCFirst();
			break;
		//reConnect
		case IDTRYAGAIN:
			OnBnClickedButtonConnect();
			return;
		//close message Box
		case IDCONTINUE:
			return;
		}
	}


	//Exchange to Sign In dialog
	CMFCSignInDlg newDlg;
	this->OnCancel();
	newDlg.DoModal();
}

void CMFCFirst::OnOK() {
	OnBnClickedButtonConnect();
}