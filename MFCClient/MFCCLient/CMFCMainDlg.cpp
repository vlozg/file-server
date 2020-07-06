// CMFCMainDlg.cpp : implementation file
//

#include "pch.h"
#include "MFCCLient.h"
#include "CMFCMainDlg.h"
#include "afxdialogex.h"

#include "CMFCSignInDlg.h"

// CMFCMainDlg dialog

IMPLEMENT_DYNAMIC(CMFCMainDlg, CDialogEx)

CMFCMainDlg::CMFCMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_DIALOG, pParent)
	, v_filePath(_T(""))
{

  
}

CMFCMainDlg::~CMFCMainDlg()
{
}

void CMFCMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    //listControl.AddString(_T("thanhduy"));  // add new notification
    DDX_Control(pDX, IDC_FILE_LIST, listFileCtrl);
    DDX_Text(pDX, IDC_FILEPATH_EDIT, v_filePath);
    DDX_Control(pDX, IDC_LIST1, notiListCtrl);
}

BOOL CMFCMainDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

   // client.SetPrintNotiFunction();
    notiListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES);
    notiListCtrl.InsertColumn(0, _T("Activity"), LVCFMT_LEFT, 200);
    notiListCtrl.InsertColumn(1, _T("Time"), LVCFMT_LEFT, 210);

    // TODO: Add extra initialization here

    return TRUE;  // return TRUE  unless you set the focus to a control
}

//Drop file to Upload
void CMFCMainDlg::OnDropFiles(HDROP dropInfo)
{
    DWORD nBuffer = 0;
    UINT nFilesDropped = DragQueryFile(dropInfo, 0xFFFFFFFF, NULL, 0);
    if (nFilesDropped == 1)
    {
        nBuffer = DragQueryFile(dropInfo, 0, NULL, 0);
        // Get path and name of the first file
        DragQueryFile(dropInfo, 0, v_filePath.GetBuffer(nBuffer + 1), nBuffer + 1);
        v_filePath.ReleaseBuffer();
        // Do something with the path
        if (PathIsDirectory(v_filePath)) {
            MessageBox(_T("You can only upload a file!!"));
            return;
        }
    }
    else {
        MessageBox(_T("You can only upload a file!!"));
        return;
    }
    // Free the memory block containing the dropped-file information
    DragFinish(dropInfo);
    UpdateData(FALSE);
}


BEGIN_MESSAGE_MAP(CMFCMainDlg, CDialogEx)
    ON_BN_CLICKED(IDC_SIGNOUT_BUTTON, &CMFCMainDlg::OnBnClickedSignoutButton)
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_DOWNLOAD_BUTTON, &CMFCMainDlg::OnBnClickedDownloadButton)
    ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CMFCMainDlg::OnBnClickedBrowseButton)
    ON_BN_CLICKED(IDC_UPLOAD_BUTTON, &CMFCMainDlg::OnBnClickedUploadButton)
END_MESSAGE_MAP()


// CMFCMainDlg message handlers

void CMFCMainDlg::OnBnClickedSignoutButton()
{
	// TODO: Add your control notification handler code here
	//handle
    client.Disconnect();
	//sign out
    //NotiListen.join();
   
    //client.mainDlg = NULL;
	CMFCSignInDlg newDlg;
	this->~CMFCMainDlg();
	newDlg.DoModal();
}




void CMFCMainDlg::OnBnClickedDownloadButton()
{
    // TODO: Add your control notification handler code here
    CString folderPath;
    CFolderPickerDialog folderDlg;
    folderDlg.DoModal();
    folderPath.Empty();
    folderPath = folderDlg.GetPathName();

    //Handle Download


}

//Choose folder to load file
void CMFCMainDlg::OnBnClickedBrowseButton()
{
	// TODO: Add your control notification handler code here
    CFileDialog fileDlg(TRUE);
    fileDlg.DoModal();
    v_filePath = fileDlg.GetPathName();
    UpdateData(FALSE);
}


//method add activity of other users 
void CMFCMainDlg::addActivity(string noti) {
    CString noti_Cstr(noti.c_str());
    CTime time = CTime::GetCurrentTime();
    CString time_str = time.Format(_T("%d/%m/%Y - %I:%M %p"));
    notiListCtrl.InsertItem(0, noti_Cstr);
    notiListCtrl.SetItemText(0, 1, time_str);
}

//Override OnOK
void CMFCMainDlg::OnOK() {
    return;
}

//Override OnCancel
void CMFCMainDlg::OnCancel() {
        client.Disconnect();
      //  NotiListen.join();
        this->~CMFCMainDlg();
}


BOOL CMFCMainDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->hwnd == this->m_hWnd && pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        {
            return TRUE;                // Do not process further
        }
    }
    return CWnd::PreTranslateMessage(pMsg);
}

void CMFCMainDlg::OnBnClickedUploadButton()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);

    //check file path
    if (v_filePath.IsEmpty() || !PathFileExists(v_filePath)) {
        MessageBox(
        (LPCWSTR)L"Can't find file that you choose!!",
        (LPCWSTR)L"Error",
        MB_ICONERROR);
        return;
    }
    string filepath(CW2A(v_filePath.GetString()));
    client.UploadCall();
    client.SendFile(client.GetSocket().GetSock(), filepath);
    v_filePath.Empty();
    UpdateData(FALSE);
}
