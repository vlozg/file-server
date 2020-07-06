#pragma once


// CMFCMainDlg dialog

class CMFCMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMFCMainDlg)

public:
	CMFCMainDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMFCMainDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnDropFiles(HDROP hDropInfo);			//drop file to upload
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSignoutButton();			//control to notification list
	CListBox listFileCtrl;								//control to Files list
	CString v_filePath;									//file path for upload

	afx_msg void OnBnClickedDownloadButton();
	afx_msg void OnBnClickedBrowseButton();
	CListCtrl notiListCtrl;
	void addActivity(CString);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	thread NotiListen;
	virtual BOOL PreTranslateMessage(MSG* pMsg);		//disable ESC quit Program

	afx_msg void OnBnClickedUploadButton();
};
