// FactoryToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FactoryTool.h"
#include "FactoryToolDlg.h"
#include <dbt.h>
#include <DbgHelp.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
cmLog *g_pLogObject=NULL;
CFactoryToolDlg *g_pMainDlg=NULL;
LONG WINAPI MyUnhandledExceptionFilter(_In_  struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	CFile dumpFile;
	BOOL bRet;
	CString strDumpPath;
	strDumpPath = cmPath::GetModulePath()+_T("tool_error.dmp");
	bRet = dumpFile.Open(strDumpPath,CFile::typeBinary|CFile::modeCreate|CFile::modeReadWrite);
	if (bRet)
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo;
		einfo.ThreadId = GetCurrentThreadId();
		einfo.ExceptionPointers = ExceptionInfo;
		einfo.ClientPointers = FALSE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dumpFile.m_hFile, MiniDumpNormal, &einfo, NULL, NULL);
		dumpFile.Close();
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
UINT ScanDeviceThread(LPVOID lpParam)
{
	CFactoryToolDlg *pDlg = (CFactoryToolDlg *)lpParam;
	pDlg->ScanThreadProc();
	
	return 0;
}
UINT UpgradeDeviceThread(LPVOID lpParam)
{
	if (g_pMainDlg)
	{
		g_pMainDlg->UpgradeThreadProc((int)lpParam);
	}
	return 0;
}
UINT LoadFwThread(LPVOID lpParam)
{
	CFactoryToolDlg *pDlg = (CFactoryToolDlg *)lpParam;
	pDlg->LoadFwProc();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CFactoryToolDlg dialog

CFactoryToolDlg::CFactoryToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFactoryToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFactoryToolDlg)
	m_nUpgradeRadio = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFactoryToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFactoryToolDlg)
	DDX_Control(pDX, IDC_TREE_DEVICEINFO, m_treeDeviceInfo);
	DDX_Control(pDX, IDC_STATIC_HELPTEXT, m_staticHelpText);
	DDX_Control(pDX, IDC_STATIC_LOADERVER, m_staticLoaderVer);

	DDX_Control(pDX, IDC_STATIC_FWVERSION, m_staticFwVersion);
	DDX_Control(pDX, IDC_STATIC_FWCHIP, m_staticFwChip);
	DDX_Control(pDX, IDC_STATIC_FIRMWARE, m_staticFirmware);
	DDX_Control(pDX, IDC_EDIT_DEMO, m_edtDemo);
	DDX_Control(pDX, IDC_STATIC_HELP, m_groupboxhelp);

	DDX_Control(pDX, IDC_EDIT_FIRMWARE, m_edtFirmware);
	DDX_Control(pDX, IDC_BUTTON_RUN, m_btnRun);
	DDX_Control(pDX, IDC_BUTTON_LANGUAGE, m_btnLanguage);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_btnExit);
	DDX_Control(pDX, IDC_BUTTON_DEMO, m_btnDemo);
	DDX_Control(pDX, IDC_BUTTON_FIRMWARE, m_btnFirmware);
	DDX_Radio(pDX, IDC_RADIO_UPGRADE, m_nUpgradeRadio);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OK_GRID, m_resultGrid);
	DDX_Control(pDX, IDC_STATIC_SUCCESS, m_staticSuccess);
	DDX_Control(pDX, IDC_STATIC_SUCCESS_COUNT, m_staticSuccessCount);
	DDX_Control(pDX, IDC_STATIC_FAIL, m_staticFail);
	DDX_Control(pDX, IDC_STATIC_FAIL_COUNT, m_staticFailCount);
	DDX_Control(pDX, IDC_STATIC_TOTAL, m_staticTotal);
	DDX_Control(pDX, IDC_STATIC_TOTAL_COUNT, m_staticTotalCount);
	DDX_Control(pDX, IDC_FAIL_GRID, m_failGrid);
	DDX_Control(pDX, IDC_CHECK_DEMO, m_checkDemo);
}

BEGIN_MESSAGE_MAP(CFactoryToolDlg, CDialog)
	//{{AFX_MSG_MAP(CFactoryToolDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_FIRMWARE, OnButtonFirmware)
	ON_BN_CLICKED(IDC_BUTTON_RUN, OnButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_DEMO, OnButtonDemo)
	ON_BN_CLICKED(IDC_BUTTON_LANGUAGE, OnButtonLanguage)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_MESSAGE(WM_DEVICECHANGE,OnDeviceChange)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_LOADFW_MSG,OnHandleLoadFwMsg)
	ON_MESSAGE(WM_UPDATE_GRID_MSG,OnHandleUpdateGridMsg)
	ON_COMMAND_RANGE(IDM_LANG_MENUITEM,IDM_LANG_MENUITEM+MAX_MENU_ITEM,OnLangSelect)
	ON_NOTIFY(VGN_GETDISPINFO, IDC_OK_GRID, OnGridGetDispInfo)
	ON_NOTIFY(VGN_SETDISPINFO, IDC_OK_GRID, OnGridSetDispInfo)
	ON_NOTIFY(VGN_GETDISPINFO, IDC_FAIL_GRID, OnGridGetDispInfo)
	ON_NOTIFY(VGN_SETDISPINFO, IDC_FAIL_GRID, OnGridSetDispInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFactoryToolDlg message handlers

BOOL CFactoryToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	SetUnhandledExceptionFilter(&MyUnhandledExceptionFilter);
	g_pMainDlg = this;
	m_hGreenLedBitmap = m_hRedLedBitmap = NULL;
	m_pLangObject = NULL;
	m_popMenu = NULL;
	m_pScan = NULL;
	m_pLog = NULL;
	m_pImage = NULL;
	m_curLang = 0;
	m_pScanThread = NULL;
	m_pScanEvent = NULL;
	m_bContinueScan = TRUE;
	m_treeItemMap.clear();
	m_deviceList.clear();
	m_hubList.clear();
	m_backupDev.clear();
	m_resultItems.clear();
	m_failItems.clear();
	m_dcf.vecDemo.clear();
	m_nScanDevice = 0;
	m_bRunFlag = FALSE;
	m_bMutexResetFlag = FALSE;
	m_bUidCreate = FALSE;
	m_bCheckFw = TRUE;
	m_bLoopTest = FALSE;
	m_nRestartTimeout = 5;
	//initialize path variables
	m_strModulePath = cmPath::GetModulePath();
	m_strLogPath = m_strModulePath + _T("Log\\");
	if ( !cmFile::IsExisted(m_strLogPath) )
	{
		CreateDirectory(m_strLogPath,NULL);
	}
	
	BOOL bRet;
	g_pLogObject = NULL;
	g_pLogObject = new cmLog(m_strLogPath,bRet);
	if (bRet)
	{
		g_pLogObject->LogFileName = _T("AppLog");
	}
	//load config and language text
	if (LoadConfig())
	{
		m_popMenu = new CMenu();
		m_popMenu->CreatePopupMenu();
		
		for (int i=0;i<m_langProp.size();i++)
		{
			m_popMenu->AppendMenu(MF_STRING,IDM_LANG_MENUITEM+i,_T(""));
		}
		m_pLangObject = new cmMultiLanguage(m_LangPath,m_langProp,g_pLogObject,bRet);
		if (bRet)
		{
			//set window interface
			m_pLangObject->CurrentLang = m_curLang;
			STRUCT_DIALOG_PROPERTY prop;
			prop.hwnd = m_hWnd;
			prop.id = IDD_FACTORYTOOL_DIALOG;
			m_pLangObject->AddDialogProp(prop);

			STRUCT_MENU_PROPERTY menuProp;
			menuProp.hwnd = m_popMenu->m_hMenu;
			menuProp.id = IDM_LANG_MENU;
			m_pLangObject->AddMenuProp(menuProp);
			
			bRet = m_pLangObject->SetLanguageString();
			if (bRet)
			{
				CString strTitle;
				GetWindowText(strTitle);
				strTitle = strTitle + APP_VERSION;
				SetWindowText(strTitle);
				InitLangString();
			}
			m_popMenu->CheckMenuItem(m_curLang,MF_BYPOSITION|MF_CHECKED);
		}
		else
		{
			MessageBox(_T("Loading Multi Language failed!"),_T("ERROR"),MB_ICONERROR|MB_OK);
		}
		//create scan and log object
		m_pScan = new CRKScan(m_mscTimeout,m_rockusbTimeout,m_bSupportFullUsb);
		if (m_pScan)
		{
			m_pScan->SetVidPid(m_mscVid,m_mscPid);
			CRKLog *pLogObject;
			pLogObject = new CRKLog(m_strLogPath,_T("ScanLog"));
			m_pScan->SetLogObject(pLogObject);
			m_pLog = new CRKLog(m_strLogPath,_T("Log"));
			m_pScanThread = AfxBeginThread(ScanDeviceThread,(LPVOID)this);
			m_pScan->Search(RKUSB_MASKROM|RKUSB_LOADER|RKUSB_MSC|RKUSB_ADB);
		}
		else
		{
			if (g_pLogObject)
			{
				g_pLogObject->Record(_T("Error:Create Scan Object failed!"));
			}
		}
		//test if uid can crate or not
		GUID guidTmp;
		HRESULT hr;
		hr = CoCreateGuid(&guidTmp);
		if (hr!=S_OK)
		{
			CString strInfoText;
			GetLocalString(_T("IDS_CREATEUIDFAIL"));
			if (g_pLogObject)
			{
				g_pLogObject->Record(_T("Warning:%s"),strInfoText);
			}
		}
		else
			m_bUidCreate = TRUE;
	}
	else
	{
		MessageBox(_T("Loading config file failed!"),_T("ERROR"),MB_ICONERROR|MB_OK);
	}
	if (m_bSnDllOn)
	{
		if (!LoadSnRes())
		{
			m_bSnDllOn = FALSE;
			MessageBox(_T("Loading sn.dat failed!"),_T("ERROR"),MB_ICONERROR|MB_OK);
		}
	}
	else
	{
		m_pSnFileData = NULL;
	}
	if (m_pLog)
	{
		CString strVer;
		if (!GetExeVersion(strVer))
		{
			strVer = APP_VERSION;
		}
		m_pLog->Record(_T("Factory Tool")+strVer+_T(" start run"));
	}
	m_bRedLedLight = TRUE;
 	m_hRedLedBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_REDLED));
	m_hGreenLedBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_GREENLED));
	
	m_nUpgradeRadio = 0;
	UpdateData(FALSE);
	m_font.CreateFont(-13,6,0,0,FW_NORMAL,FALSE,FALSE,0,  
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,  
	DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,_T("宋体"));
	LoadLayerMap(m_layerMap);

	InitRunContext();
	InitToolButton();
	InitStatic();
	InitGroupbox();
	InitTree();
	InitGrid();
	if (cmFile::IsExisted(m_strDefaultDemo))
	{
		m_edtDemo.SetWindowText(m_strDefaultDemo);
		m_strDemoPath = m_strDefaultDemo;
		m_checkDemo.SetCheck(TRUE);
		if (cmPath::GetFileExt(m_strDemoPath).CompareNoCase(_T("dcf"))==0)
		{
			if (!LoadDemoConfigFile(m_strDemoPath))
			{
				m_dcf.vecDemo.clear();
			}
		}
	}
	if (cmFile::IsExisted(m_strDefaultFirmware)==IE_RET_EXIST_FILE)
	{
		if ((m_strDefaultFirmware.Find(_T(".img"))!=-1)||(m_strDefaultFirmware.Find(_T(".bin"))!=-1))
		{
			PostMessage(WM_LOADFW_MSG,0,0);
		}
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFactoryToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CPaintDC dc(this); // device context for painting
		HBITMAP hLedBitmap;
		if (m_bRedLedLight)
		{
			hLedBitmap = m_hRedLedBitmap;
		}
		else
			hLedBitmap = m_hGreenLedBitmap;
	// TODO: Add your message handler code here
		if (hLedBitmap)
		{
			CPaintDC dc(GetDlgItem(IDC_PICTURE_LED));
			CDC ImageDC;
			ImageDC.CreateCompatibleDC(&dc);
			HGDIOBJ hOldGdiObject;
			hOldGdiObject = ImageDC.SelectObject(hLedBitmap);
			int nDstWidth,nDstHeight;
			RECT dstClientRect;
			dc.GetWindow()->GetClientRect(&dstClientRect);
			nDstHeight = dstClientRect.bottom - dstClientRect.top;
			nDstWidth = dstClientRect.right-dstClientRect.left;
			BITMAP bmpStruct;
			BOOL bRet;
			GetObject(hLedBitmap,sizeof(BITMAP),&bmpStruct);
			
			bRet = TransparentBlt(dc.m_hDC, 0, 0, nDstWidth, nDstHeight, ImageDC.m_hDC, 0, 0, bmpStruct.bmWidth, bmpStruct.bmHeight, RGB(255,255,255));
			ImageDC.SelectObject(hOldGdiObject);
		}
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFactoryToolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFactoryToolDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bRunFlag)
	{
		return;
	}
	if (m_pScanThread)
	{
		m_bContinueScan = FALSE;
		MSG msg;
		DWORD dwRet;
		while (TRUE)
		{
			dwRet = MsgWaitForMultipleObjects(1, &m_pScanEvent->m_hObject,FALSE, 10000, QS_ALLINPUT);
			if(WAIT_OBJECT_0 ==dwRet )
			{
				break;
			}
			else if( (WAIT_OBJECT_0+1)==dwRet )
			{
				while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				
			}
			else if (WAIT_TIMEOUT==dwRet)
			{
				TerminateThread(m_pScanThread->m_hThread,0);
				break;
			}
		}//end while
		m_pScanThread = NULL;
		delete m_pScanEvent;
		m_pScanEvent = NULL;
	}
	if (m_bSnDllOn)
	{
		FreeSnRes();
	}
	if (m_popMenu)
	{
		delete m_popMenu;
		m_popMenu = NULL;
	}
	if (g_pLogObject)
	{
		delete g_pLogObject;
		g_pLogObject = NULL;
	}
	if (m_pLangObject)
	{
		delete m_pLangObject;
		m_pLangObject = NULL;
	}
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if (m_pScan)
	{
		delete m_pScan;
		m_pScan = NULL;
	}
	if (m_pLog)
	{
		delete m_pLog;
		m_pLog = NULL;
	}

	if (m_hRedLedBitmap)
	{
		DeleteObject(m_hRedLedBitmap);
		m_hRedLedBitmap = NULL;
	}

	if (m_hGreenLedBitmap)
	{
		DeleteObject(m_hGreenLedBitmap);
		m_hGreenLedBitmap = NULL;
	}
	CDialog::OnOK();
}

void CFactoryToolDlg::OnButtonFirmware() 
{
	// TODO: Add your control notification handler code here
	BOOL bRet;
	CString strFile,strFilter;
	strFilter = _T("Firmware(*.img)/Loader(*.bin)|*.img;*.bin|All File(*.*)|*.*||");
	bRet = cmCommonDlg::OpenDialog(strFile,strFilter);
	if (bRet)
	{
		UpdateFwInfo(strFile);
	}
}

void CFactoryToolDlg::OnButtonRun() 
{
	// TODO: Add your control notification handler code here
	CString strTitle;
	m_btnRun.GetWindowText(strTitle);
	int i;
	if (!m_bRunFlag)
	{//do run
		m_bRunFlag = TRUE;
		m_bRedLedLight = FALSE;
		m_btnRun.SetBitmaps(IDB_BITMAP_PAUSE,RGB(255,255,255));
		EnableCtrl(TRUE,FALSE,TRUE,TRUE,TRUE,FALSE,TRUE,FALSE);
		m_btnRun.SetWindowText(m_strStopText);
		//add backup device to run context
		AddBackupToRunContext();
	}
	else
	{//do stop
		for (i=0;i<MAX_DEVICE;i++)
		{
			if (m_runContext[i].bUse)
			{
				if (m_runContext[i].bRun)
				{
					return;
				}
			}
		}
		m_bRunFlag = FALSE;
		m_bRedLedLight = TRUE;
		m_btnRun.SetBitmaps(IDB_BITMAP_RUN,RGB(255,255,255));
		EnableCtrl(TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE);
		m_btnRun.SetWindowText(m_strRunText);
	}
	UpdateData(TRUE);
	Invalidate();
	UpdateWindow();
}

void CFactoryToolDlg::OnButtonDemo() 
{
	// TODO: Add your control notification handler code here
	BOOL bRet;
	CString strFile;
	bRet = cmCommonDlg::BrowseDialog(strFile,_T(""),_T(""),_T("Demo"),TRUE);
	if (bRet)
	{
		m_strDemoPath = strFile;
		if (cmPath::GetFileExt(m_strDemoPath).CompareNoCase(_T("dcf"))==0)
		{
			if (!LoadDemoConfigFile(m_strDemoPath))
			{
				m_dcf.vecDemo.clear();
			}
		}
		m_edtDemo.SetWindowText(strFile);
		m_edtDemo.SetFocus();
		m_edtDemo.SetSel(0,-1);
	}
}

void CFactoryToolDlg::OnButtonLanguage() 
{
	// TODO: Add your control notification handler code here
	POINT curXY;
	if (GetCursorPos(&curXY))
	{
		if (m_popMenu)
		{
			TrackPopupMenu(m_popMenu->m_hMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON,curXY.x,curXY.y,0,m_hWnd,NULL);
		}
	}
}
void CFactoryToolDlg::InitToolButton()
{
	if (m_font.GetSafeHandle()!=NULL)
	{
		m_btnFirmware.SetFont(&m_font);
		m_btnDemo.SetFont(&m_font);
		m_btnRun.SetFont(&m_font);
		m_btnLanguage.SetFont(&m_font);
		m_btnExit.SetFont(&m_font);
		((CButton *)GetDlgItem(IDC_RADIO_UPGRADE))->SetFont(&m_font);
		((CButton *)GetDlgItem(IDC_RADIO_RESTORE))->SetFont(&m_font);
	}

	m_btnFirmware.SetFlat(FALSE);
	m_btnFirmware.SetBitmaps(IDB_BITMAP_FIRMWARE,RGB(0,0,0));
	m_btnDemo.SetFlat(FALSE);
	m_btnDemo.SetBitmaps(IDB_BITMAP_DEMO,RGB(0,0,0));
	m_btnRun.SetFlat(FALSE);
	m_btnRun.SetBitmaps(IDB_BITMAP_RUN,RGB(255,255,255));
	m_btnLanguage.SetFlat(FALSE);
	m_btnLanguage.SetBitmaps(IDB_BITMAP_LANGUAGE,RGB(0,0,0));
	m_btnExit.SetFlat(FALSE);
	m_btnExit.SetBitmaps(IDB_BITMAP_EXIT,RGB(0,0,0));

//	m_btnRun.SetWindowText(m_strRunText);
	EnableCtrl(TRUE,TRUE,TRUE,FALSE,TRUE,FALSE,TRUE,TRUE);
}
void CFactoryToolDlg::InitGroupbox()
{
	if (m_font.GetSafeHandle()!=NULL)
	{
		m_groupboxhelp.SetFont(&m_font);
	}

	m_groupboxhelp.SetBold(TRUE);

	m_groupboxhelp.SetTextColor(RGB(0,0,0));

}
void CFactoryToolDlg::InitStatic()
{
	m_staticFirmware.SetFontStatic(_T("宋体"),13,RGB(0,0,0),FS_NORMAL);
	m_staticFwChip.SetFontStatic(_T("宋体"),13,RGB(0,0,0),FS_NORMAL);
	m_staticFwVersion.SetFontStatic(_T("宋体"),13,RGB(0,0,0),FS_NORMAL);
	m_staticLoaderVer.SetFontStatic(_T("宋体"),13,RGB(0,0,0),FS_NORMAL);

	m_staticSuccess.SetFontStatic(_T("宋体"),13,RGB(0,0,0),FS_NORMAL);
	m_staticFail.SetFontStatic(_T("宋体"),13,RGB(0,0,0),FS_NORMAL);
	m_staticTotal.SetFontStatic(_T("宋体"),13,RGB(0,0,0),FS_NORMAL);

	m_staticSuccessCount.SetFontStatic(_T("宋体"),20,RGB(0,255,0),FS_BOLD);
	m_staticFailCount.SetFontStatic(_T("宋体"),20,RGB(255,0,0),FS_BOLD);
	m_staticTotalCount.SetFontStatic(_T("宋体"),20,RGB(0,0,0),FS_BOLD);
	m_staticSuccessCount.SetWindowText(_T("00000"));
	m_staticFailCount.SetWindowText(_T("00000"));
	m_staticTotalCount.SetWindowText(_T("00000"));


	m_staticHelpText.SetFontStatic(_T("宋体"),13,RGB(0,0,0),FS_NORMAL);
 	m_staticHelpText.SetWindowText(m_strHelpText);

	CFont font;
	font.CreateFont(-13,6,0,0,FW_NORMAL,FALSE,FALSE,0,  
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,  
	DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,_T("宋体"));
	m_edtFirmware.SetFont(&font);
	m_edtDemo.SetFont(&font);
	font.Detach();
}
void CFactoryToolDlg::RefreshUsbInteface()
{
	if (m_pScan)
	{
		vector<STRUCT_HOST_INFO> vecHost;
		m_pScan->GetHostInfo(vecHost);
		int i,j;
		HTREEITEM hParent,hRoot,hItem;
		TVINSERTSTRUCT item;
		CString strHubText,strPortText,strPortLayer;
		hRoot = m_treeDeviceInfo.GetTreeCtrl().InsertItem(m_strComputerText,IMAGE_INDEX_COMPUTER,IMAGE_INDEX_COMPUTER);
		
		item.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE;
 		item.item.stateMask = 0xFF;
 		item.item.state = TVIS_EXPANDED;

		for (i=0;i<vecHost.size();i++)
		{
			if (vecHost[i].bUsb20)
			{
				strHubText = _T("RootHub20");
				item.hInsertAfter = TVI_FIRST;
			}
			else
			{
				if (!m_bSupportFullUsb)
				{
					continue;
				}
				strHubText = _T("RootHub");;
				item.hInsertAfter = TVI_LAST;
			}
			item.hParent = hRoot;
			item.item.cchTextMax = strHubText.GetLength();
			item.item.pszText = (LPTSTR)(LPCTSTR)strHubText;
			item.item.iImage = IMAGE_INDEX_HUB;
			item.item.iSelectedImage = IMAGE_INDEX_HUB;
			item.item.lParam = GetLayerID(vecHost[i].strHostLayer);
			hParent = m_treeDeviceInfo.GetTreeCtrl().InsertItem(&item);
			item.hParent = hParent;
			item.hInsertAfter = TVI_LAST;
			for(j=0;j<vecHost[i].nPort;j++)
			{
				strPortLayer.Format(_T("%s-%d"),vecHost[i].strHostLayer,j+1);
				GetIDFromLayerMap(strPortLayer);
				strPortText.Format(_T("Port[%d]"),j+1);
				item.item.cchTextMax = strPortText.GetLength();
				item.item.pszText = (LPTSTR)(LPCTSTR)strPortText;
				item.item.iImage = IMAGE_INDEX_PORT_OFF;
				item.item.iSelectedImage = IMAGE_INDEX_PORT_OFF;
				item.item.lParam = GetLayerID(strPortLayer);
				hItem = m_treeDeviceInfo.GetTreeCtrl().InsertItem(&item);
				m_treeItemMap[strPortLayer] = hItem;
			}
		}
		m_treeDeviceInfo.GetTreeCtrl().Expand(hRoot,TVE_EXPAND);
		m_nScanDevice = 1;
	}
}
void CFactoryToolDlg::InitTree()
{
	// set style for tree view
	UINT uTreeStyle = TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_FULLROWSELECT;
	m_treeDeviceInfo.GetTreeCtrl().ModifyStyle(0,uTreeStyle);

	/*
	 * Create image list for tree & load icons
	 */

    m_imgList.Create (16, 16, ILC_COLOR32|ILC_MASK,5,1);
	
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_ICON_MONITOR));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_ICON_HOST));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_ICON_HUB));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_ICON_PORT));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_ICON_PORT_OFF));
	
	// assign image list to tree control
	m_treeDeviceInfo.GetTreeCtrl().SetImageList(&m_imgList,TVSIL_NORMAL);
		
	/*
	 *  Insert columns to tree control
	 */

	m_treeDeviceInfo.InsertColumn(0, m_strDeviceListText, LVCFMT_CENTER, 180);
	m_treeDeviceInfo.InsertColumn(1, m_strDeviceTypeText, LVCFMT_CENTER, 100);
	m_treeDeviceInfo.InsertColumn(2, m_strIDText, LVCFMT_CENTER, 50);
	m_treeDeviceInfo.InsertColumn(3, m_strPromptText, LVCFMT_CENTER, 220);

	RefreshUsbInteface();
 	
}
void CFactoryToolDlg::RefreshGridText()
{
	CGridHeaderSections *pSections;
	CGridHeaderSection *pSection;

	
	pSections = m_resultGrid.GetHeader()->GetSections();
	pSection = pSections->GetSection(1);
	pSection->SetCaption(m_strSuccessText);

	pSections = m_failGrid.GetHeader()->GetSections();
	pSection = pSections->GetSection(1);
	pSection->SetCaption(m_strFailText);

}
void CFactoryToolDlg::InitGrid()
{
	CGridColumn *pColumn;
	CGridHeaderSections *pSections;
	CGridHeaderSection *pSection;
	
	m_resultGrid.AddColumn(_T(""), 50, LVCFMT_CENTER);//ID
	m_resultGrid.AddColumn(_T(""), 130, LVCFMT_CENTER);//Time
	
	pColumn = m_resultGrid.GetColumn(0);
	pColumn->SetTabStop(FALSE);
	pColumn->SetReadOnly();
	
	
	pColumn = m_resultGrid.GetColumn(1);
	pColumn->SetTabStop(FALSE);
	pColumn->SetReadOnly();

	
	pSections = m_resultGrid.GetHeader()->GetSections();
	pSection = pSections->GetSection(0);
	pSection->SetCaption(m_strIDText);
	pSection->SetAlignment(LVCFMT_CENTER);
	
	
	pSection = pSections->GetSection(1);
	pSection->SetCaption(m_strSuccessText);
	pSection->SetAlignment(LVCFMT_CENTER);

	

	m_resultGrid.SetRowHeight(22);
	m_resultGrid.GetHeader()->SetSectionHeight(25);


	m_failGrid.AddColumn(_T(""), 50, LVCFMT_CENTER);//ID
	m_failGrid.AddColumn(_T(""), 130, LVCFMT_CENTER);//Error

	pColumn = m_failGrid.GetColumn(0);
	pColumn->SetTabStop(FALSE);
	pColumn->SetReadOnly();


	pColumn = m_failGrid.GetColumn(1);
	pColumn->SetTabStop(FALSE);
	pColumn->SetReadOnly();


	pSections = m_failGrid.GetHeader()->GetSections();
	pSection = pSections->GetSection(0);
	pSection->SetCaption(m_strIDText);
	pSection->SetAlignment(LVCFMT_CENTER);


	pSection = pSections->GetSection(1);
	pSection->SetCaption(m_strFailText);
	pSection->SetAlignment(LVCFMT_CENTER);



	m_failGrid.SetRowHeight(22);
	m_failGrid.GetHeader()->SetSectionHeight(25);
	
	
	CFont font;
	font.CreateFont(-13,6,0,0,FW_BOLD,FALSE,FALSE,0,  
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,  
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,_T("宋体"));
	m_resultGrid.SetFont(&font);
	m_resultGrid.GetHeader()->SetFont(&font);
	m_failGrid.SetFont(&font);
	m_failGrid.GetHeader()->SetFont(&font);

	font.Detach();
	
	m_resultGrid.GetHeader()->SynchronizeSections();
	m_resultGrid.SetRowCount(m_resultItems.size());
	m_failGrid.GetHeader()->SynchronizeSections();
	m_failGrid.SetRowCount(m_failItems.size());

}
BOOL CFactoryToolDlg::LoadDemoConfigFile(CString strDcfFile)
{
	cmIniFile iniFile;
	if (!cmFile::IsExisted(strDcfFile))
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadDemoConfigFile-->PathFileExists failed"));
		}
		return FALSE;
	}
	bool bRet;
	bRet = iniFile.Load((LPTSTR)(LPCTSTR)strDcfFile);
	if (!bRet)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadDemoConfigFile-->Load file failed"));
		}
		return FALSE;
	}
	m_dcf.nCount = 0;
	m_dcf.vecDemo.clear();
	cmIniSection *pSection;
	TCHAR tzDemoSection[]=_T("Demo");
	pSection = iniFile.GetSection(tzDemoSection);
	if (!pSection)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadDemoConfigFile-->GetSection Language failed"));
		}
		return FALSE;
	}
	CString strValue;
	strValue.Format(_T("%s"), pSection->GetKeyValue(_T("Count")).c_str() );
	int nCount;
	nCount = cmNumString::StrToInt32(strValue);
	if (nCount<=0)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadDemoConfigFile-->GetKeyValue Count failed"));
		}
		return FALSE;
	}
	
	strValue.Format(_T("%s"), pSection->GetKeyValue(_T("NoCheck")).c_str() );
	if (strValue.CompareNoCase(_T("TRUE"))==0)
	{
		m_dcf.bCheck = FALSE;
	}
	else
		m_dcf.bCheck = TRUE;
	int i;
	CString strKey;
	STRUCT_DEMO_ITEM item;
	ULONGLONG ullFileSize;
	m_dcf.ullTotalSize = 0;
	for (i=0;i<nCount;i++)
	{
		strKey.Format(_T("File_%d_Path"),i+1);
		strValue.Format(_T("%s"), pSection->GetKeyValue((LPTSTR)(LPCTSTR)strKey).c_str() );
		if (!cmFile::IsExisted(strValue))
		{
			if (g_pLogObject)
			{
				g_pLogObject->Record(_T("LoadDemoConfigFile-->PathFileExists %s failed"),strValue);
			}
			return FALSE;
		}
		item.strFilePath = strValue;
		cmFile::GetFileSize(strValue,ullFileSize);
		item.ullFileSize = ullFileSize;
		m_dcf.ullTotalSize += ullFileSize;

		strKey.Format(_T("File_%d_Offset"),i+1);
		strValue.Format(_T("%s"), pSection->GetKeyValue((LPTSTR)(LPCTSTR)strKey).c_str() );
		item.uiFlashOffset = cmNumString::StrToULong(strValue,0);

		m_dcf.vecDemo.push_back(item);
	}
	m_dcf.nCount = m_dcf.vecDemo.size();
	return TRUE;
}

BOOL CFactoryToolDlg::LoadConfig()
{
	CString strConfigPath;
	strConfigPath = m_strModulePath + _T("config.ini");
	if (!cmFile::IsExisted(strConfigPath))
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->PathFileExists failed"));
		}
		return FALSE;
	}
	bool bRet;
	bRet = m_iniConfig.Load((LPTSTR)(LPCTSTR)strConfigPath);
	if (!bRet)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->Load file failed"));
		}
		return FALSE;
	}
	cmIniSection *pSection;
	TCHAR tzLangSection[]=_T("Language");
	pSection = m_iniConfig.GetSection(tzLangSection);
	if (!pSection)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->GetSection Language failed"));
		}
		return FALSE;
	}
 	CString strValue;
	strValue.Format(_T("%s"), pSection->GetKeyValue(_T("Kinds")).c_str() );
	int langKinds;
	langKinds = cmNumString::StrToInt32(strValue);
	if (langKinds<=0)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->GetKeyValue kinds failed"));
		}
		return FALSE;
	}

	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("Selected")).c_str() );
	int curLang;
	curLang = cmNumString::StrToInt32(strValue);
	if (curLang<=0)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->GetKeyValue Selected failed"));
		}
		return FALSE;
	}
	m_curLang = curLang-1;
	
	strValue.Format(_T("%s"), pSection->GetKeyValue(_T("LangPath")).c_str() );
	m_LangPath = cmPath::FixPath(m_strModulePath + strValue);

	if (!cmFile::IsExisted(m_LangPath))
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->PathFileExists LangPath failed"));
		}
		return FALSE;
	}
	int i;
	CString strKey;
	STRUCT_LANG_PROPERTY langProp;
	for (i=0;i<langKinds;i++)
	{
		strKey.Format(_T("Lang%dFile"),i+1);
		strValue.Format(_T("%s"), pSection->GetKeyValue((LPTSTR)(LPCTSTR)strKey).c_str() );
		if (!cmFile::IsExisted(m_LangPath+strValue))
		{
			if (g_pLogObject)
			{
				g_pLogObject->Record(_T("LoadConfig-->PathFileExists %s failed"),strValue);
			}
			return FALSE;
		}
		langProp.strFileName = strValue;
	
		strKey.Format(_T("Lang%dFontName"),i+1);
		strValue.Format(_T("%s"), pSection->GetKeyValue((LPTSTR)(LPCTSTR)strKey).c_str() );
		langProp.strFontName = strValue;

		strKey.Format(_T("Lang%dFontSize"),i+1);
		strValue.Format(_T("%s"), pSection->GetKeyValue((LPTSTR)(LPCTSTR)strKey).c_str() );
		langProp.uiFontSize = cmNumString::StrToInt32(strValue);
		if (langProp.uiFontSize<=0)
		{
			langProp.uiFontSize = 9;
		}
		m_langProp.push_back(langProp);
	}
	TCHAR tzSystemSection[]=_T("System");
	pSection = m_iniConfig.GetSection(tzSystemSection);
	if (!pSection)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("LoadConfig-->GetSection System failed"));
		}
		return FALSE;
	}
	
	int nValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("MSC_VID")).c_str() );	
	nValue = cmNumString::StrToULong(strValue,0);

	if (nValue<=0)
	{
		m_mscVid = 0;
	}
	else
		m_mscVid = nValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("MSC_PID")).c_str() );	
	nValue = cmNumString::StrToULong(strValue,0);

	if (nValue<=0)
	{
		m_mscPid = 0;
	}
	else
		m_mscPid = nValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("SUPPORTLOWUSB")).c_str() );	
	if (strValue.CompareNoCase(_T("TRUE"))==0)
	{
		m_bSupportFullUsb = TRUE;
	}
	else
		m_bSupportFullUsb = FALSE;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("FW_NOT_CHECK")).c_str() );	
	if (strValue.CompareNoCase(_T("TRUE"))==0)
	{
		m_bCheckFw = FALSE;
	}
	else
		m_bCheckFw = TRUE;

	
	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("MSC_UPGRADE_SUPPORT")).c_str() );	
	if (strValue.CompareNoCase(_T("TRUE"))==0)
	{
		m_bSupportMsc = TRUE;
	}
	else
		m_bSupportMsc = FALSE;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("DISKVOLUME")).c_str());
	m_strDiskVolume = strValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("DEMOPATH")).c_str());
	m_strDefaultDemo = strValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("DEFAULT_FIRMWARE")).c_str());
	m_strDefaultFirmware = strValue;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("UID_STRING")).c_str());
	m_strUidHead = strValue;

	
	m_mscTimeout = cmNumString::StrToInt32(pSection->GetKeyValue(_T("MSC_TIMEOUT")).c_str());
	m_rockusbTimeout = cmNumString::StrToInt32(pSection->GetKeyValue(_T("ROCKUSB_TIMEOUT")).c_str());

	if (m_mscTimeout<=0)
	{
		m_mscTimeout = 20;
	}
	if (m_rockusbTimeout<=0)
	{
		m_rockusbTimeout = 20;
	}

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("RESET_COPYDEMO")).c_str() );	
	if (strValue.CompareNoCase(_T("TRUE"))==0)
	{
		m_bResetAfterDemo = TRUE;
	}
	else
		m_bResetAfterDemo = FALSE;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("NOTRESET_AFTER_UPGRADE")).c_str() );	
	if (strValue.CompareNoCase(_T("TRUE"))==0)
	{
		m_bResetAfterUpgrade = FALSE;
	}
	else
		m_bResetAfterUpgrade = TRUE;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("FW_VERSION_MOD")).c_str() );	
	if (strValue.CompareNoCase(_T("DEC"))==0)
	{
		m_bFwVerHex = FALSE;
	}
	else
		m_bFwVerHex = TRUE;

	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("MISC_MODIFY_MODE")).c_str());
	if (strValue.CompareNoCase(_T("WIPE_ALL"))==0)
		m_emMiscModifyFlag = MISC_MODIFY_WIPE_ALL;
	else if (strValue.CompareNoCase(_T("WIPE_DATA"))==0)
		m_emMiscModifyFlag = MISC_MODIFY_WIPE_DATA;
	else
		m_emMiscModifyFlag = MISC_MODIFY_NONE;
	
	nValue = cmNumString::StrToInt32(pSection->GetKeyValue(_T("FORCE_DATA_BAND")).c_str());
	if ((nValue>0)&&(nValue<=6))
	{
		m_uiForceDataBand = nValue;
	}
	else
		m_uiForceDataBand = 0;
	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("LOOP_TEST")).c_str() );	
	if (strValue.CompareNoCase(_T("TRUE"))==0)
	{
		m_bLoopTest = TRUE;
	}
	else
		m_bLoopTest = FALSE;

	m_nRestartTimeout = cmNumString::StrToInt32(pSection->GetKeyValue(_T("RESTART_TIMEOUT")).c_str());
	if ((m_nRestartTimeout<=0)||(m_nRestartTimeout>60))
	{
		m_nRestartTimeout = 5;
	}
	m_nSwitchRetryInterval = cmNumString::StrToInt32(pSection->GetKeyValue(_T("SWITCH_RETRY_INTERVAL")).c_str());
	if ((m_nSwitchRetryInterval<=0)||(m_nSwitchRetryInterval>60))
	{
		m_nSwitchRetryInterval = 5;
	}
	strValue.Empty();
	strValue.Format(_T("%s"),pSection->GetKeyValue(_T("SN_DLL_ON")).c_str() );	
	if (strValue.CompareNoCase(_T("TRUE"))==0)
	{
		m_bSnDllOn = TRUE;
	}
	else
		m_bSnDllOn = FALSE;

	return TRUE;
}
CString CFactoryToolDlg::GetLocalString(CString strKey)
{
	BOOL bRet;
	CString strValue ;
	bRet = m_pLangObject->GetString(_T("PROMPT"),strKey,strValue);
	if (!bRet)
	{
		strValue = _T("");
	}
	return strValue;
}
void CFactoryToolDlg::InitLangString()
{
	m_staticFwVersion.GetWindowText(m_strFwVerText);
	m_staticFwChip.GetWindowText(m_strFwChipText);
	m_staticLoaderVer.GetWindowText(m_strLoaderVerText);
	m_strDeviceListText = GetLocalString(_T("IDS_DEVICELIST"));
	m_strDeviceTypeText = GetLocalString(_T("IDS_DEVICETYPE"));
	m_strIDText = GetLocalString(_T("IDS_ID"));
	m_strProcessText = GetLocalString(_T("IDS_ID"));
	m_strProcessText = GetLocalString(_T("IDS_PROCESS"));
	m_strPromptText = GetLocalString(_T("IDS_PROMPT"));
	m_strComputerText = GetLocalString(_T("IDS_MYCOMPUTER"));
// 	m_strHighControllerText = GetLocalString(_T("IDS_HIGH_CONTROLLER"));
// 	m_strFullControllerText = GetLocalString(_T("IDS_FULL_CONTROLLER"));
//	m_strRootHubText = GetLocalString(_T("IDS_ROOTHUB"));
//	m_strPortText = GetLocalString(_T("IDS_PORT"));
//	m_strEhciText = GetLocalString(_T("IDS_EHCI"));
//	m_strUhicText = GetLocalString(_T("IDS_UHCI"));
//	m_strHubText = GetLocalString(_T("IDS_HUB"));
	m_strRunText = GetLocalString(_T("IDS_RUN_TEXT"));
	m_strStopText = GetLocalString(_T("IDS_STOP_TEXT"));
	m_strFailText = GetLocalString(_T("IDS_FAIL_TEXT"));
	m_strSuccessText = GetLocalString(_T("IDS_SUCCESS_TEXT"));
	m_strResultText = GetLocalString(_T("IDS_RESULT_TEXT"));
	m_strTimeText = GetLocalString(_T("IDS_TIME_TEXT"));
	m_strHelpText = GetLocalString(_T("IDS_HELP"));
	m_strHelpText.Replace(_T("\\r\\n"),_T("\r\n"));
}
void CFactoryToolDlg::OnLangSelect(UINT uid)
{
	CHAR index;
	index = uid - IDM_LANG_MENUITEM;
	if (index!=m_curLang)
	{
		m_popMenu->CheckMenuItem(m_curLang,MF_UNCHECKED|MF_BYPOSITION);
		m_curLang = index;
		m_pLangObject->CurrentLang = m_curLang;
		m_pLangObject->SetLanguageString();
		m_popMenu->CheckMenuItem(m_curLang,MF_CHECKED|MF_BYPOSITION);
		CString strTitle;
		GetWindowText(strTitle);
		strTitle = strTitle + APP_VERSION;
		SetWindowText(strTitle);
		InitLangString();
		RefreshTreeText();
		RefreshGridText();
		ShowFwInfo();
		Invalidate();
		UpdateWindow();
	}
}
void CFactoryToolDlg::RefreshTreeText()
{
	m_staticHelpText.SetWindowText(m_strHelpText);
	HDITEM item;
	item.mask = HDI_TEXT;
	item.pszText = (LPTSTR)(LPCTSTR)m_strDeviceListText;
	m_treeDeviceInfo.m_bHeaderChangesBlocked = FALSE;
	m_treeDeviceInfo.GetHeaderCtrl().SetItem(0,&item);
	item.pszText = (LPTSTR)(LPCTSTR)m_strDeviceTypeText;
	m_treeDeviceInfo.m_bHeaderChangesBlocked = FALSE;
	m_treeDeviceInfo.GetHeaderCtrl().SetItem(1,&item);
	item.pszText = (LPTSTR)(LPCTSTR)m_strIDText;
	m_treeDeviceInfo.m_bHeaderChangesBlocked = FALSE;
	m_treeDeviceInfo.GetHeaderCtrl().SetItem(2,&item);
	item.pszText = (LPTSTR)(LPCTSTR)m_strPromptText;
	m_treeDeviceInfo.m_bHeaderChangesBlocked = FALSE;
	m_treeDeviceInfo.GetHeaderCtrl().SetItem(3,&item);
	
 	HTREEITEM hRoot,hNextItem,hItem;
	hRoot = m_treeDeviceInfo.GetTreeCtrl().GetRootItem();
	m_treeDeviceInfo.GetTreeCtrl().SetItemText(hRoot,m_strComputerText);
	hItem = m_treeDeviceInfo.GetTreeCtrl().GetNextItem(hRoot,TVGN_CHILD);
	vector<STRUCT_HOST_INFO> vecHost;
	CString strPortLayer,strPortText,strDeviceText;
	DWORD dwParam;
	int i,j;
	m_pScan->GetHostInfo(vecHost);
	while (hItem)
	{
		dwParam = m_treeDeviceInfo.GetTreeCtrl().GetItemData(hItem);
		for (i=0;i<vecHost.size();i++)
		{
			if (dwParam==GetLayerID(vecHost[i].strHostLayer))
			{
				break;
			}
		}
		hNextItem = hItem;
		if (i<vecHost.size())
		{//found it
			if (vecHost[i].bUsb20)
			{
				m_treeDeviceInfo.GetTreeCtrl().SetItemText(hItem,_T("RootHub20"));
			}
			else
				m_treeDeviceInfo.GetTreeCtrl().SetItemText(hItem,_T("RootHub"));
			
			for (j=0;j<vecHost[i].nPort;j++)
			{
				strPortLayer.Format(_T("%s-%d"),vecHost[i].strHostLayer,j+1);
				strPortText.Format(_T("Port[%d]"),j+1);
				hItem = m_treeItemMap[strPortLayer];
				m_treeDeviceInfo.GetTreeCtrl().SetItemText(hItem,strPortText);
			}
		}
		hItem = m_treeDeviceInfo.GetTreeCtrl().GetNextItem(hNextItem,TVGN_NEXT);
	}

	//refresh connected device item 
	device_list_iter devIter;
	hub_list_iter hubIter;
	for (hubIter=m_hubList.begin();hubIter!=m_hubList.end();hubIter++)
	{
		hItem = m_treeItemMap[(*hubIter).strHubLayer];
		m_treeDeviceInfo.SetItemText(hItem,1,_T("Hub"));
		m_treeDeviceInfo.SetItemText(hItem,2,(*hubIter).strHubLayer);
		for (j=0;j<(*hubIter).nPort;j++)
		{
			strPortLayer.Format(_T("%s-%d"),(*hubIter).strHubLayer,j+1);
			strPortText.Format(_T("Port[%d]"),j+1);
			hItem = m_treeItemMap[strPortLayer];
			m_treeDeviceInfo.GetTreeCtrl().SetItemText(hItem,strPortText);
		}
	}
	for (devIter=m_deviceList.begin();devIter!=m_deviceList.end();devIter++)
	{
		if ((*devIter).emUsbType==RKUSB_MSC)
		{
			strDeviceText = _T("Msc");
		}
		else if ((*devIter).emUsbType==RKUSB_ADB)
		{
			strDeviceText = _T("Adb");
		}
		else if ((*devIter).emUsbType==RKUSB_LOADER)
		{
			strDeviceText = _T("Loader");
		}
		else if ((*devIter).emUsbType==RKUSB_MASKROM)
		{
			strDeviceText = _T("Maskrom");
		}
		else
			strDeviceText = _T("");
		hItem = m_treeItemMap[(*devIter).strLayer];
		m_treeDeviceInfo.SetItemText(hItem,1,strDeviceText);
		m_treeDeviceInfo.SetItemText(hItem,2,(*devIter).strLayer);
	}

}

BOOL CFactoryToolDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ( pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE )
	{
		return TRUE;
	}
	if ( pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN )
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CFactoryToolDlg::OnButtonExit() 
{
	// TODO: Add your control notification handler code here
	OnClose();
}
LRESULT CFactoryToolDlg::OnDeviceChange(WPARAM wParam,LPARAM lParam)
{
	UINT uiEvent = wParam;
	if ((uiEvent==DBT_DEVICEARRIVAL)||(uiEvent==DBT_DEVICEREMOVECOMPLETE)||(uiEvent==0x7))
	{
		m_csScanDevice.Lock();
		m_nScanDevice++;
		m_csScanDevice.Unlock();
	}
	return 0;
}
VOID CFactoryToolDlg::ScanThreadProc()
{
	m_pScanEvent = new CEvent(FALSE,TRUE);
	m_pScanEvent->ResetEvent();
	vector<STRUCT_HUB_INFO> vecHub;
	STRUCT_RKDEVICE_DESC device;
	device_list_iter devIter;
	hub_list_iter hubIter;
	int i,nDeviceCount;
	BOOL bRet;
	while(m_bContinueScan)
	{
		m_csScanDevice.Lock();
		if (m_nScanDevice>0)
		{
			m_nScanDevice--;
			m_csScanDevice.Unlock();
			nDeviceCount = m_pScan->Search(RKUSB_MASKROM|RKUSB_LOADER|RKUSB_MSC|RKUSB_ADB);
			/*if (g_pLogObject)
			{
				g_pLogObject->Record(_T("Info:ScanThreadProc device=%d"),nDeviceCount);
			}*/
//check hub change
			m_pScan->GetExternalHubInfo(vecHub);
			for (hubIter=m_hubList.begin();hubIter!=m_hubList.end();)
			{
				for (i=0;i<vecHub.size();i++)
				{
					if ((*hubIter).strHubLayer==vecHub[i].strHubLayer)
					{
						break;
					}
				}
				if (i>=vecHub.size())
				{//hub remove
					UpdateUsbInterface(&(*hubIter),NULL,FALSE,TRUE);
					hubIter = m_hubList.erase(hubIter);
				}
				else
					hubIter++;
			}
			for (i=0;i<vecHub.size();i++)
			{
				for (hubIter=m_hubList.begin();hubIter!=m_hubList.end();)
				{
					if ((*hubIter).strHubLayer==vecHub[i].strHubLayer)
					{
						break;
					}
					hubIter++;
				}
				if (hubIter==m_hubList.end())
				{//hub add
					if (vecHub[i].nPort!=-1)
					{
						UpdateUsbInterface(&vecHub[i],NULL,FALSE,FALSE);
						m_hubList.push_back(vecHub[i]);
					}
				}
			}
//check device change 
			for (devIter=m_deviceList.begin();devIter!=m_deviceList.end();)
			{
				for (i=0;i<nDeviceCount;i++)
				{
					bRet = m_pScan->GetDevice(device,i);
					if (bRet)
					{
						if ((*devIter).strLayer==device.strLayer)
						{
							break;
						}
					}
				}
				if (i>=nDeviceCount)
				{//device remove
					UpdateUsbInterface(NULL,&(*devIter),TRUE,TRUE);
					HandleRemoveDevice(*devIter);//must do after UpdateUsbInterface
					devIter = m_deviceList.erase(devIter);
				}
				else
					devIter++;
			}
			for (i=0;i<nDeviceCount;i++)
			{
				bRet = m_pScan->GetDevice(device,i);
				if (bRet)
				{
					/*if (g_pLogObject)
					{
						g_pLogObject->Record(_T("Info:Layer %s"),device.strLayer);
					}*/
					for (devIter=m_deviceList.begin();devIter!=m_deviceList.end();)
					{
						if ((*devIter).strLayer==device.strLayer)
						{
							break;
						}
						devIter++;
					}
					if (devIter==m_deviceList.end())
					{//device add
						UpdateUsbInterface(NULL,&device,TRUE,FALSE);
						HandleNewDevice(device);
						m_deviceList.push_back(device);
					}
				}
			}
			//when looptest is true,detect any device is pending
			if (m_bLoopTest)
			{
				ReRunContext();
			}
		}
		else
			m_csScanDevice.Unlock();
		Sleep(200);
	}
	m_pScanEvent->SetEvent();
}
void CFactoryToolDlg::RemoveHubInterface(CString strLayer)
{
	HTREEITEM hParent,hItem;
	CString strKey;
	int i=1;
	hParent = m_treeItemMap[strLayer];
	hItem = m_treeDeviceInfo.GetTreeCtrl().GetChildItem(hParent);
	while(hItem)
	{
		strKey.Format(_T("%s-%d"),strLayer,i);
		m_treeDeviceInfo.GetTreeCtrl().DeleteItem(hItem);
		m_treeItemMap.erase(strKey);
		i++;
		hItem = m_treeDeviceInfo.GetTreeCtrl().GetChildItem(hParent);
	}
}
void CFactoryToolDlg::AddHubInterface(CString strLayer,int nPort)
{
	TVINSERTSTRUCT item;
	HTREEITEM hParent,hItem;
	CString strPortLayer,strPortText;
	hParent = m_treeItemMap[strLayer];
	item.hParent = hParent;
	item.hInsertAfter = TVI_LAST;
	item.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	int i;
	for (i=0;i<nPort;i++)
	{
		strPortLayer.Format(_T("%s-%d"),strLayer,i+1);
		GetIDFromLayerMap(strPortLayer);
		strPortText.Format(_T("Port[%d]"),i+1);
		item.item.cchTextMax = strPortText.GetLength();
		item.item.pszText = (LPTSTR)(LPCTSTR)strPortText;
		item.item.iImage = IMAGE_INDEX_PORT_OFF;
		item.item.iSelectedImage = IMAGE_INDEX_PORT_OFF;
		item.item.lParam = GetLayerID(strPortLayer);
		hItem = m_treeDeviceInfo.GetTreeCtrl().InsertItem(&item);
		m_treeItemMap[strPortLayer] = hItem;
	}
}
void CFactoryToolDlg::UpdateUsbInterface(PSTRUCT_HUB_INFO pHub, PSTRUCT_RKDEVICE_DESC pDevice, BOOL bDevice, BOOL bRemove)
{
	TVITEM item;
	CString strDeviceText,strLayer,strID;
	DWORD dwID;
	BOOL bRet;
	int index=-1;
	if (bDevice)
	{
		strLayer = pDevice->strLayer;
	}
	else
		strLayer = pHub->strHubLayer;
	dwID = GetIDFromLayerMap(strLayer);
	strID = cmNumString::NumToStr(dwID,10);
	
	item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_HANDLE;
	item.hItem = m_treeItemMap[strLayer];
	bRet = m_treeDeviceInfo.GetTreeCtrl().GetItem(&item);
	if (bRet)
	{
		if (bRemove)
		{
			if (bDevice)
			{
				item.iImage = IMAGE_INDEX_PORT_OFF;
				item.iSelectedImage = IMAGE_INDEX_PORT_OFF;
				m_treeDeviceInfo.GetTreeCtrl().SetItem(&item);
				m_treeDeviceInfo.SetItemText(item.hItem,1,_T(""));
				m_treeDeviceInfo.SetItemText(item.hItem,2,_T(""));
				index = GetIndexFromLayer(GetLayerID(strLayer));
				if (index!=-1)
				{
					if (!m_runContext[index].bRun)
					{
						m_treeDeviceInfo.SetItemText(item.hItem,3,_T(""));
						m_treeDeviceInfo.SetRowBKColor(item.hItem,GetSysColor(COLOR_WINDOW),FALSE);
					}
				}
				
			}
			else
			{
				item.iImage = IMAGE_INDEX_PORT_OFF;
				item.iSelectedImage = IMAGE_INDEX_PORT_OFF;
				m_treeDeviceInfo.GetTreeCtrl().SetItem(&item);
				m_treeDeviceInfo.SetItemText(item.hItem,1,_T(""));
				m_treeDeviceInfo.SetItemText(item.hItem,2,_T(""));
				RemoveHubInterface(strLayer);
			}
		}
		else
		{//new device
			if (bDevice)
			{
				if (pDevice->emUsbType==RKUSB_MSC)
				{
					strDeviceText = _T("Msc");
				}
				else if (pDevice->emUsbType==RKUSB_ADB)
				{
					strDeviceText = _T("Adb");
				}
				else if (pDevice->emUsbType==RKUSB_LOADER)
				{
					strDeviceText = _T("Loader");
				}
				else if (pDevice->emUsbType==RKUSB_MASKROM)
				{
					strDeviceText = _T("Maskrom");
				}
				else
					strDeviceText = _T("");
				item.iImage = IMAGE_INDEX_PORT;
				item.iSelectedImage = IMAGE_INDEX_PORT;
				m_treeDeviceInfo.GetTreeCtrl().SetItem(&item);
				m_treeDeviceInfo.SetItemText(item.hItem,1,strDeviceText);
				m_treeDeviceInfo.SetItemText(item.hItem,2,strID);
			}
			else
			{
				item.iImage = IMAGE_INDEX_HUB;
				item.iSelectedImage = IMAGE_INDEX_HUB;
				m_treeDeviceInfo.GetTreeCtrl().SetItem(&item);
				m_treeDeviceInfo.SetItemText(item.hItem,1,_T("Hub"));
				m_treeDeviceInfo.SetItemText(item.hItem,2,strID);
				AddHubInterface(strLayer,pHub->nPort);
			}
		}
	}
}
CString GetLayerString(DWORD layerID)
{
	vector<BYTE> digitVec;
	int i;
	BYTE digit;
	digitVec.clear();
	for (i=0;i<8;i++)
	{
		digit = (layerID >> 28)&0xF;
		layerID = layerID << 4;
		digitVec.push_back(digit);
	}
	CString strLayer=_T("");
	CString strChar;
	for(i=0;i<digitVec.size();i++)
	{
		if (digitVec[i]==0)
		{
			if (!strLayer.IsEmpty())
			{
				strLayer = strLayer + _T("0-");
			}
		}
		else
		{
			strChar.Format(_T("%d-"),digitVec[i]);
			strLayer = strLayer + strChar;
		}
	}
	if (!strLayer.IsEmpty())
	{
		strLayer = strLayer.Left(strLayer.GetLength()-1);
	}
	return strLayer;
}
DWORD GetLayerID(CString strLayer)
{
	DWORD dwRet=0;
	int iRet=0;
	iRet = strLayer.Remove(_T('-'));
	if (strLayer.GetLength()>8)
	{
		strLayer = strLayer.Left(8);
	}
	for ( iRet=0;iRet<(strLayer.GetLength()-1);iRet++ )
	{
		dwRet += (strLayer[iRet]-_T('0'));
		dwRet = dwRet<<4;
	}
	dwRet += (strLayer[iRet]-_T('0'));
	
	return dwRet;
	
}
void CFactoryToolDlg::EnableCtrl(BOOL bFwFlag,BOOL bFwOn,BOOL bRunFlag,BOOL bRunOn,BOOL bUpgradeFlag,BOOL bUpgradeOn,BOOL bOtherFlag,BOOL bOtherOn)
{
	if (bFwFlag)
	{
		m_btnFirmware.EnableWindow(bFwOn);
	}
	if (bRunFlag)
	{
		m_btnRun.EnableWindow(bRunOn);
	}

	if (bOtherFlag)
	{
		m_btnLanguage.EnableWindow(bOtherOn);
		m_btnExit.EnableWindow(bOtherOn);
	}
	if (bUpgradeFlag)
	{
		GetDlgItem(IDC_RADIO_UPGRADE)->EnableWindow(bUpgradeOn);
		GetDlgItem(IDC_RADIO_RESTORE)->EnableWindow(bUpgradeOn);
		m_btnDemo.EnableWindow(bUpgradeOn);
		m_checkDemo.EnableWindow(bUpgradeOn);
	}
	
}
BOOL WatiObjectWithMsg(LPHANDLE pObjHandle)
{
	MSG msg;
	DWORD dwRet;
	BOOL bRet=FALSE;
	while (TRUE)
	{
		dwRet = MsgWaitForMultipleObjects(1, pObjHandle,FALSE, INFINITE, QS_ALLINPUT);
		if(WAIT_OBJECT_0 ==dwRet )
		{
			bRet = TRUE;
			break;
		}
		else if( (WAIT_OBJECT_0+1)==dwRet )
		{
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			
		}
		else if (WAIT_TIMEOUT==dwRet)
		{
			TerminateThread(*pObjHandle,0);
			break;
		}
	}//end while
	return bRet;
}
BOOL CFactoryToolDlg::ParsePartInfo(string &strPartInfo,string &strName,UINT &uiOffset,UINT &uiLen)
{
	string::size_type pos,prevPos;
	string strOffset,strLen;
	int iCount;
	prevPos = pos = 0;
	if (strPartInfo.size()<=0)
	{
		return FALSE;
	}
	pos = strPartInfo.find('@');
	if (pos==string::npos)
	{
		return FALSE;
	}
	strLen = strPartInfo.substr(prevPos,pos-prevPos);
	strLen.erase(0,strLen.find_first_not_of(" "));
	strLen.erase(strLen.find_last_not_of(" ")+1);
	if (strchr(strLen.c_str(),'-'))
	{
		uiLen = 0xFFFFFFFF;
	}
	else
	{
		iCount = sscanf(strLen.c_str(),"0x%x",&uiLen);
		if (iCount!=1)
		{
			return FALSE;
		}
	}

	prevPos = pos +1;
	pos = strPartInfo.find('(',prevPos);
	if (pos==string::npos)
	{
		return FALSE;
	}
	strOffset = strPartInfo.substr(prevPos,pos-prevPos);
	strOffset.erase(0,strOffset.find_first_not_of(" "));
	strOffset.erase(strOffset.find_last_not_of(" ")+1);
	iCount = sscanf(strOffset.c_str(),"0x%x",&uiOffset);
	if (iCount!=1)
	{
		return FALSE;
	}

	prevPos = pos +1;
	pos = strPartInfo.find(')',prevPos);
	if (pos==string::npos)
	{
		return FALSE;
	}
	strName = strPartInfo.substr(prevPos,pos-prevPos);
	strName.erase(0,strName.find_first_not_of(" "));
	strName.erase(strName.find_last_not_of(" ")+1);

	return TRUE;
}
BOOL CFactoryToolDlg::ParseParameterForPartInfo(PCHAR pParam,PCHAR pPartName,UINT &uiOffset,UINT &uiSize)
{

	stringstream paramStream(pParam);
	string strLine,strPartition,strPartInfo,strPartName;
	string::size_type line_size,pos,posColon,posComma;
	BOOL bRet;
	while (!paramStream.eof())
	{
		getline(paramStream,strLine);
		line_size = strLine.size();
		if (line_size==0)
		{
			continue;
		}
		if (strLine[line_size-1]=='\r')
		{
			strLine = strLine.substr(0,line_size-1);
		}
		pos = strLine.find("mtdparts");
		if (pos==string::npos)
		{
			continue;
		}
		posColon = strLine.find(':',pos);
		if (posColon==string::npos)
		{
			continue;
		}		
		strPartition = strLine.substr(posColon+1);
		//提取分区信息
		pos = 0;
		posComma = strPartition.find(',',pos);
		while (posComma!=string::npos)
		{
			strPartInfo = strPartition.substr(pos,posComma-pos);
			bRet = ParsePartInfo(strPartInfo,strPartName,uiOffset,uiSize);
			if (bRet)
			{
				if (stricmp(strPartName.c_str(),pPartName)==0)
				{
					return TRUE;
				}
			}
			pos = posComma+1;
			posComma = strPartition.find(',',pos);
		}
		strPartInfo = strPartition.substr(pos);
		if (strPartInfo.size()>0)
		{
			bRet = ParsePartInfo(strPartInfo,strPartName,uiOffset,uiSize);
			if (bRet)
			{
				if (stricmp(strPartName.c_str(),pPartName)==0)
				{
					return TRUE;
				}
			}
		}
		break;
	}
	return FALSE;

}
BOOL CFactoryToolDlg::GetPartitionInfo(CRKImage *pImage,char *pszName,UINT &uiPartOffset,UINT &uiPartSize)
{
	PSTRUCT_RKIMAGE_HDR pHeader;
	int i,nParamSize;
	char *pParamBuf=NULL;
	BOOL bSuccess=FALSE,bRet;
	if (!pImage)
		return FALSE;
	pHeader = pImage->AndroidImageHead;
	for ( i=0;i<pHeader->item_count;i++ )
	{
		if (stricmp(pHeader->item[i].name,"parameter")==0)
		{
			break;
		}
	}
	if (i>=pHeader->item_count)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Error:GetPartitionInfo-->no found parameter item."));
		}
		goto Exit_GetPartitionInfo;
	}
	nParamSize = pHeader->item[i].size-12;
	pParamBuf = new char[nParamSize+1];
	if (!pParamBuf)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Error:GetPartitionInfo-->new memory failed,err=%d."),GetLastError());
		}
		goto Exit_GetPartitionInfo;
	}
	memset(pParamBuf,0,nParamSize+1);
	bRet = pImage->GetData(pImage->FWOffset+pHeader->item[i].offset+8,nParamSize,(PBYTE)pParamBuf);
	if (!bRet)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Error:GetPartitionInfo-->get parameter failed."));
		}
		goto Exit_GetPartitionInfo;
	}
	bRet = ParseParameterForPartInfo(pParamBuf,pszName,uiPartOffset,uiPartSize);
	if (!bRet)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Error:GetPartitionInfo-->ParseParameterForPartInfo failed."));
		}
		goto Exit_GetPartitionInfo;
	}
	bSuccess = TRUE;
Exit_GetPartitionInfo:
	if (pParamBuf)
	{
		delete []pParamBuf;
		pParamBuf = NULL;
	}
	return bSuccess;

}
CString StringToWideString(char *pszSrc)
{
	CString strText=_T("");
	LPWSTR lpwzStr=NULL;
	int nSize;
	BOOL bRet;
	bRet = cmStrCode::AnsiToUnicode(lpwzStr,nSize,pszSrc);
	if (bRet)
	{
		strText = lpwzStr;
	}

	if (lpwzStr)
	{
		delete []lpwzStr;
		lpwzStr = NULL;
	}
	return strText;
}
BOOL CFactoryToolDlg::CheckPartitionSize(CRKImage *pImage)
{
	PSTRUCT_RKIMAGE_HDR pHeader;
	PSTRUCT_RKIMAGE_ITEM pItem;
	BYTE headerData[64*1024];
	BOOL bRet;
	if (!pImage)
		return FALSE;
	int i,iHeadSize,iHeadMetaSize;
	ULONG64 ulItemSize,ulPartSize;
	pHeader = pImage->AndroidImageHead;
	if ((pHeader->manufacturer[56]==0x55)&&(pHeader->manufacturer[57]==0x66))
	{
		USHORT *pItemRemain;
		pItemRemain = (USHORT *)(&pHeader->manufacturer[58]);
		pHeader->item_count += *pItemRemain;
	}
	iHeadMetaSize = iHeadSize = sizeof(STRUCT_RKIMAGE_HDR)-sizeof(STRUCT_RKIMAGE_ITEM)*MAX_PACKAGE_FILES;
	iHeadSize += pHeader->item_count*sizeof(STRUCT_RKIMAGE_ITEM);
	iHeadSize = CALC_UNIT(iHeadSize, PAGE_SIZE)*PAGE_SIZE;

	bRet = pImage->GetData(pImage->FWOffset,iHeadSize,(LPBYTE)headerData);
	if ( !bRet )
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:CheckPartitionSize-->GetData for header failed"));
		}
		return FALSE;
	}
	for ( i=0;i<pHeader->item_count;i++ )
	{
		pItem = (STRUCT_RKIMAGE_ITEM *)(headerData+iHeadMetaSize+i*sizeof(STRUCT_RKIMAGE_ITEM));
		if ( pItem->flash_offset!=0xFFFFFFFF )
		{
			if (pItem->file[55]=='H')
			{
				ulItemSize = *((DWORD *)(&pItem->file[56]));
				ulItemSize <<= 32;
				ulItemSize += pItem->size;
			}
			else
				ulItemSize = pItem->size;
			ulPartSize = pItem->part_size;
			ulPartSize *= 512;
			if (ulPartSize<ulItemSize)
			{
				CString strText;
				LPWSTR lpwzStr=NULL;
				int nSize;
				BOOL bRet;
				bRet = cmStrCode::AnsiToUnicode(lpwzStr,nSize,pItem->name);
				if (bRet)
				{
					strText.Format(GetLocalString(_T("IDS_CHECKFWPARTITIONSIZEFAIL")),lpwzStr);
				}
				else
					strText.Format(GetLocalString(_T("IDS_CHECKFWPARTITIONSIZEFAIL")),_T("UNKNOWN"));

				if (lpwzStr)
				{
					delete []lpwzStr;
					lpwzStr = NULL;
				}
				if (m_pLog)
				{
					m_pLog->Record(_T("Error:%s size is larger than partition.part=%I64u,size=%I64u"),lpwzStr,ulPartSize,ulItemSize);
				}
				MessageBox(strText,NULL,MB_ICONERROR|MB_OK);
				return FALSE;
			}
		}
	}
	return TRUE;
}
BOOL CFactoryToolDlg:: UpdateFwInfo(CString strFWPath)
{
	BOOL bRet = FALSE;
	CString strInfoText,strButtonTitle;
	
	m_btnFirmware.GetWindowText(strButtonTitle);
	strInfoText = GetLocalString(_T("IDS_LOADING_FW"));
	m_btnFirmware.SetWindowText(strInfoText);

	EnableCtrl(TRUE,FALSE,TRUE,FALSE,TRUE,FALSE,TRUE,FALSE);

	m_pWaitEvent = new CEvent(FALSE,TRUE);
	m_pWaitEvent->ResetEvent();
	m_strFwPath = strFWPath;
	AfxBeginThread(LoadFwThread,(LPVOID)this);
	bRet = WatiObjectWithMsg(&m_pWaitEvent->m_hObject);
	delete m_pWaitEvent;
	m_pWaitEvent =NULL;

	if ((!bRet)||(!m_pImage))
	{
		if (m_pImage)
		{
			delete m_pImage;
			m_pImage = NULL;
		}
		m_edtFirmware.SetWindowText(_T(""));
		m_btnFirmware.SetWindowText(strButtonTitle);
		EnableCtrl(TRUE,TRUE,TRUE,FALSE,TRUE,FALSE,TRUE,TRUE);
		ShowFwInfo();
		strInfoText = GetLocalString(_T("IDS_LOADFWFAIL"));
		if (!strInfoText.IsEmpty())
		{
			MessageBox(strInfoText,_T("ERROR"),MB_ICONERROR|MB_OK);
		}
		else
		{
			MessageBox(_T("Error:Loading firmware failed!"),_T("ERROR"),MB_ICONERROR|MB_OK);
		}
		return FALSE;
	}
	else
	{
		if (!m_pImage->SingleBootFlag)
		{
			if (m_pImage->OsType==ANDROID_OS)
			{
				if (!CheckPartitionSize(m_pImage))
				{
					if (m_pImage)
					{
						delete m_pImage;
						m_pImage = NULL;
					}
					m_edtFirmware.SetWindowText(_T(""));
					m_btnFirmware.SetWindowText(strButtonTitle);
					EnableCtrl(TRUE,TRUE,TRUE,FALSE,TRUE,FALSE,TRUE,TRUE);
					ShowFwInfo();
					return FALSE;
				}
			}
		}
		
		m_btnFirmware.SetWindowText(strButtonTitle);
		m_edtFirmware.SetWindowText(strFWPath);
		m_edtFirmware.SetFocus();
		m_edtFirmware.SetSel(0,-1);
		EnableCtrl(TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE);
		ShowFwInfo();
		return TRUE;
	}

}
VOID CFactoryToolDlg::LoadFwProc()
{
	BOOL bRet;
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
	if (m_bCheckFw)
	{
		bRet = TRUE;
	}
	else
		bRet = FALSE;
	m_pImage = new CRKImage(m_strFwPath,bRet);
	if (!bRet)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("Error:LoadFwProc-->create image objetc failed!"));
		}
		if (m_pImage)
		{
			delete m_pImage;
			m_pImage = NULL;
		}
	}
	
	m_pWaitEvent->SetEvent();
}
VOID CFactoryToolDlg::ShowFwInfo()
{
	if (m_pImage)
	{
		//固件支持芯片
		CString strChip,strVer;
		BYTE btMajor,btMinor;
		USHORT usSmall;
		ENUM_RKDEVICE_TYPE type;
		type = m_pImage->SupportDevice;
		switch ( type )
		{
		case RK27_DEVICE:
			strChip = _T("RK27");
			break;
		case RK28_DEVICE:
			strChip = _T("RK28");
			break;
		case RK281X_DEVICE:
			strChip = _T("RK281X");
			break;
		case RKPANDA_DEVICE:
			strChip = _T("RKPANDA");
			break;
		case RKCROWN_DEVICE:
			strChip = _T("RKCROWN");
			break;
		case RKNANO_DEVICE:
			strChip = _T("RKNANO");
			break;
		case RKSMART_DEVICE:
			strChip = _T("RKSMART");
			break;
		case RKCAYMAN_DEVICE:
			strChip = _T("RKCAYMAN");
			break;
		case RK29_DEVICE:
			strChip = _T("RK29");
			break;
		case RK292X_DEVICE:
			strChip = _T("RK292X");
			break;
		case RK30_DEVICE:
			strChip = _T("RK30");
			break;
		case RK30B_DEVICE:
			strChip = _T("RK30B");
			break;
		case RK31_DEVICE:
			strChip = _T("RK31");
			break;
		case RK32_DEVICE:
			strChip = _T("RK32");
			break;
		default:
			TCHAR szChip[7];
			memset(szChip,0,7*sizeof(TCHAR));
			PBYTE pValue = (PBYTE)(&type);
			szChip[0] = _T('R');
			szChip[1] = _T('K');
			szChip[2] = (TCHAR)(pValue[3]);
			szChip[3] = (TCHAR)(pValue[2]);
			szChip[4] = (TCHAR)(pValue[1]);
			szChip[5] = (TCHAR)(pValue[0]);
			strChip = szChip;
		}
		m_staticFwChip.SetWindowText(m_strFwChipText+strChip);
	
		//固件版本
		if (!m_pImage->SingleBootFlag)
		{
			btMajor = ((m_pImage->Version)&0xFF000000)>>24;
			btMinor = ((m_pImage->Version)&0x00FF0000)>>16;
			usSmall = (m_pImage->Version)&0x0000FFFF;
			if (m_bFwVerHex)
			{
				strVer.Format(_T("%x.%x.%02x"),btMajor,btMinor,usSmall);
			}
			else
				strVer.Format(_T("%u.%u.%02u"),btMajor,btMinor,usSmall);
			m_staticFwVersion.SetWindowText(m_strFwVerText+strVer);
		}
		else
		{//loader
			m_staticFwVersion.SetWindowText(m_strFwVerText);
		}
		
		
		//BOOT版本
		btMajor = ((m_pImage->m_bootObject->Version)&0x0000FF00)>>8;
		btMinor = ((m_pImage->m_bootObject->Version)&0x000000FF);
		strVer.Format(_T("%x.%02x"),btMajor,btMinor);
		m_staticLoaderVer.SetWindowText(m_strLoaderVerText+strVer);
	}
	else
	{	
		m_staticFwChip.SetWindowText(m_strFwChipText);
		m_staticFwVersion.SetWindowText(m_strFwVerText);
		m_staticLoaderVer.SetWindowText(m_strLoaderVerText);
	}
	Invalidate();
	UpdateWindow();
}
void CFactoryToolDlg::SetRKDeviceData(STRUCT_RKDEVICE_DESC &dst,PSTRUCT_RKDEVICE_DESC pSrc)
{
	if (pSrc)
	{
		dst.bUsb20 = pSrc->bUsb20;
		dst.dwDeviceInstance = pSrc->dwDeviceInstance;
		dst.emDeviceType = pSrc->emDeviceType;
		dst.emUsbType = pSrc->emUsbType;
		dst.strLayer = pSrc->strLayer;
		dst.strLinkName = pSrc->strLinkName;
		dst.szDrive = pSrc->szDrive;
		dst.usbcdUsb = pSrc->usbcdUsb;
		dst.usPid = pSrc->usPid;
		dst.usVid = pSrc->usVid;
	}
	else
	{
		dst.bUsb20 = FALSE;
		dst.dwDeviceInstance = 0;
		dst.emDeviceType = RKNONE_DEVICE;
		dst.emUsbType = RKUSB_NONE;
		dst.strLayer = _T("");
		dst.strLinkName = _T("");
		dst.szDrive = 0;
		dst.usbcdUsb = 0;
		dst.usPid = 0;
		dst.usVid = 0;
	}
}
VOID CFactoryToolDlg::InitRunContext()
{
	int i;
	for (i=0;i<MAX_DEVICE;i++)
	{
		m_runContext[i].bRun = FALSE;
		m_runContext[i].bUse = FALSE;
		m_runContext[i].tmStart = 0;
		SetRKDeviceData(m_runContext[i].dev);
	}


}
VOID CFactoryToolDlg::HandleNewDevice(STRUCT_RKDEVICE_DESC &device)
{
	int i,nNextPos=-1;
	if (!m_bRunFlag)
	{//add to backup context
		m_backupDev.push_back(device);
	}
	else
	{
		for (i=0;i<MAX_DEVICE;i++)
		{//check it if use or not
			if (m_runContext[i].bUse)
			{
				if (m_runContext[i].dev.strLayer==device.strLayer)
				{
					break;
				}
			}
			else
			{
				if (nNextPos==-1)
				{
					nNextPos = i;
				}
			}
		}
		if (i>=MAX_DEVICE)
		{//device hasn't use
			if (!m_bMutexResetFlag)
			{//to run
				if (nNextPos==-1)
				{//no context to use
					if (m_pLog)
					{
						m_pLog->Record(_T("<Layer:%s>Error:HandleNewDevice no available context"),device.strLayer);
					}
					m_backupDev.push_back(device);
				}
				else
				{//create upgrade thread
					if ((device.emUsbType==RKUSB_MASKROM)||(device.emUsbType==RKUSB_LOADER)||((m_bSupportMsc)&&((device.emUsbType==RKUSB_MSC)||(device.emUsbType==RKUSB_ADB))))
					{
						m_runContext[nNextPos].bUse = TRUE;
						m_runContext[nNextPos].bRun = TRUE;
						m_runContext[nNextPos].tmStart = 0;
						SetRKDeviceData(m_runContext[nNextPos].dev,&device);
						AfxBeginThread(UpgradeDeviceThread,(LPVOID)nNextPos);
					}
				}
			}
		}		
	}
	/*if (m_pLog)
	{
		m_pLog->Record(_T("<Layer:%s>Error:HandleNewDevice in ,i=%d"),device.strLayer,i);
	}*/
}
VOID CFactoryToolDlg::HandleRemoveDevice(STRUCT_RKDEVICE_DESC &device)
{
	int i;
	
	for (i=0;i<MAX_DEVICE;i++)
	{
		if (m_runContext[i].bUse)
		{
			if (m_runContext[i].dev.strLayer==device.strLayer)
			{
				if (!m_runContext[i].bRun)
				{
					//release context
					m_runContext[i].bUse = FALSE;
					m_runContext[i].tmStart = 0;
					//add backup to run context
					AddBackupToRunContext();
				}
				break;
			}
		}
	}
	/*if (m_pLog)
	{
		m_pLog->Record(_T("<Layer:%s>Error:HandleRemoveDevice in,i=%d"),device.strLayer,i);
	}*/
	if (i>=MAX_DEVICE)
	{//no found it in running context
		for (i=0;i<m_backupDev.size();i++)
		{
			if (device.strLayer==m_backupDev[i].strLayer)
			{
				m_backupDev.erase(m_backupDev.begin()+i);
				break;
			}
		}
	}
	
	
}
VOID PromptInfoProc(DWORD deviceLayer,ENUM_UPGRADE_PROMPT promptID,DWORD oldDeviceLayer)
{
	if (g_pMainDlg)
	{
		g_pMainDlg->HandlePromptCB(deviceLayer,promptID,oldDeviceLayer);
	}
}
VOID ProgressInfoProc(DWORD deviceLayer,ENUM_PROGRESS_PROMPT promptID,ULONG64 totalValue,ULONG64 currentValue,ENUM_CALL_STEP emCall)
{
	if (g_pMainDlg)
	{
		g_pMainDlg->HandleProgressCB(deviceLayer,promptID,totalValue,currentValue,emCall);
	}
}
VOID CFactoryToolDlg::HandlePromptCB(DWORD deviceLayer,ENUM_UPGRADE_PROMPT promptID,DWORD oldDeviceLayer)
{
	CString strLayer,strOldLayer,strInfoText;
	CHAR index;
	if (promptID==MUTEXRESETDEVICE_PASS)
	{
		index = GetIndexFromLayer(oldDeviceLayer);
	}
	else
		index = GetIndexFromLayer(deviceLayer);
	if (index==-1)
	{
		return;
	}
	BOOL bFail=FALSE;
	strLayer = strOldLayer = _T("");
	switch (promptID)
	{
	case DOWNLOADBOOT_START:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADBOOT_START"));
		break;
	case DOWNLOADBOOT_FAIL:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADBOOT_FAIL"));
		bFail = TRUE;
		break;
	case DOWNLOADBOOT_PASS:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADBOOT_PASS"));
		break;
	case DOWNLOADIDBLOCK_START:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADIDBLOCK_START"));
		break;
	case DOWNLOADIDBLOCK_FAIL:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADIDBLOCK_FAIL"));
		bFail = TRUE;
		if (m_bSnDllOn)
		{
			NextSn_End(m_runContext[index].uiSnIndex,FALSE);
		}
		break;
	case DOWNLOADIDBLOCK_PASS:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADIDBLOCK_PASS"));
		if (m_bSnDllOn)
		{
			NextSn_End(m_runContext[index].uiSnIndex,TRUE);
		}
		break;
	case DOWNLOADIMAGE_START:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADIMAGE_START"));
		break;
	case DOWNLOADIMAGE_FAIL:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADIMAGE_FAIL"));
		bFail = TRUE;
		break;
	case DOWNLOADIMAGE_PASS:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADIMAGE_PASS"));
		break;
	case DOWNLOADDEMO_START:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADDEMO_START"));
		break;
	case DOWNLOADDEMO_FAIL:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADDEMO_FAIL"));
		bFail = TRUE;
		break;
	case DOWNLOADDEMO_PASS:
		strInfoText = GetLocalString(_T("IDS_DOWNLOADDEMO_PASS"));
		break;
	case TESTDEVICE_START:
		strInfoText = GetLocalString(_T("IDS_TESTDEVICE_START"));
		break;
	case TESTDEVICE_FAIL:
		strInfoText = GetLocalString(_T("IDS_TESTDEVICE_FAIL"));
		bFail = TRUE;
		break;
	case TESTDEVICE_PASS:
		strInfoText = GetLocalString(_T("IDS_TESTDEVICE_PASS"));
		break;
	case LOWERFORMAT_START:
		strInfoText = GetLocalString(_T("IDS_LOWERFORMATDEVICE_START"));
		break;
	case LOWERFORMAT_FAIL:
		strInfoText = GetLocalString(_T("IDS_LOWERFORMATDEVICE_FAIL"));
		bFail = TRUE;
		break;
	case LOWERFORMAT_PASS:
		strInfoText = GetLocalString(_T("IDS_LOWERFORMATDEVICE_PASS"));
		break;
	case RESETDEVICE_START:
		strInfoText = GetLocalString(_T("IDS_RESETDEVICE_START"));
		break;
	case RESETDEVICE_FAIL:
		strInfoText = GetLocalString(_T("IDS_RESETDEVICE_FAIL"));
		bFail = TRUE;
		break;
	case RESETDEVICE_PASS:
		strInfoText = GetLocalString(_T("IDS_RESETDEVICE_PASS"));
		break;
	case SETRESETFLAG_START:
		strInfoText = GetLocalString(_T("IDS_SETRESETFLAG_START"));
		break;
	case SETRESETFLAG_FAIL:
		strInfoText = GetLocalString(_T("IDS_SETRESETFLAG_FAIL"));
		bFail = TRUE;
		break;
	case SETRESETFLAG_PASS:
		strInfoText = GetLocalString(_T("IDS_SETRESETFLAG_PASS"));
		break;
	case FORMATDISK_START:
		strInfoText = GetLocalString(_T("IDS_FORMATDISK_START"));
		break;
	case FORMATDISK_FAIL:
		strInfoText = GetLocalString(_T("IDS_FORMATDISK_FAIL"));
		bFail = TRUE;
		break;
	case FORMATDISK_PASS:
		strInfoText = GetLocalString(_T("IDS_FORMATDISK_PASS"));
		break;
	case COPYDATA_START:
		strInfoText = GetLocalString(_T("IDS_COPYDATA_START"));
		break;
	case COPYDATA_FAIL:
		strInfoText = GetLocalString(_T("IDS_COPYDATA_FAIL"));
		bFail = TRUE;
		break;
	case COPYDATA_PASS:
		strInfoText = GetLocalString(_T("IDS_COPYDATA_PASS"));
		break;
	case SWITCHMSC_START:
		if(oldDeviceLayer==0)
			strInfoText = GetLocalString(_T("IDS_SWITCHMSC_START"));
		else
			strInfoText = GetLocalString(_T("IDS_SWITCHADB_START"));
		break;
	case SWITCHMSC_FAIL:
		if (oldDeviceLayer==0)
			strInfoText = GetLocalString(_T("IDS_SWITCHMSC_FAIL"));
		else
			strInfoText = GetLocalString(_T("IDS_SWITCHADB_FAIL"));
		bFail = TRUE;
		break;
	case SWITCHMSC_PASS:
		if (oldDeviceLayer==0)
			strInfoText = GetLocalString(_T("IDS_SWITCHMSC_PASS"));
		else
			strInfoText = GetLocalString(_T("IDS_SWITCHADB_PASS"));
		break;
	case RESETMSC_START:
		strInfoText = GetLocalString(_T("IDS_RESETMSC_START"));
		break;
	case RESETMSC_FAIL:
		strInfoText = GetLocalString(_T("IDS_RESETMSC_FAIL"));
		bFail = TRUE;
		break;
	case RESETMSC_PASS:
		strInfoText = GetLocalString(_T("IDS_RESETMSC_PASS"));
		break;
	case WAITMSC_START:
		strInfoText = GetLocalString(_T("IDS_WAITMSC_START"));
		break;
	case WAITMSC_FAIL:
		strInfoText = GetLocalString(_T("IDS_WAITMSC_FAIL"));
		bFail = TRUE;
		break;
	case WAITMSC_PASS:
		strInfoText = GetLocalString(_T("IDS_WAITMSC_PASS"));
		break;
	case WAITLOADER_START:
		strInfoText = GetLocalString(_T("IDS_WAITLOADER_START"));
		break;
	case WAITLOADER_FAIL:
		strInfoText = GetLocalString(_T("IDS_WAITLOADER_FAIL"));
		bFail = TRUE;
		break;
	case WAITLOADER_PASS:
		strInfoText = GetLocalString(_T("IDS_WAITLOADER_PASS"));
		break;
	case WAITMASKROM_START:
		strInfoText = GetLocalString(_T("IDS_WAITMASKROM_START"));
		break;
	case WAITMASKROM_FAIL:
		strInfoText = GetLocalString(_T("IDS_WAITMASKROM_FAIL"));
		bFail = TRUE;
		break;
	case WAITMASKROM_PASS:
		strInfoText = GetLocalString(_T("IDS_WAITMASKROM_PASS"));
		break;
	case ERASEIDB_START:
		strInfoText = GetLocalString(_T("IDS_ERASEIDB_START"));
		break;
	case ERASEIDB_FAIL:
		strInfoText = GetLocalString(_T("IDS_ERASEIDB_FAIL"));
		bFail = TRUE;
		break;
	case ERASEIDB_PASS:
		strInfoText = GetLocalString(_T("IDS_ERASEIDB_PASS"));
		break;
	case SHOWUSERDISK_START:
		strInfoText = GetLocalString(_T("IDS_SHOWUSERDISK_START"));
		break;
	case SHOWUSERDISK_FAIL:
		strInfoText = GetLocalString(_T("IDS_SHOWUSERDISK_FAIL"));
		bFail = TRUE;
		break;
	case SHOWUSERDISK_PASS:
		strInfoText = GetLocalString(_T("IDS_SHOWUSERDISK_PASS"));
		break;
	case CHECKCHIP_START:
		strInfoText = GetLocalString(_T("IDS_CHECKCHIP_START"));
		break;
	case CHECKCHIP_FAIL:
		strInfoText = GetLocalString(_T("IDS_CHECKCHIP_FAIL"));
		bFail = TRUE;
		break;
	case CHECKCHIP_PASS:
		strInfoText = GetLocalString(_T("IDS_CHECKCHIP_PASS"));
		break;
	case PREPAREIDB_START:
		strInfoText = GetLocalString(_T("IDS_PREPAREIDB_START"));
		break;
	case PREPAREIDB_FAIL:
		strInfoText = GetLocalString(_T("IDS_PREPAREIDB_FAIL"));
		bFail = TRUE;
		break;
	case PREPAREIDB_PASS:
		strInfoText = GetLocalString(_T("IDS_PREPAREIDB_PASS"));
		break;
	case GETFLASHINFO_START:
		strInfoText = GetLocalString(_T("IDS_GETFLASHINFO_START"));
		break;
	case GETFLASHINFO_FAIL:
		strInfoText = GetLocalString(_T("IDS_GETFLASHINFO_FAIL"));
		bFail = TRUE;
		break;
	case GETFLASHINFO_PASS:
		strInfoText = GetLocalString(_T("IDS_GETFLASHINFO_PASS"));
		break;
	case MUTEXRESETDEVICE_START:
		strInfoText = GetLocalString(_T("IDS_MUTEXRESETDEVICE_START"));
		m_bMutexResetFlag = TRUE;
		m_bRedLedLight = TRUE;
		Invalidate();
		UpdateWindow();
		break;
	case MUTEXRESETDEVICE_FAIL:
		strInfoText = GetLocalString(_T("IDS_MUTEXRESETDEVICE_FAIL"));
		m_bMutexResetFlag = FALSE;
		m_bRedLedLight = FALSE;
		bFail = TRUE;
		Invalidate();
		UpdateWindow();
		break;
	case MUTEXRESETDEVICE_PASS:
		strInfoText = GetLocalString(_T("IDS_MUTEXRESETDEVICE_PASS"));
		m_bMutexResetFlag = FALSE;
		m_bRedLedLight = FALSE;
		strLayer = GetLayerString(deviceLayer);
		strOldLayer = GetLayerString(oldDeviceLayer);
		m_runContext[index].dev.strLayer = strLayer;
		HTREEITEM hItem;
		hItem = m_treeItemMap[strOldLayer];
		m_treeDeviceInfo.SetItemText(hItem,1,_T(""));
		m_treeDeviceInfo.SetItemText(hItem,2,_T(""));
		m_treeDeviceInfo.SetItemText(hItem,3,_T(""));
		Invalidate();
		UpdateWindow();
		break;
	case ERASEALLBLOCKS_START:
		strInfoText = GetLocalString(_T("IDS_ERASEALLBLOCKS_START"));
		break;
	case ERASEALLBLOCKS_FAIL:
		strInfoText = GetLocalString(_T("IDS_ERASEALLBLOCKS_FAIL"));
		bFail = TRUE;
		break;
	case ERASEALLBLOCKS_PASS:
		strInfoText = GetLocalString(_T("IDS_ERASEALLBLOCKS_PASS"));
		break;
	case FINDUSERDISK_START:
		strInfoText = GetLocalString(_T("IDS_FINDUSERDISK_START"));
		break;
	case FINDUSERDISK_FAIL:
		strInfoText = GetLocalString(_T("IDS_FINDUSERDISK_FAIL"));
		bFail = TRUE;
		break;
	case FINDUSERDISK_PASS:
		strInfoText = GetLocalString(_T("IDS_FINDUSERDISK_PASS"));
		break;
	}
	if (!strInfoText.IsEmpty())
	{
		HTREEITEM hItem;
		hItem = m_treeItemMap[m_runContext[index].dev.strLayer];
		m_treeDeviceInfo.SetItemText(hItem,3,strInfoText);
		DWORD dwID;
		dwID = GetIDFromLayerMap(m_runContext[index].dev.strLayer);
		if (bFail)
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("[Error] Layer<%d,%s>:%s"),dwID,m_runContext[index].dev.strLayer,strInfoText);
			}
			
		}
		else
		{
			if ((!strOldLayer.IsEmpty())&&(!strLayer.IsEmpty()))
			{
				if (m_pLog)
				{
					m_pLog->Record(_T("Layer<%s --> %s>:%s"),strOldLayer,strLayer,strInfoText);
				}
			}
			else
			{
				if (m_pLog)
				{
					m_pLog->Record(_T("Layer<%d,%s>:%s"),dwID,m_runContext[index].dev.strLayer,strInfoText);
				}
			}
		}
	}
	
}
VOID CFactoryToolDlg::HandleProgressCB(DWORD deviceLayer,ENUM_PROGRESS_PROMPT promptID,ULONG64 totalValue,ULONG64 currentValue,ENUM_CALL_STEP emCall)
{
	CString strLayer,strInfoText,strFormatText;
	CHAR index;
	index = g_pMainDlg->GetIndexFromLayer(deviceLayer);
	if (index==-1)
	{
		return;
	}
	int iPercent;
	strInfoText = strFormatText = _T("");
	switch (promptID)
	{
	case TESTDEVICE_PROGRESS:
		if (emCall==CALL_LAST)
		{
			totalValue = currentValue = 100;
		}
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_TESTDEVICEPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case LOWERFORMAT_PROGRESS:
		if (emCall==CALL_LAST)
		{
			totalValue = currentValue = 100;
		}
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_LOWERFORMATPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case DOWNLOADIMAGE_PROGRESS:
// 		currentValue = currentValue/1024;
// 		totalValue = totalValue/1024;
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_DOWNLOADIMAGEPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case CHECKIMAGE_PROGRESS:
// 		currentValue = currentValue/1024;
// 		totalValue = totalValue/1024;
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_CHECKIMAGEPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case TAGBADBLOCK_PROGRESS:
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_TAGBADBLOCKPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case TESTBLOCK_PROGRESS:
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_TESTBLOCKPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case ERASEFLASH_PROGRESS:
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_ERASEFLASHPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case ERASESYSTEM_PROGRESS:
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_ERASESYSTEMPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case ERASEUSERDATA_PROGRESS:
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_ERASEUSERDATAPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case DOWNLOADDEMO_PROGRESS:
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_DOWNLOADDEMOPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	case CHECKDEMO_PROGRESS:
		if (0==totalValue)
		{
			break;
		}
		strFormatText = GetLocalString(_T("IDS_CHECKDEMOPROGRESSINFO"));
		iPercent = currentValue*100/totalValue;
		if (!strFormatText.IsEmpty())
		{
			strInfoText.Format(strFormatText,iPercent);
		}
		break;
	}
	if (!strInfoText.IsEmpty())
	{
		HTREEITEM hItem;
		hItem = m_treeItemMap[m_runContext[index].dev.strLayer];
		m_treeDeviceInfo.SetItemText(hItem,3,strInfoText);
	}
}
CHAR CFactoryToolDlg::GetIndexFromLayer(DWORD dwLayerID)
{
	int i;
	for (i=0;i<MAX_DEVICE;i++)
	{
		if (m_runContext[i].bUse)
		{
			if (GetLayerID(m_runContext[i].dev.strLayer)==dwLayerID)
			{
				break;
			}
		}
	}
	if (i>=MAX_DEVICE)
	{
		return -1;
	}
	else
		return i;
}
BOOL CFactoryToolDlg::IsDeviceAlive(CString strLayer,BOOL &bAlive)
{
	CRKScan *pScan=NULL;
	STRUCT_RKDEVICE_DESC device;
	BOOL bRet;
	int i,nDeviceCount;
	pScan = new CRKScan(m_mscTimeout,m_rockusbTimeout,m_bSupportFullUsb);
	if (!pScan)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Error:IsDeviceAlive-->new scan object failed."));
		}
		return FALSE;
	}
	bAlive = FALSE;
	pScan->SetVidPid(m_mscVid,m_mscPid);
	nDeviceCount = pScan->Search(RKUSB_MASKROM|RKUSB_LOADER|RKUSB_MSC|RKUSB_ADB);
	for (i=0;i<nDeviceCount;i++)
	{
		bRet = pScan->GetDevice(device,i);
		if (bRet)
		{
			if (strLayer==device.strLayer)
			{
				bAlive = TRUE;
				break;
			}
		}
	}
	delete pScan;
	pScan = NULL;
	return TRUE;
}
VOID CFactoryToolDlg::UpgradeThreadProc(int index)
{
	CString strInfoText=_T(""),strCount=_T("");
	CString strLayer;
	PBYTE pSn=NULL;
	DWORD dwLayerID,dwID;
	BYTE uid[RKDEVICE_UID_LEN];
	BOOL bRet,bDevAlive,bSuccess=FALSE,bDemoImage=FALSE,bDemoConfigFile=FALSE;
	UINT uiDemoPartOffset,uiDemoPartSize;
	CRKScan *pScan=NULL;
	CRKComm *pComm=NULL;
	CRKDevice *pDevice=NULL;
	CRKUpgrade *pUpgrade=NULL;
	RK_UPGRADE_SET ctrlStepStruct;
	UINT uiMscOper=MSC_NONE_OPER;
	ENUM_RKDEVICE_TYPE type=m_pImage->SupportDevice;
	HTREEITEM hItem;
	time_t startTime,endTime;
	startTime = time(NULL);
	strLayer = m_runContext[index].dev.strLayer;
	hItem = m_treeItemMap[strLayer];
	if (m_treeDeviceInfo.GetRowBKColor(hItem)!=GetSysColor(COLOR_WINDOW))
	{
		m_treeDeviceInfo.SetRowBKColor(hItem,GetSysColor(COLOR_WINDOW));
	}

	UpdateResultItem(strLayer,0,TRUE);
	UpdateFailItem(strLayer,_T(""),TRUE);

	dwLayerID = GetLayerID(strLayer);
	if ((m_checkDemo.GetCheck())&&(!m_strDemoPath.IsEmpty()))
	{
		if (!IsDemoImage())
		{
			if (cmPath::GetFileExt(m_strDemoPath).CompareNoCase(_T("dcf"))==0)
			{
				bDemoConfigFile = TRUE;
				if (m_dcf.nCount<=0)
				{
					strInfoText = GetLocalString(_T("IDS_ERRCHECKDEMOCONFIGFILE"));
					goto Exit_UpgradeThreadProc;
				}
			}
			else
			{
				uiMscOper = MSC_FORMAT_OPER|MSC_COPY_OPER;
				m_emMiscModifyFlag = MISC_MODIFY_WIPE_DATA;
			}
		}
		else
		{
			bDemoImage = TRUE;
			bRet = CheckDemoImageDownload(uiDemoPartOffset,uiDemoPartSize);
			if (!bRet)
			{
				strInfoText = GetLocalString(_T("IDS_ERRCHECKDEMOIMAGE"));
				goto Exit_UpgradeThreadProc;
			}
		}
	}
	if (m_bSnDllOn)
	{
		pSn = NextSn_Start(m_runContext[index].uiSnIndex);
		if (!pSn)
		{
			strInfoText = GetLocalString(_T("IDS_ERRGETSNFAIL"));
			goto Exit_UpgradeThreadProc;
		}

	}

	ctrlStepStruct.clear();
	if (!m_pImage->SingleBootFlag)
	{
		if ((m_pImage->OsType==ANDROID_OS)&&(uiMscOper!=MSC_NONE_OPER))
		{//增加android情况下msc操作标识
			STRUCT_UPGRADE_STEP step;
			step.uiParam = MSC_ANDROID_OPER;
			ctrlStepStruct.push_back(step);
		}
		if (m_nUpgradeRadio==0)
		{
			bRet = CRKUpgrade::CreateUpgradeStep(type,ctrlStepStruct,uiMscOper);
		}
		else
			bRet = CRKUpgrade::CreateRestoreStep(type,ctrlStepStruct,uiMscOper);
	}
	else
		bRet = CRKUpgrade::CreateDownloadIDBStep(type,ctrlStepStruct);
	
	
	if (!bRet)
	{
		strInfoText = GetLocalString(_T("IDS_ERRBYCREATESTEPFAIL"));
		goto Exit_UpgradeThreadProc;
	}

	if (uiMscOper==MSC_NONE_OPER)
	{//del reset step
		ctrlStepStruct.pop_back();
	}
	
	pScan = new CRKScan(m_mscTimeout,m_rockusbTimeout,m_bSupportFullUsb);
	if (!pScan)
	{
		strInfoText = GetLocalString(_T("IDS_ERRBYCREATESCANFAIL"));
		goto Exit_UpgradeThreadProc;
	}
	pScan->SetVidPid(m_mscVid,m_mscPid);
	
	m_runContext[index].dev.emDeviceType = type;
	pComm = new CRKUsbComm(m_pLog);
	if (!pComm)
	{
		strInfoText = GetLocalString(_T("IDS_ERRBYCREATECOMMFAIL"));
		goto Exit_UpgradeThreadProc;
	}

	pDevice = CDeviceFactory::CreateDevice(m_runContext[index].dev,type,m_pImage->OsType);
	if (!pDevice)
	{
		strInfoText = GetLocalString(_T("IDS_ERRBYCREATEDEVICEFAIL"));
		goto Exit_UpgradeThreadProc;
	}
						
	pDevice->SetObject(m_pImage,pComm,m_pLog);
	if (m_pImage->SingleBootFlag)
	{
		pDevice->GetNewDiskSizeFlag = FALSE;
	}
	pDevice->DiskVolume = m_strDiskVolume;
	pDevice->DataPath = m_strDemoPath;
	pDevice->ForceDataBand = m_uiForceDataBand;
	pDevice->CallBackPointer=ProgressInfoProc;

	pDevice->MiscModifyFlag = m_emMiscModifyFlag;
	
	if (m_bSnDllOn)
	{
		pDevice->SnSize = m_nSnSize;
		
		pDevice->Sn = pSn;
	}

	if (!m_strUidHead.IsEmpty())
	{
		bRet = CreateUid(uid);
		if (bRet)
		{
			pDevice->Uid = uid;
		}
	}
	if (m_nUpgradeRadio==0)
	{
		pUpgrade = new CRKUpgrade(ctrlStepStruct,WF_UPGRADE);
	}
	else
		pUpgrade = new CRKUpgrade(ctrlStepStruct,WF_RESTORE);
	if (!pUpgrade)
	{
		strInfoText = GetLocalString(_T("IDS_ERRBYCREATEUPGRADEFAIL"));
		goto Exit_UpgradeThreadProc;
	}
	pUpgrade->SyncFlag = FALSE;			
	bRet = pUpgrade->SetObject(pDevice,pScan,NULL,NULL,NULL,NULL);
	if (!bRet)
	{
		delete pUpgrade;
		pUpgrade = NULL;
		strInfoText = GetLocalString(_T("IDS_ERRBYSETOBJECTFAIL"));
		goto Exit_UpgradeThreadProc;
	}
	pUpgrade->CallBackPointer=PromptInfoProc;

	//switch msc to rockusb
 	if (m_runContext[index].dev.emUsbType==RKUSB_MSC)
 	{
 		HandlePromptCB(dwLayerID,SWITCHMSC_START,0);
 		bRet = CRKDevice::SendExtCommand(m_runContext[index].dev.szDrive,MSC_SWITCHROCKUSB,m_pLog);
 		if (!bRet)
 		{
			Sleep(m_nSwitchRetryInterval*1000);
			if (m_runContext[index].dev.szDrive==0)
			{
				CRKScan::GetDrive(m_runContext[index].dev);
			}
			bRet = CRKDevice::SendExtCommand(m_runContext[index].dev.szDrive,MSC_SWITCHROCKUSB,m_pLog);
			if (!bRet)
			{
				HandlePromptCB(dwLayerID,SWITCHMSC_FAIL,0);
				strInfoText = _T("");
				goto Exit_UpgradeThreadProc;
			}
 		}
 		HandlePromptCB(dwLayerID,SWITCHMSC_PASS,0);
 		if ((type==RKNANO_DEVICE)||(type==RKSMART_DEVICE))
 		{
 			HandlePromptCB(dwLayerID,WAITMASKROM_START,0);
 			bRet = pScan->Wait(m_runContext[index].dev,RKUSB_MASKROM);
 			if (!bRet)
 			{
 				HandlePromptCB(dwLayerID,WAITMASKROM_FAIL,0);
 				strInfoText = _T("");
 				goto Exit_UpgradeThreadProc;
 			}
 			HandlePromptCB(dwLayerID,WAITMASKROM_PASS,0);
 		}
 		else
 		{
 			HandlePromptCB(dwLayerID,WAITLOADER_START,0);
 			bRet = pScan->Wait(m_runContext[index].dev,RKUSB_LOADER);
 			if (!bRet)
 			{
 				HandlePromptCB(dwLayerID,WAITLOADER_FAIL,0);
 				strInfoText = _T("");
 				goto Exit_UpgradeThreadProc;
 			}
 			HandlePromptCB(dwLayerID,WAITLOADER_PASS,0);
 		}
 		pDevice->VendorID = m_runContext[index].dev.usVid;
 		pDevice->ProductID = m_runContext[index].dev.usPid;
 		pDevice->UsbType = m_runContext[index].dev.emUsbType;
 		pDevice->Drive = m_runContext[index].dev.szDrive;
 		pDevice->LinkName = m_runContext[index].dev.strLinkName;
 		pDevice->LayerName = m_runContext[index].dev.strLayer;
 		pDevice->DeviceInstance = m_runContext[index].dev.dwDeviceInstance;
 		pDevice->BCDUSB = m_runContext[index].dev.usbcdUsb;
 		pDevice->Usb20 = m_runContext[index].dev.bUsb20;
		strLayer = m_runContext[index].dev.strLayer;
		dwLayerID = GetLayerID(strLayer);
 	}
	else if (m_runContext[index].dev.emUsbType==RKUSB_ADB)
	{
		HandlePromptCB(dwLayerID,SWITCHMSC_START,1);
		bRet = AdbToRockusb(m_runContext[index].dev.strLinkName);
		if (!bRet)
		{
			HandlePromptCB(dwLayerID,SWITCHMSC_FAIL,1);
			strInfoText = _T("");
			goto Exit_UpgradeThreadProc;
		}
		HandlePromptCB(dwLayerID,SWITCHMSC_PASS,1);

		HandlePromptCB(dwLayerID,WAITLOADER_START,0);
		bRet = pScan->Wait(m_runContext[index].dev,RKUSB_LOADER);
		if (!bRet)
		{
			HandlePromptCB(dwLayerID,WAITLOADER_FAIL,0);
			strInfoText = _T("");
			goto Exit_UpgradeThreadProc;
		}
		HandlePromptCB(dwLayerID,WAITLOADER_PASS,0);
		pDevice->VendorID = m_runContext[index].dev.usVid;
		pDevice->ProductID = m_runContext[index].dev.usPid;
		pDevice->UsbType = m_runContext[index].dev.emUsbType;
		pDevice->Drive = m_runContext[index].dev.szDrive;
		pDevice->LinkName = m_runContext[index].dev.strLinkName;
		pDevice->LayerName = m_runContext[index].dev.strLayer;
		pDevice->DeviceInstance = m_runContext[index].dev.dwDeviceInstance;
		pDevice->BCDUSB = m_runContext[index].dev.usbcdUsb;
		pDevice->Usb20 = m_runContext[index].dev.bUsb20;
		strLayer = m_runContext[index].dev.strLayer;
		dwLayerID = GetLayerID(strLayer);
	}

	bRet = pUpgrade->Run();
	if (!bRet)
	{
		strInfoText = _T("");
		goto Exit_UpgradeThreadProc;
	}

	if (bDemoImage)
	{
		HandlePromptCB(dwLayerID,DOWNLOADDEMO_START,0);
		bRet = DownloadDemoImage(dwLayerID,pDevice->LinkName,pComm,uiDemoPartOffset);
		if (!bRet)
		{
			HandlePromptCB(dwLayerID,DOWNLOADDEMO_FAIL,0);
			strInfoText = _T("");
			goto Exit_UpgradeThreadProc;
		}
		HandlePromptCB(dwLayerID,DOWNLOADDEMO_PASS,0);
	}
	if (bDemoConfigFile)
	{
		HandlePromptCB(dwLayerID,DOWNLOADDEMO_START,0);
		bRet = DownloadDCF(dwLayerID,pDevice->LinkName,pComm);
		if (!bRet)
		{
			HandlePromptCB(dwLayerID,DOWNLOADDEMO_FAIL,0);
			strInfoText = _T("");
			goto Exit_UpgradeThreadProc;
		}
		HandlePromptCB(dwLayerID,DOWNLOADDEMO_PASS,0);
	}

	if (uiMscOper!=MSC_NONE_OPER)
	{
		if (m_bResetAfterDemo)
		{
			HandlePromptCB(dwLayerID,RESETMSC_START,0);
			bRet = CRKDevice::SendExtCommand(pDevice->Drive,MSC_RESETDEVICE,m_pLog,NULL,1);
			if (!bRet)
			{
				HandlePromptCB(dwLayerID,RESETMSC_FAIL,0);
				strInfoText = _T("");
				goto Exit_UpgradeThreadProc;
			}
			HandlePromptCB(dwLayerID,RESETMSC_PASS,0);
		}
	}
	else
	{
		if (m_bResetAfterUpgrade)
		{//support reset when plug device out,compatible with new and old loader so first disconnect cmd,second reset device
			HandlePromptCB(dwLayerID,RESETDEVICE_START,0);
			if (m_bSupportMsc)
			{
				bRet = pDevice->DisconnectUsbReset();
				if (!bRet)
				{
					HandlePromptCB(dwLayerID,RESETDEVICE_FAIL,0);
					strInfoText = _T("");
					goto Exit_UpgradeThreadProc;
				}
				Sleep(200);
			}
			else
			{
				m_runContext[index].bRun = FALSE;//add this in order to set runcontext's bUse flag to false when removing device 
				if (m_bLoopTest)
				{
					m_runContext[index].tmStart = time(NULL);
				}
			}
			
			bRet = pDevice->ResetDevice();
			if (!bRet)
			{
				HandlePromptCB(dwLayerID,RESETDEVICE_FAIL,0);
				strInfoText = _T("");
				goto Exit_UpgradeThreadProc;
			}
			HandlePromptCB(dwLayerID,RESETDEVICE_PASS,0);
		}

	}

	if (m_nUpgradeRadio==0)
	{
		strInfoText = GetLocalString(_T("IDS_UPGRADE_SUCCESS"));
	}
	else
		strInfoText = GetLocalString(_T("IDS_RESTORE_SUCCESS"));
	endTime = time(NULL);
	bSuccess = TRUE;
Exit_UpgradeThreadProc:
	
	if (pUpgrade)
	{
		delete pUpgrade;
		pUpgrade = NULL;
	}
	else
	{
		if (pDevice)
		{
			delete pDevice;
			pDevice = NULL;
		}
		else
		{
			if (pComm)
			{
				delete pComm;
				pComm = NULL;
			}
		}
		if (pScan)
		{
			delete pScan;
			pScan = NULL;
		}
	}
	m_runContext[index].bRun = FALSE;
	dwID = GetIDFromLayerMap(strLayer);

	if (bSuccess)
	{
		UpdateResultItem(strLayer,endTime-startTime);
		if ((uiMscOper==MSC_NONE_OPER)&&(m_bResetAfterUpgrade)&&(!m_bSupportMsc))
		{//clear upgrade info because of device reset
			m_treeDeviceInfo.SetItemText(hItem,3,_T(""));
		}
		else
		{
			m_treeDeviceInfo.SetRowBKColor(hItem,RGB(0,255,0));
			m_treeDeviceInfo.SetItemText(hItem,3,strInfoText);
		}
		m_csUpdateCount.Lock();
		m_staticTotalCount.GetWindowText(strCount);
		m_staticTotalCount.SetWindowText(cmNumString::IntStrIncrease(strCount));
		m_staticTotalCount.Invalidate();
		m_staticTotalCount.UpdateWindow();
		m_staticSuccessCount.GetWindowText(strCount);
		m_staticSuccessCount.SetWindowText(cmNumString::IntStrIncrease(strCount));
		m_staticSuccessCount.Invalidate();
		m_staticSuccessCount.UpdateWindow();
		m_csUpdateCount.Unlock();
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%d,%s>:%s"),dwID,strLayer,strInfoText);
		}
	}
	else
	{
		m_csUpdateCount.Lock();
		m_staticTotalCount.GetWindowText(strCount);
		m_staticTotalCount.SetWindowText(cmNumString::IntStrIncrease(strCount));
		m_staticTotalCount.Invalidate();
		m_staticTotalCount.UpdateWindow();
		m_staticFailCount.GetWindowText(strCount);
		m_staticFailCount.SetWindowText(cmNumString::IntStrIncrease(strCount));
		m_staticFailCount.Invalidate();
		m_staticFailCount.UpdateWindow();
		m_csUpdateCount.Unlock();
		
		if (!strInfoText.IsEmpty())
		{
			m_treeDeviceInfo.SetRowBKColor(hItem,RGB(255,0,0));
			m_treeDeviceInfo.SetItemText(hItem,3,strInfoText);
			if (m_pLog)
			{
				m_pLog->Record(_T("[Error] Layer<%d,%s>:%s"),dwID,strLayer,strInfoText);
			}
		}
		else
		{
			strInfoText = m_treeDeviceInfo.GetItemText(hItem,3);
			bRet = IsDeviceAlive(strLayer,bDevAlive);
			if ((bRet)&&(!bDevAlive))
			{
				m_runContext[index].bUse = FALSE;
				m_treeDeviceInfo.SetItemText(hItem,3,_T(""));
			}
			else
				m_treeDeviceInfo.SetRowBKColor(hItem,RGB(255,0,0));
		}
		UpdateFailItem(strLayer,strInfoText);
		//strInfoText = m_treeDeviceInfo.GetItemText(hItem,3);
		//if ((strInfoText.CompareNoCase(GetLocalString(_T("IDS_WAITLOADER_FAIL")))==0) ||
		//	(strInfoText.CompareNoCase(GetLocalString(_T("IDS_WAITMASKROM_FAIL")))==0) ||
		//	(strInfoText.CompareNoCase(GetLocalString(_T("IDS_WAITMSC_FAIL")))==0) )
		//{//when waiting device failed,rescan 
		//	m_bScanDevice = TRUE;
		//}
	}
}
int CFactoryToolDlg::RandomInteger(int low, int high)
{
	int k;
	double d;
	
	d = (double)rand() / ((double)RAND_MAX + 1);
	k = (int)(d * (high - low + 1));
	return (low+k);
}
BOOL CFactoryToolDlg::CreateUid(PBYTE pUid)
{
	if (!m_bUidCreate)
	{
		return FALSE;
	}
	if (!pUid)
	{
		return FALSE;
	}
	memset(pUid,0,RKDEVICE_UID_LEN);
	char *pszUid=NULL;
	int nSize;
	BOOL bRet;
	bRet = cmStrCode::UnicodeToAnsi(pszUid,nSize,m_strUidHead);
	if (!bRet)
	{
		return FALSE;
	}
	
	PUCHAR pManufactory,pTime,pGuid,pCrc;
	pManufactory = pUid;
	pTime = pManufactory + 8;
	pGuid = pTime + 4;
	pCrc = pGuid + 16;
	int iStringLen;
	iStringLen = m_strUidHead.GetLength();
	if (iStringLen>=8)
	{
		memcpy(pManufactory,pszUid,8);
	}
	else
	{
		if (iStringLen>0)
		{
			memcpy(pManufactory,pszUid,iStringLen);
		}
		int i,j;
		UCHAR randomValue;
		for (i=8-iStringLen,j=0;i>0;i--,j++)
		{
			randomValue = RandomInteger(0,0xFF);
			*(pManufactory+iStringLen+j)=randomValue;
		}
	}
	delete []pszUid;
	pszUid = NULL;
	time_t now;
	now = time(NULL);
	memcpy(pTime,(BYTE *)&now,4);
	GUID guidValue;
	HRESULT hr;
	hr = CoCreateGuid(&guidValue);
	if (hr!=S_OK)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("ERROR:CreateUid-->CoCreateGuid failed(0x%x)"),hr);
		}
		return FALSE;
	}

	memcpy(pGuid,(BYTE *)&guidValue,16);
	
	USHORT usCrc=0;
	usCrc = CRC_CCITT(pManufactory,28);
	memcpy(pCrc,(BYTE *)&usCrc,2);
	return TRUE;
	
}
void CFactoryToolDlg::ReRunContext()
{
	int j;
	time_t tmNow;
	for (j=0;j<MAX_DEVICE;j++)
	{
		if (m_runContext[j].bUse)
		{
			if ((m_runContext[j].tmStart>0)&&(!m_runContext[j].bRun))
			{
				tmNow = time(NULL);
				if ((tmNow-m_runContext[j].tmStart)>=m_nRestartTimeout)
				{
					if ((m_runContext[j].dev.emUsbType==RKUSB_MASKROM)||(m_runContext[j].dev.emUsbType==RKUSB_LOADER)||((m_bSupportMsc)&&((m_runContext[j].dev.emUsbType==RKUSB_MSC)||(m_runContext[j].dev.emUsbType==RKUSB_ADB))))
					{
						m_runContext[j].bRun = TRUE;
						m_runContext[j].tmStart = 0;
						AfxBeginThread(UpgradeDeviceThread,(LPVOID)j);
					}
				}
			}	
		}
	}
}
void CFactoryToolDlg::AddBackupToRunContext()
{
	int j;
	BOOL bNoContext;
	while(m_backupDev.size()>0)
	{
		bNoContext = TRUE;
		for (j=0;j<MAX_DEVICE;j++)
		{
			if (!m_runContext[j].bUse)
			{
				bNoContext = FALSE;
				if ((m_backupDev[0].emUsbType==RKUSB_MASKROM)||(m_backupDev[0].emUsbType==RKUSB_LOADER)||((m_bSupportMsc)&&((m_backupDev[0].emUsbType==RKUSB_MSC)||(m_backupDev[0].emUsbType==RKUSB_ADB))))
				{
					m_runContext[j].bUse = TRUE;
					m_runContext[j].bRun = TRUE;
					m_runContext[j].tmStart = 0;
					SetRKDeviceData(m_runContext[j].dev,&m_backupDev[0]);
					m_backupDev.erase(m_backupDev.begin());
					AfxBeginThread(UpgradeDeviceThread,(LPVOID)j);
				}
				else
					m_backupDev.erase(m_backupDev.begin());
				break;
			}
		}
		if (bNoContext)
		{
			return;
		}
	}
}
void CFactoryToolDlg::OnGridGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString strText;
	VG_DISPINFO *pDispInfo = (VG_DISPINFO *)pNMHDR;
	if (pDispInfo->hdr.idFrom==IDC_OK_GRID)
	{
		m_csResultItems.Lock();
		if (pDispInfo->item.nRow<m_resultItems.size())
		{
			if (!m_resultItems[pDispInfo->item.nRow].strLayer.IsEmpty())
			{
				if (pDispInfo->item.mask & LVIF_TEXT)
				{
					switch (pDispInfo->item.nColumn)  
					{
					case 0:		//ID
						DWORD dwID;
						dwID = GetIDFromLayerMap(m_resultItems[pDispInfo->item.nRow].strLayer);
						pDispInfo->item.strText = cmNumString::NumToStr(dwID,10);
						break;
					case 1:		//Elapsed Time
						DWORD dwMin,dwSec;
						dwMin = m_resultItems[pDispInfo->item.nRow].dwElapseSecond / 60;
						dwSec = m_resultItems[pDispInfo->item.nRow].dwElapseSecond % 60;
						strText.Format(_T("%d:%d"),dwMin,dwSec);
						pDispInfo->item.strText = strText;
						break;
					default:
						break;
					}
				}
				else if (pDispInfo->item.mask & LVIF_COLOR)  
				{
					pDispInfo->item.pDC->SetBkColor(RGB(0, 255, 0));
				}
			}
		}
		m_csResultItems.Unlock();
	}
	else if (pDispInfo->hdr.idFrom==IDC_FAIL_GRID)
	{
		m_csFailItems.Lock();
		if (pDispInfo->item.nRow<m_failItems.size())
		{
			if (!m_failItems[pDispInfo->item.nRow].strLayer.IsEmpty())
			{
				if (pDispInfo->item.mask & LVIF_TEXT)
				{
					switch (pDispInfo->item.nColumn)  
					{
					case 0:		//ID
						DWORD dwID;
						dwID = GetIDFromLayerMap(m_failItems[pDispInfo->item.nRow].strLayer);
						pDispInfo->item.strText = cmNumString::NumToStr(dwID,10);
						break;
					case 1:		//Error Text
						pDispInfo->item.strText = m_failItems[pDispInfo->item.nRow].strErrorText;
						break;
					default:
						break;
					}
				}
				else if (pDispInfo->item.mask & LVIF_COLOR)  
				{
					pDispInfo->item.pDC->SetBkColor(RGB(255, 0, 0));
				}
			}
		}
		m_csFailItems.Unlock();
	}
}


void CFactoryToolDlg::OnGridSetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{

	//VG_DISPINFO *pDispInfo = (VG_DISPINFO *)pNMHDR;

	//if (pDispInfo->hdr.idFrom==IDC_OK_GRID)
	//{
	//	;
	//}
	*pResult = 0;
}
void CFactoryToolDlg::UpdateFailItem(CString strLayer,CString strError,BOOL bRemove)
{
	int i;
	BOOL bSetRowCount=FALSE;
	m_csFailItems.Lock();
	if (bRemove)
	{
		for(i=0;i<m_failItems.size();i++)
		{
			if (m_failItems[i].strLayer==strLayer)
			{
				m_failItems.erase(m_failItems.begin()+i);
				bSetRowCount = TRUE;
				break;
			}
		}
	}
	else
	{
		STRUCT_FAIL_ITEM item;
		item.strLayer = strLayer;
		item.strErrorText = strError;
		m_failItems.push_back(item);
		bSetRowCount = TRUE;
	}
	m_csFailItems.Unlock();
	if (bSetRowCount)
	{
		PostMessage(WM_UPDATE_GRID_MSG,0,1);
		/*m_failGrid.SetRowCount(m_failItems.size());
		m_failGrid.Invalidate();
		m_failGrid.UpdateWindow();*/
	}
	
	
}
void CFactoryToolDlg::UpdateResultItem(CString strLayer,DWORD dwElapseSecond,BOOL bRemove)
{
	int i;
	BOOL bSetRowCount=FALSE;
	m_csResultItems.Lock();
	if (bRemove)
	{
		for(i=0;i<m_resultItems.size();i++)
		{
			if (m_resultItems[i].strLayer==strLayer)
			{
				m_resultItems.erase(m_resultItems.begin()+i);
				bSetRowCount = TRUE;
				break;
			}
		}
	}
	else
	{
		STRUCT_RESULT_ITEM item;
		item.strLayer = strLayer;
		item.dwElapseSecond = dwElapseSecond;
		m_resultItems.push_back(item);
		bSetRowCount = TRUE;
	}
	m_csResultItems.Unlock();
	if (bSetRowCount)
	{
		PostMessage(WM_UPDATE_GRID_MSG);
		/*m_resultGrid.SetRowCount(m_resultItems.size());
		m_resultGrid.Invalidate();
		m_resultGrid.UpdateWindow();*/
	}
	
}
BOOL GetExeVersion(CString &strVer)
{
	int iVerInfoSize;
    char *pBuf;
    VS_FIXEDFILEINFO *pVsInfo;
    unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
	DWORD dwHandle;
	CString strFile;
	strVer = _T("");
	GetModuleFileName(NULL, strFile.GetBuffer(MAX_PATH), MAX_PATH);
	strFile.ReleaseBuffer();
    iVerInfoSize = GetFileVersionInfoSize((LPTSTR)(LPCTSTR)strFile,&dwHandle);
	
    if(iVerInfoSize!=0)
    {
        pBuf = new char[iVerInfoSize];
        if( GetFileVersionInfo((LPTSTR)(LPCTSTR)strFile,0,iVerInfoSize,pBuf) )
        {
            if(VerQueryValue(pBuf,_T("\\"),(void **)&pVsInfo,&iFileInfoSize))
            {
                strVer.Format(_T(" v%d.%d.%d.%d"),HIWORD(pVsInfo->dwFileVersionMS),LOWORD(pVsInfo->dwFileVersionMS),HIWORD(pVsInfo->dwFileVersionLS),LOWORD(pVsInfo->dwFileVersionLS));
				delete []pBuf;
				return TRUE;
            }
        }
        delete []pBuf;
    }
    return FALSE;
}
LRESULT CFactoryToolDlg::OnHandleLoadFwMsg(WPARAM wParam,LPARAM lParam)
{
	UpdateFwInfo(m_strDefaultFirmware);
	return 0;
}
LRESULT CFactoryToolDlg::OnHandleUpdateGridMsg(WPARAM wParam,LPARAM lParam)
{
	if (lParam==0)
	{
		m_resultGrid.SetRowCount(m_resultItems.size());
		m_resultGrid.Invalidate();
		m_resultGrid.UpdateWindow();
	}
	else
	{
		m_failGrid.SetRowCount(m_failItems.size());
		m_failGrid.Invalidate();
		m_failGrid.UpdateWindow();
	}
	return 0;
}

BOOL CFactoryToolDlg::LoadLayerMap(LAYER_MAP &layerMap)
{
	CRegKey layerKey;
	int iRet,index;
	LAYER_MAP::iterator iter;
	layerMap.clear();
	m_layerID = 0;
	iRet = layerKey.Create(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\FactoryTool"));
	if (iRet!=ERROR_SUCCESS)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record(_T("Error:LoadLayerMap Create or Open Key failed,err=%d"),GetLastError());
		}
		return FALSE;
	}
	DWORD dwNameSize,dwValueSize,dwValueType,dwValue;
	CString strName;
	index = 0;
	dwNameSize = MAX_PATH;
	dwValueSize = 4;

	while (TRUE)
	{
		iRet = RegEnumValue(layerKey.m_hKey,index,strName.GetBuffer(MAX_PATH),&dwNameSize,NULL,&dwValueType,(LPBYTE)&dwValue,&dwValueSize); 
		if (iRet!=ERROR_SUCCESS)
		{
			break;
		}
		strName.ReleaseBuffer();
		layerMap[strName] = dwValue;
		index++;
	}
	layerKey.Close();
	for (iter=layerMap.begin();iter!=layerMap.end();iter++)
	{
		if (iter->second>m_layerID)
		{
			m_layerID = iter->second;
		}
	}
	return TRUE;

}
DWORD CFactoryToolDlg::GetIDFromLayerMap(CString strLayer)
{
	LAYER_MAP::iterator iter;
	iter = m_layerMap.find(strLayer);
	if (iter==m_layerMap.end())
	{//add new item
		m_layerID++;
		m_layerMap[strLayer] = m_layerID;
		CRegKey layerKey;
		int iRet;
		iRet = layerKey.Create(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\FactoryTool"));
		if (iRet!=ERROR_SUCCESS)
		{
			if (g_pLogObject)
			{
				g_pLogObject->Record(_T("Error:GetIDFromLayerMap Create or Open Key failed,err=%d"),GetLastError());
			}
			return m_layerID;
		}
		
		iRet = layerKey.SetValue(m_layerID,strLayer);
		layerKey.Close();
		if (iRet!=ERROR_SUCCESS)
		{
			if (g_pLogObject)
			{
				g_pLogObject->Record(_T("Error:GetIDFromLayerMap Set Key value failed,err=%d"),GetLastError());
			}
			return m_layerID;
		}
		return m_layerID;
	}
	else
		return iter->second;
}
BOOL CFactoryToolDlg::IsDemoImage()
{
	CFile file;
	STRUCT_DEMOIMAGE_HDR hdr;
	int nRead;
	BOOL bRet;
	bRet = file.Open(m_strDemoPath,CFile::typeBinary|CFile::modeRead|CFile::shareDenyWrite);
	if (!bRet)
	{
		return FALSE;
	}
	nRead = file.Read(&hdr,sizeof(STRUCT_DEMOIMAGE_HDR));
	if (nRead!=sizeof(STRUCT_DEMOIMAGE_HDR))
	{
		file.Close();
		return FALSE;
	}
	file.Close();
	if (hdr.uiTag==0x4F4D4544)
	{
		return TRUE;
	}
	else
		return FALSE;
}
BOOL CFactoryToolDlg::CheckDemoImageDownload(UINT &uiDemoPartOffset,UINT &uiDemoPartSize)
{
	CFile file;
	STRUCT_DEMOIMAGE_HDR hdr;
	ULONGLONG ullPartSize;
	int nRead;
	BOOL bRet;
	bRet = file.Open(m_strDemoPath,CFile::typeBinary|CFile::modeRead|CFile::shareDenyWrite);
	if (!bRet)
	{
		return FALSE;
	}
	nRead = file.Read(&hdr,sizeof(STRUCT_DEMOIMAGE_HDR));
	if (nRead!=sizeof(STRUCT_DEMOIMAGE_HDR))
	{
		file.Close();
		return FALSE;
	}
	file.Close();
	bRet = GetPartitionInfo(m_pImage,(char *)hdr.ucPartition,uiDemoPartOffset,uiDemoPartSize);
	if (!bRet)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Error:CheckDemoImageDownload-->GetPartitionInfo failed"));
		}
		return FALSE;
	}
	if (uiDemoPartSize!=-1)
	{
		ullPartSize = uiDemoPartSize * 512;
		if (ullPartSize<hdr.ullImageSize)
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("Error:CheckDemoImageDownload-->Demo is larger than partition"));
			}
			return FALSE;
		}
	}
	return TRUE;
}
BOOL CFactoryToolDlg::HandleDemoMiscUser(DWORD dwLayer,CString strLinkName,CRKComm *pComm)
{
	CString strLayer,strFile;
	BYTE srcbuf[4096],dstbuf[4096];
	STRUCT_BOOTLOADER_MSG *pSrcMsg,*pDstMsg;
	PCHAR pToken=NULL,pPos=NULL;
	BOOL bRet,bWriteMisc=FALSE;
	int iRet,nStrSize;
	UINT uiMiscOffset,uiMiscSize;
	SYSTEMTIME	now;
	TCHAR szDateTime[100];
	strLayer = GetLayerString(dwLayer);
	pSrcMsg = (PSTRUCT_BOOTLOADER_MSG)srcbuf;
	pDstMsg = (PSTRUCT_BOOTLOADER_MSG)dstbuf;

	bRet = GetPartitionInfo(m_pImage,"misc",uiMiscOffset,uiMiscSize);
	if (!bRet)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscUser-->GetPartitionInfo misc failed"),strLayer);
		}
		return FALSE;
	}
	memset(srcbuf,0,4096);
	memset(dstbuf,0,4096);
	iRet = pComm->RKU_ReadLBA(strLinkName,uiMiscOffset+32,4,srcbuf);
	if (iRet!=ERR_SUCCESS)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscUser-->RKU_ReadLBA failed,RetCode(%d)"),strLayer,iRet);
		}
		return FALSE;
	}

	if (stricmp(pSrcMsg->command,"boot-recovery")==0)
	//{
	//	strcpy(pDstMsg->command,"boot-recovery");
	//	strcpy(pDstMsg->recovery,"recovery\n--wipe_data");
	//}
	//else
	{
		strcpy(pDstMsg->command,"boot-recovery");
		pPos = pDstMsg->recovery;
		pToken = strtok(pSrcMsg->recovery,"\n");
		while(pToken)
		{
			nStrSize = strlen(pToken);
			if (stricmp(pToken,"--wipe_all")==0)
			{
				nStrSize = strlen("--wipe_data");
				strncpy(pPos,"--wipe_data",nStrSize);
				bWriteMisc = TRUE;
			}
			else
				strncpy(pPos,pToken,nStrSize);
			pPos += nStrSize;
			*pPos = '\n';
			pPos++;
			pToken = strtok(NULL,"\n");
		}
		nStrSize = strlen(pDstMsg->recovery);
		if (pDstMsg->recovery[nStrSize-1]=='\n')
		{
			pDstMsg->recovery[nStrSize-1] = 0;
		}
	}
	if (bWriteMisc)
	{
		iRet = pComm->RKU_WriteLBA(strLinkName,uiMiscOffset+32,4,dstbuf);
		if (iRet!=ERR_SUCCESS)
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscUser-->RKU_WriteLBA failed,RetCode(%d)"),strLayer,iRet);
			}
			return FALSE;
		}
		iRet = pComm->RKU_ReadLBA(strLinkName,uiMiscOffset+32,4,srcbuf);
		if (iRet!=ERR_SUCCESS)
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscUser-->RKU_ReadLBA failed,RetCode(%d)"),strLayer,iRet);
			}
			return FALSE;
		}
		if (memcmp(srcbuf,dstbuf,2048)!=0)
		{
			if (m_pLog)
			{
				GetLocalTime(&now);
				_stprintf(szDateTime,_T("%02d-%02d-%02d"),now.wHour,now.wMinute,now.wSecond);
				strFile = m_pLog->LogSavePath;
				strFile += szDateTime;
				strFile += _T("file.bin");
				m_pLog->SaveBuffer( strFile,dstbuf,2048 );
				strFile = m_pLog->LogSavePath;
				strFile += szDateTime;
				strFile += _T("flash.bin");
				m_pLog->SaveBuffer( strFile,srcbuf,2048 );
				m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscUser-->memcmp failed!"),strLayer);
			}
			return FALSE;
		}
	}
	
	return TRUE;
}
BOOL CFactoryToolDlg::HandleDemoMiscExt4(DWORD dwLayer,CString strLinkName,CRKComm *pComm,char *szPartition)
{
	CString strLayer,strFile;
	BYTE srcbuf[4096],dstbuf[4096];
	STRUCT_BOOTLOADER_MSG *pSrcMsg,*pDstMsg;
	PCHAR pToken=NULL,pPos=NULL;
	BOOL bRet,bDelWipeCmd=FALSE;
	int iRet,nStrSize;
	UINT uiMiscOffset,uiMiscSize;
	SYSTEMTIME	now;
	TCHAR szDateTime[100];
	strLayer = GetLayerString(dwLayer);
	pSrcMsg = (PSTRUCT_BOOTLOADER_MSG)srcbuf;
	pDstMsg = (PSTRUCT_BOOTLOADER_MSG)dstbuf;
	bRet = GetPartitionInfo(m_pImage,"misc",uiMiscOffset,uiMiscSize);
	if (!bRet)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscExt4-->GetPartitionInfo misc failed"),strLayer);
		}
		return FALSE;
	}
	if (stricmp(szPartition,"userdata")==0)
	{
		bDelWipeCmd = TRUE;
	}
	memset(srcbuf,0,4096);
	memset(dstbuf,0,4096);
	iRet = pComm->RKU_ReadLBA(strLinkName,uiMiscOffset+32,4,srcbuf);
	if (iRet!=ERR_SUCCESS)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscExt4-->RKU_ReadLBA failed,RetCode(%d)"),strLayer,iRet);
		}
		return FALSE;
	}
	if (stricmp(pSrcMsg->command,"boot-recovery")!=0)
	{
		strcpy(pDstMsg->command,"boot-recovery");
		sprintf(pDstMsg->recovery,"recovery\n--resize_partition=%s",szPartition);
	}
	else
	{
		strcpy(pDstMsg->command,pSrcMsg->command);
		if (strncmp(pSrcMsg->recovery,"recovery",8)==0)
		{
			strcpy(pDstMsg->recovery,"recovery");
			nStrSize = strlen(pDstMsg->recovery);
			pPos = pDstMsg->recovery+nStrSize;
			sprintf(pPos,"\n--resize_partition=%s\n",szPartition);
			nStrSize = strlen(pDstMsg->recovery);
			pPos = pDstMsg->recovery+nStrSize;
			pToken = strtok(pSrcMsg->recovery+8,"\n");
			while(pToken)
			{
				nStrSize = strlen(pToken);
				if (nStrSize>0)
				{
					if (bDelWipeCmd)
					{
						if ((stricmp(pToken,"--wipe_all")!=0)&&(stricmp(pToken,"--wipe_data")!=0))
						{
							strncpy(pPos,pToken,nStrSize);
							pPos += nStrSize;
							*pPos = '\n';
							pPos++;
						}
					}
					else
					{
						strncpy(pPos,pToken,nStrSize);
						pPos += nStrSize;
						*pPos = '\n';
						pPos++;
					}
				}
				pToken = strtok(NULL,"\n");
			}
			nStrSize = strlen(pDstMsg->recovery);
			if (pDstMsg->recovery[nStrSize-1]=='\n')
			{
				pDstMsg->recovery[nStrSize-1] = 0;
			}
		}
		else
		{
			sprintf(pDstMsg->recovery,"recovery\n--resize_partition=%s",szPartition);
		}
		
	}
	
	iRet = pComm->RKU_WriteLBA(strLinkName,uiMiscOffset+32,4,dstbuf);
	if (iRet!=ERR_SUCCESS)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscExt4-->RKU_WriteLBA failed,RetCode(%d)"),strLayer,iRet);
		}
		return FALSE;
	}
	iRet = pComm->RKU_ReadLBA(strLinkName,uiMiscOffset+32,4,srcbuf);
	if (iRet!=ERR_SUCCESS)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscExt4-->RKU_ReadLBA failed,RetCode(%d)"),strLayer,iRet);
		}
		return FALSE;
	}
	if (memcmp(srcbuf,dstbuf,2048)!=0)
	{
		if (m_pLog)
		{
			GetLocalTime(&now);
			_stprintf(szDateTime,_T("%02d-%02d-%02d"),now.wHour,now.wMinute,now.wSecond);
			strFile = m_pLog->LogSavePath;
			strFile += szDateTime;
			strFile += _T("file.bin");
			m_pLog->SaveBuffer( strFile,dstbuf,2048 );
			strFile = m_pLog->LogSavePath;
			strFile += szDateTime;
			strFile += _T("flash.bin");
			m_pLog->SaveBuffer( strFile,srcbuf,2048 );
			m_pLog->Record(_T("Layer<%s> ERROR:HandleDemoMiscExt4-->memcmp failed!"),strLayer);
		}
		return FALSE;
	}
	return TRUE;
}
BOOL CFactoryToolDlg::DownloadDemoImage(DWORD dwLayer,CString strLinkName,CRKComm *pComm,UINT uiPartOffset)
{
	int iRet;
	ULONGLONG iFileSize;
	BOOL bSuccess=FALSE,bNewLine=FALSE,bRet,bUser=FALSE;
	CFile file;
	DWORD dwTotal,dwCurrent;
	DWORD dwMaxReadWriteSectors=32;
	DWORD dwMaxReadWriteBytes ;
	PBYTE pBuf=NULL ;
	PBYTE pRead=NULL ;
	PBYTE pDbr,pCopyDbr;
	UINT  uiBegin,uiRead,uiTransferSec;
	ULONGLONG dwTotalWritten,dwTotalRead;
	DWORD dwTransferBytes,dwLoopBytes,dwLoopConstant=1024*1024;
	ULONGLONG dwFWSize;
	TCHAR szDateTime[100];
	CString strFile,strPromptText,strFormatText,strLayer;
	STRUCT_DEMOIMAGE_HDR hdr;
	ENUM_CALL_STEP emCallStep;
	UINT uiUserSectors;
	SYSTEMTIME	now;
	file.m_hFile = INVALID_HANDLE_VALUE;
	strLayer = GetLayerString(dwLayer);
	bRet = file.Open(m_strDemoPath,CFile::typeBinary|CFile::modeRead|CFile::shareDenyWrite);
	if( !bRet )
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->open %s failed,err=%d"),strLayer,m_strDemoPath,GetLastError());
		}
		goto Exit_DownloadDemoImage;
	}
	uiRead = file.Read(&hdr,sizeof(STRUCT_DEMOIMAGE_HDR));
	if (uiRead!=sizeof(STRUCT_DEMOIMAGE_HDR))
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->read %s failed,err=%d"),strLayer,m_strDemoPath,GetLastError());
		}
		goto Exit_DownloadDemoImage;
	}
	if (stricmp((char *)hdr.ucPartition,"user")==0)
	{
		bUser = TRUE;
	}

	if (m_pLog)
	{
		m_pLog->Record(_T("Layer<%s> INFO:Start to download demo,offset=0x%x,size=%I64u"),strLayer,uiPartOffset,hdr.ullImageSize);
	}

	iRet = pComm->RKU_TestDeviceReady(strLinkName,&dwTotal,&dwCurrent);
	if (iRet!=ERR_SUCCESS)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->RKU_TestDeviceReady failed,err=%d"),strLayer,iRet);
		}
		goto Exit_DownloadDemoImage;
	}
	dwMaxReadWriteSectors = dwMaxReadWriteSectors * (1<<dwCurrent);
	dwMaxReadWriteBytes = dwMaxReadWriteSectors * SECTOR_SIZE;
	pBuf = new BYTE[dwMaxReadWriteBytes];
	pRead = new BYTE[dwMaxReadWriteBytes];

	
	iFileSize = hdr.ullImageSize;
	dwFWSize = iFileSize;
	dwTotalWritten = 0;
	dwLoopBytes = 0;
	uiBegin = uiPartOffset;
	emCallStep = CALL_FIRST;
	while(dwTotalWritten<dwFWSize) 
	{
		memset(pBuf, 0, dwMaxReadWriteBytes);
		if ((dwFWSize-dwTotalWritten)>=dwMaxReadWriteBytes)
		{
			dwTransferBytes = dwMaxReadWriteBytes;
			uiTransferSec = dwMaxReadWriteSectors;
		}
		else
		{
			dwTransferBytes = dwFWSize - dwTotalWritten;
			uiTransferSec = ( (dwTransferBytes%SECTOR_SIZE==0) ? (dwTransferBytes/SECTOR_SIZE) : (dwTransferBytes/SECTOR_SIZE+1) );
		}
		uiRead = file.Read(pBuf,dwTransferBytes);
		if (uiRead!=dwTransferBytes)
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->Read data from file failed in write file,err=%d,read=%d,ret=%d"),strLayer,GetLastError(),dwTransferBytes,uiRead);
			}
			goto Exit_DownloadDemoImage;
		}
		if (dwTotalWritten==0)
		{
			if (bUser)
			{

				iRet = pComm->RKU_TestDeviceReady(strLinkName,(DWORD *)&uiUserSectors,NULL,TU_GETUSERSECTOR_SUBCODE);
				if (iRet!=ERR_SUCCESS)
				{
					if (m_pLog)
					{
						m_pLog->Record(_T("<LAYER %s> ERROR:DownloadDemoImage-->Get user sectors failed,RetCode(%d)"),strLayer,iRet);
					}
					goto Exit_DownloadDemoImage;
				}
				if ((uiUserSectors==0)||(uiUserSectors==(DWORD)-1))
				{
					if (m_pLog)
					{
						m_pLog->Record(_T("<LAYER %s> ERROR:RKA_File_Download-->User size is wrong,value=0x%x"),strLayer,uiUserSectors);
					}

					goto Exit_DownloadDemoImage;
				}
				if (uiUserSectors<=uiBegin)
				{
					if (m_pLog)
					{
						m_pLog->Record(_T("<LAYER %s> ERROR:RKA_File_Download-->Availabe total is smaller than user offset"),strLayer);
					}

					goto Exit_DownloadDemoImage;
				}
				uiUserSectors -= uiBegin;

				pDbr = pBuf;
				pCopyDbr = pBuf + SECTOR_SIZE*6;
				if (*(UINT *)(pDbr+32)<uiUserSectors)
				{
					if (m_pLog)
					{
						m_pLog->Record(_T("<LAYER %s> ERROR:RKA_File_Download-->Original size is smaller than current user size"),strLayer);
					}

					goto Exit_DownloadDemoImage;
				}

				(*(UINT *)(pDbr+32)) = uiUserSectors;
				(*(UINT *)(pCopyDbr+32)) = uiUserSectors;
			}
		}
		

		iRet = pComm->RKU_WriteLBA(strLinkName,uiBegin,uiTransferSec,pBuf,RWMETHOD_LBA);
		if( ERR_SUCCESS == iRet )
		{
			dwTotalWritten += dwTransferBytes;
			uiBegin += uiTransferSec;
			dwLoopBytes += dwTransferBytes;



			if (dwLoopBytes>=dwLoopConstant)
			{
				HandleProgressCB(dwLayer,DOWNLOADDEMO_PROGRESS,dwFWSize,dwTotalWritten,emCallStep);
				emCallStep = CALL_MIDDLE;
				dwLoopBytes -= dwLoopConstant;
			}
		}
		else
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->RKU_WriteLBA failed,Written(%d),RetCode(%d)"),strLayer,dwTotalWritten,iRet);
			}
			goto Exit_DownloadDemoImage;
		}
	}
	emCallStep = CALL_LAST;
	HandleProgressCB(dwLayer,DOWNLOADDEMO_PROGRESS,dwFWSize,dwTotalWritten,emCallStep);

	if (hdr.bCheckFlag)
	{
		uiBegin=uiPartOffset, dwTotalRead=0, dwLoopBytes=0, emCallStep=CALL_FIRST; 
		file.Seek(sizeof(STRUCT_DEMOIMAGE_HDR),CFile::begin);

		while(dwTotalRead<dwFWSize) 
		{
			memset(pBuf, 0, dwMaxReadWriteBytes);
			memset(pRead, 0, dwMaxReadWriteBytes);
			if ((dwFWSize-dwTotalRead)>=dwMaxReadWriteBytes)
			{
				dwTransferBytes = dwMaxReadWriteBytes;
				uiTransferSec = dwMaxReadWriteSectors;
			}
			else
			{
				dwTransferBytes = dwFWSize - dwTotalRead;
				uiTransferSec = ( (dwTransferBytes%SECTOR_SIZE==0) ? (dwTransferBytes/SECTOR_SIZE) : (dwTransferBytes/SECTOR_SIZE+1) );
			}
			uiRead = file.Read(pBuf,dwTransferBytes);
			if (uiRead!=dwTransferBytes)
			{
				if (m_pLog)
				{
					m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->Read data from file failed in check file,err=%d,read=%d,ret=%d"),strLayer,GetLastError(),dwTransferBytes,uiRead);
				}
				goto Exit_DownloadDemoImage;
			}
			if (dwTotalRead==0)
			{
				if (bUser)
				{
					pDbr = pBuf;
					pCopyDbr = pBuf + SECTOR_SIZE*6;

					(*(UINT *)(pDbr+32)) = uiUserSectors;
					(*(UINT *)(pCopyDbr+32)) = uiUserSectors;
				}
			}
			iRet = pComm->RKU_ReadLBA(strLinkName,uiBegin,uiTransferSec,pRead,RWMETHOD_LBA);

			if( ERR_SUCCESS == iRet )
			{
				dwTotalRead += dwTransferBytes;
				uiBegin += uiTransferSec;
			}
			else
			{
				if (m_pLog)
				{
					m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->RKU_ReadLBA failed,Read(%d),RetCode(%d)"),strLayer,dwTotalRead,iRet);
				}
				goto Exit_DownloadDemoImage;
			}


			if( 0!=memcmp(pBuf, pRead, dwTransferBytes) )
			{
				if (m_pLog)
				{
					m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->memcmp failed,Checked(%dK)"),strLayer,dwTotalRead/1024);
				}

				if (m_pLog)
				{
					GetLocalTime(&now);
					_stprintf(szDateTime,_T("%02d-%02d-%02d"),now.wHour,now.wMinute,now.wSecond);
					strFile = m_pLog->LogSavePath;
					strFile += szDateTime;
					strFile += _T("file.bin");
					m_pLog->SaveBuffer( strFile,pBuf,dwMaxReadWriteBytes );
					strFile = m_pLog->LogSavePath;
					strFile += szDateTime;
					strFile += _T("flash.bin");
					m_pLog->SaveBuffer( strFile,pRead,dwMaxReadWriteBytes );
				}
				goto Exit_DownloadDemoImage;
			}
			dwLoopBytes += dwTransferBytes;
			if (dwLoopBytes>=dwLoopConstant)
			{
				HandleProgressCB(dwLayer,CHECKDEMO_PROGRESS,dwFWSize,dwTotalRead,emCallStep);
				emCallStep = CALL_MIDDLE;	
				dwLoopBytes -= dwLoopConstant;
			}
		}
		emCallStep = CALL_LAST;
		HandleProgressCB(dwLayer,CHECKDEMO_PROGRESS,dwFWSize,dwTotalRead,emCallStep);
	}

	if (hdr.ucImageFS==1)
	{//ext4
		if (!HandleDemoMiscExt4(dwLayer,strLinkName,pComm,(char *)hdr.ucPartition))
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->HandleDemoMiscExt4 failed"),strLayer);
			}
			goto Exit_DownloadDemoImage;
		}
	}
	else if (stricmp((char *)hdr.ucPartition,"user")==0)
	{
		if (!HandleDemoMiscUser(dwLayer,strLinkName,pComm))
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("Layer<%s> ERROR:DownloadDemoImage-->HandleDemoMiscUser failed"),strLayer);
			}
			goto Exit_DownloadDemoImage;
		}
	}
	bSuccess = TRUE;

Exit_DownloadDemoImage:

	if (file.m_hFile!=INVALID_HANDLE_VALUE)
		file.Close();
	if (pBuf)
		delete []pBuf;
	if (pRead)
		delete []pRead;

	return bSuccess;
}
BOOL CFactoryToolDlg::DownloadDCF(DWORD dwLayer,CString strLinkName,CRKComm *pComm)
{
	int i,iRet;
	BOOL bSuccess=FALSE,bRet;
	CFile file;
	DWORD dwTotal,dwCurrent;
	DWORD dwMaxReadWriteSectors=32;
	DWORD dwMaxReadWriteBytes ;
	PBYTE pBuf=NULL ;
	PBYTE pRead=NULL ;
	UINT  uiBegin,uiRead,uiTransferSec;
	ULONGLONG dwTotalWritten,dwTotalRead;
	DWORD dwTransferBytes,dwLoopBytes,dwLoopConstant=1024*1024;
	ULONGLONG dwFWSize;
	TCHAR szDateTime[100];
	CString strFile,strPromptText,strFormatText,strLayer;
	ENUM_CALL_STEP emCallStep;
	SYSTEMTIME	now;
	RW_SUBCODE rwSubCode = (m_pImage->OsType==ANDROID_OS?RWMETHOD_LBA:RWMETHOD_IMAGE);
	file.m_hFile = INVALID_HANDLE_VALUE;
	strLayer = GetLayerString(dwLayer);

	iRet = pComm->RKU_TestDeviceReady(strLinkName,&dwTotal,&dwCurrent);
	if (iRet!=ERR_SUCCESS)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> ERROR:DownloadDCF-->RKU_TestDeviceReady failed,err=%d"),strLayer,iRet);
		}
		goto Exit_DownloadDcf;
	}
	dwMaxReadWriteSectors = dwMaxReadWriteSectors * (1<<dwCurrent);
	dwMaxReadWriteBytes = dwMaxReadWriteSectors * SECTOR_SIZE;
	pBuf = new BYTE[dwMaxReadWriteBytes];
	pRead = new BYTE[dwMaxReadWriteBytes];
	
	dwTotalWritten = 0;
	dwLoopBytes = 0;
	emCallStep = CALL_FIRST;
	if (m_pLog)
	{
		m_pLog->Record(_T("Layer<%s> INFO:Start to download demo,size=%I64u"),strLayer,m_dcf.ullTotalSize);
	}

	for (i=0;i<m_dcf.nCount;i++)
	{
		bRet = file.Open(m_dcf.vecDemo[i].strFilePath,CFile::typeBinary|CFile::modeRead|CFile::shareDenyWrite);
		if( !bRet )
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("Layer<%s> ERROR:DownloadDCF-->open %s failed,err=%d"),strLayer,m_dcf.vecDemo[i].strFilePath,GetLastError());
			}
			goto Exit_DownloadDcf;
		}

		if (m_pLog)
		{
			m_pLog->Record(_T("Layer<%s> INFO:Start to download demo=%s,offset=0x%x,size=%I64u"),strLayer,cmPath::GetFileName(m_dcf.vecDemo[i].strFilePath),m_dcf.vecDemo[i].uiFlashOffset,m_dcf.vecDemo[i].ullFileSize);
		}
		dwFWSize = m_dcf.vecDemo[i].ullFileSize;
		uiBegin =  m_dcf.vecDemo[i].uiFlashOffset;
		
		while(dwFWSize>0) 
		{
			memset(pBuf, 0, dwMaxReadWriteBytes);
			if (dwFWSize>=dwMaxReadWriteBytes)
			{
				dwTransferBytes = dwMaxReadWriteBytes;
				uiTransferSec = dwMaxReadWriteSectors;
			}
			else
			{
				dwTransferBytes = dwFWSize;
				uiTransferSec = ( (dwTransferBytes%SECTOR_SIZE==0) ? (dwTransferBytes/SECTOR_SIZE) : (dwTransferBytes/SECTOR_SIZE+1) );
			}
			uiRead = file.Read(pBuf,dwTransferBytes);
			if (uiRead!=dwTransferBytes)
			{
				if (m_pLog)
				{
					m_pLog->Record(_T("Layer<%s> ERROR:DownloadDCF-->Read data from file failed in write file,err=%d,read=%d,ret=%d"),strLayer,GetLastError(),dwTransferBytes,uiRead);
				}
				goto Exit_DownloadDcf;
			}

			iRet = pComm->RKU_WriteLBA(strLinkName,uiBegin,uiTransferSec,pBuf,rwSubCode);
			if( ERR_SUCCESS == iRet )
			{
				dwTotalWritten += dwTransferBytes;
				uiBegin += uiTransferSec;
				dwLoopBytes += dwTransferBytes;
				dwFWSize -= dwTransferBytes;

				if (dwLoopBytes>=dwLoopConstant)
				{
					HandleProgressCB(dwLayer,DOWNLOADDEMO_PROGRESS,m_dcf.ullTotalSize,dwTotalWritten,emCallStep);
					emCallStep = CALL_MIDDLE;
					dwLoopBytes -= dwLoopConstant;
				}
			}
			else
			{
				if (m_pLog)
				{
					m_pLog->Record(_T("Layer<%s> ERROR:DownloadDCF-->RKU_WriteLBA failed,Written(%d),RetCode(%d)"),strLayer,dwTotalWritten,iRet);
				}
				goto Exit_DownloadDcf;
			}
		}
		file.Close();
	}

	emCallStep = CALL_LAST;
	HandleProgressCB(dwLayer,DOWNLOADDEMO_PROGRESS,m_dcf.ullTotalSize,dwTotalWritten,emCallStep);


	if (m_dcf.bCheck)
	{
		dwTotalRead = 0;
		dwLoopBytes = 0;
		emCallStep = CALL_FIRST;
		for (i=0;i<m_dcf.nCount;i++)
		{
			bRet = file.Open(m_dcf.vecDemo[i].strFilePath,CFile::typeBinary|CFile::modeRead|CFile::shareDenyWrite);
			if( !bRet )
			{
				if (m_pLog)
				{
					m_pLog->Record(_T("Layer<%s> ERROR:DownloadDCF-->open %s failed,err=%d"),strLayer,m_dcf.vecDemo[i].strFilePath,GetLastError());
				}
				goto Exit_DownloadDcf;
			}

			if (m_pLog)
			{
				m_pLog->Record(_T("Layer<%s> INFO:Start to check demo=%s"),strLayer,cmPath::GetFileName(m_dcf.vecDemo[i].strFilePath));
			}
			dwFWSize = m_dcf.vecDemo[i].ullFileSize;
			uiBegin =  m_dcf.vecDemo[i].uiFlashOffset;

			while(dwFWSize>0) 
			{
				memset(pBuf, 0, dwMaxReadWriteBytes);
				if (dwFWSize>=dwMaxReadWriteBytes)
				{
					dwTransferBytes = dwMaxReadWriteBytes;
					uiTransferSec = dwMaxReadWriteSectors;
				}
				else
				{
					dwTransferBytes = dwFWSize;
					uiTransferSec = ( (dwTransferBytes%SECTOR_SIZE==0) ? (dwTransferBytes/SECTOR_SIZE) : (dwTransferBytes/SECTOR_SIZE+1) );
				}
				uiRead = file.Read(pBuf,dwTransferBytes);
				if (uiRead!=dwTransferBytes)
				{
					if (m_pLog)
					{
						m_pLog->Record(_T("Layer<%s> ERROR:DownloadDCF-->Read data from file failed in check file,err=%d,read=%d,ret=%d"),strLayer,GetLastError(),dwTransferBytes,uiRead);
					}
					goto Exit_DownloadDcf;
				}

				iRet = pComm->RKU_ReadLBA(strLinkName,uiBegin,uiTransferSec,pRead,rwSubCode);

				if( ERR_SUCCESS == iRet )
				{
					dwTotalRead += dwTransferBytes;
					uiBegin += uiTransferSec;
					dwFWSize -= dwTransferBytes;
				}
				else
				{
					if (m_pLog)
					{
						m_pLog->Record(_T("Layer<%s> ERROR:DownloadDCF-->RKU_ReadLBA failed,Read(%d),RetCode(%d)"),strLayer,dwTotalRead,iRet);
					}
					goto Exit_DownloadDcf;
				}


				if( 0!=memcmp(pBuf, pRead, dwTransferBytes) )
				{
					if (m_pLog)
					{
						m_pLog->Record(_T("Layer<%s> ERROR:DownloadDCF-->memcmp failed,Checked(%dK)"),strLayer,dwTotalRead/1024);
					}

					if (m_pLog)
					{
						GetLocalTime(&now);
						_stprintf(szDateTime,_T("%02d-%02d-%02d"),now.wHour,now.wMinute,now.wSecond);
						strFile = m_pLog->LogSavePath;
						strFile += szDateTime;
						strFile += _T("file.bin");
						m_pLog->SaveBuffer( strFile,pBuf,dwMaxReadWriteBytes );
						strFile = m_pLog->LogSavePath;
						strFile += szDateTime;
						strFile += _T("flash.bin");
						m_pLog->SaveBuffer( strFile,pRead,dwMaxReadWriteBytes );
					}
					goto Exit_DownloadDcf;
				}
				dwLoopBytes += dwTransferBytes;
				if (dwLoopBytes>=dwLoopConstant)
				{
					HandleProgressCB(dwLayer,CHECKDEMO_PROGRESS,m_dcf.ullTotalSize,dwTotalRead,emCallStep);
					emCallStep = CALL_MIDDLE;	
					dwLoopBytes -= dwLoopConstant;
				}
			}
			file.Close();
		}

		emCallStep = CALL_LAST;
		HandleProgressCB(dwLayer,CHECKDEMO_PROGRESS,m_dcf.ullTotalSize,dwTotalRead,emCallStep);
	}

	bSuccess = TRUE;

Exit_DownloadDcf:

	if (file.m_hFile!=INVALID_HANDLE_VALUE)
		file.Close();
	if (pBuf)
		delete []pBuf;
	if (pRead)
		delete []pRead;

	return bSuccess;
}
BOOL CFactoryToolDlg::RunExe(CString strCmd,tstring &strRet,ENUM_STRING_CODE emRetStringCode)
{
	CMySpawnConsumer spawnConsumer;
	cmSpawn spawn;

	strRet.clear();	
	try
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("Info:RunExe->cmd=%s"),strCmd);
		}
		spawn.Execute(strCmd);
	}
	catch (LPCTSTR pszError)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:RunExe-->Execute failed,Cmd(%s),Error(%s)"),strCmd,pszError);
		}
		return FALSE;
	}
	catch (...)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:RunExe-->Execute failed,Cmd(%s)"),strCmd);
		}
		return FALSE;
	}

	while(TRUE)
	{
		if (!spawn.TestThread())
		{
			break;
		}
		Sleep(200);
	}

	spawnConsumer.SetSpawnObject(&spawn);
	spawnConsumer.GetSpawnString(strRet,emRetStringCode);

	return TRUE;

}


BOOL CFactoryToolDlg::GetAdbSerialno(CString strDevPath,CString &strSerialno)
{
	BOOL bSuccess=FALSE;
	CString strAdbWinApiDll=m_strModulePath + _T("bin\\AdbWinApi.dll");
	TCHAR tzSerial[MAX_PATH];
	unsigned long ulSerialSize = MAX_PATH;
	HMODULE hAdbwinapi=NULL;
	ADBAPIHANDLE hAdbHandle=NULL;
	AdbCloseHandle pAdbCloseHandle=NULL;
	AdbCreateInterfaceByName pAdbCreateInterfaceByName=NULL;
	AdbGetSerialNumber pAdbGetSerialNumber=NULL;
	strSerialno = _T("");

	SetCurrentDirectory(m_strModulePath + _T("bin\\"));
	hAdbwinapi = LoadLibrary(strAdbWinApiDll);
	if (!hAdbwinapi)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:GetAdbSerialno->LoadLibrary AdbWinApi.dll failed!"));
		}
		goto Exit_GetAdbSerialno;
	}
	pAdbCloseHandle = (AdbCloseHandle)GetProcAddress( hAdbwinapi, "AdbCloseHandle");
	if (!pAdbCloseHandle)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:GetAdbSerialno->Get AdbCloseHandle address failed!"));
		}
		goto Exit_GetAdbSerialno;
	}

	pAdbCreateInterfaceByName = (AdbCreateInterfaceByName)GetProcAddress( hAdbwinapi, "AdbCreateInterfaceByName");
	if (!pAdbCreateInterfaceByName)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:GetAdbSerialno->Get AdbCreateInterfaceByName address failed!"));
		}
		goto Exit_GetAdbSerialno;
	}

	pAdbGetSerialNumber = (AdbGetSerialNumber)GetProcAddress( hAdbwinapi, "AdbGetSerialNumber");
	if (!pAdbGetSerialNumber)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:GetAdbSerialno->Get AdbGetSerialNumber address failed!"));
		}
		goto Exit_GetAdbSerialno;
	}

	hAdbHandle = pAdbCreateInterfaceByName((LPTSTR)(LPCTSTR)strDevPath);
	if (!hAdbHandle)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:GetAdbSerialno->AdbCreateInterfaceByName failed,err=%d,dev=%s!"),GetLastError(),strDevPath);
		}
		goto Exit_GetAdbSerialno;
	}
	memset(tzSerial,0,sizeof(TCHAR)*MAX_PATH);
	if (!pAdbGetSerialNumber(hAdbHandle,(void *)tzSerial,&ulSerialSize,false))
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:GetAdbSerialno->AdbGetSerialNumber failed!"));
		}
		goto Exit_GetAdbSerialno;
	}
	strSerialno = tzSerial;
	bSuccess = TRUE;
Exit_GetAdbSerialno:
	if (hAdbHandle)
	{
		pAdbCloseHandle(hAdbHandle);
		hAdbHandle = NULL;
	}
	if (hAdbwinapi)
	{
		FreeLibrary(hAdbwinapi);
		hAdbwinapi = NULL;
	}
	SetCurrentDirectory(m_strModulePath);
	return bSuccess;
}
BOOL CFactoryToolDlg::AdjustPrivileges()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	TOKEN_PRIVILEGES oldtp;
	DWORD dwSize=sizeof(TOKEN_PRIVILEGES);
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		if (GetLastError()==ERROR_CALL_NOT_IMPLEMENTED) 
			return TRUE;
		else 
			return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		CloseHandle(hToken);
		return FALSE;
	}
	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount=1;
	tp.Privileges[0].Luid=luid;
	tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) 
	{
		CloseHandle(hToken);
		return FALSE;
	}
	// close handles
	CloseHandle(hToken);
	return TRUE;
}
BOOL CFactoryToolDlg::KillAdbProcess()
{
	CString strCmd,strProcess;
	tstring strRet;
	DWORD dwProcessID;
	BOOL bRet;
	int iRet;
	strCmd = CMySpawnConsumer::FindAdbProcess();

	bRet = RunExe(strCmd,strRet,SC_ANSI);
	if (!bRet)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:KillAdbProcess->FindAdbProcess failed!"));
		}
		return FALSE;
	}
	tstringstream stream(strRet);
	tstring strLine;
	tstring::size_type line_size;
	while(!stream.eof())
	{
		getline(stream,strLine);
		line_size = strLine.size();
		if (line_size<=0)
		{
			continue;
		}
		if (strLine[line_size-1]==_T('\r'))
		{
			strLine = strLine.substr(0,line_size-1);
		}
		strProcess.Format(_T("%s"),strLine.c_str());
		iRet = strProcess.Find(_T("5037"));
		if (iRet!=-1)
		{
			iRet = strProcess.Find(_T("LISTENING"));
			if (iRet!=-1)
			{
				strProcess = strProcess.Mid(iRet+10);
				dwProcessID = cmNumString::StrToInt32(strProcess);
				if (dwProcessID>0)
				{
					HANDLE hProcess=NULL;
					hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessID);
					if (hProcess)
					{
						bRet = TerminateProcess(hProcess,0);
						bRet = CloseHandle(hProcess);
					}
					else
					{
						AdjustPrivileges();
						hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessID);
						if (hProcess)
						{
							bRet = TerminateProcess(hProcess,0);
							bRet = CloseHandle(hProcess);
						}
					}
				}
			}
		}
	}
	return TRUE;
}
BOOL CFactoryToolDlg::AdbToRockusb(CString strDevPath)
{
	CString strAdb,strAdbDll,strAdbUsbDll;
	CString strCmd,strSerial;
	tstring strRet;
	BOOL bRet;
	strAdb = m_strModulePath + _T("bin\\adb.exe");
	strAdbDll = m_strModulePath + _T("bin\\AdbWinApi.dll");
	strAdbUsbDll = m_strModulePath + _T("bin\\AdbWinUsbApi.dll");
	if (!cmFile::IsExisted(strAdb))
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:AdbToRockusb->adb.exe is not existed!"));
		}
		return FALSE;
	}
	if (!cmFile::IsExisted(strAdbDll))
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:AdbToRockusb->AdbWinApi.dll is not existed!"));
		}
		return FALSE;
	}
	if (!cmFile::IsExisted(strAdbUsbDll))
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:AdbToRockusb->AdbWinUsbApi.dll is not existed!"));
		}
		return FALSE;
	}
	KillAdbProcess();
	bRet = GetAdbSerialno(strDevPath,strSerial);
	if (!bRet)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:AdbToRockusb->GetAdbSerialno failed!"));
		}
		return FALSE;
	}

	strCmd = CMySpawnConsumer::RebootLoaderCmd(strAdb,strSerial);

	bRet = RunExe(strCmd,strRet,SC_ANSI);
	if (!bRet)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:AdbToRockusb->RebootLoader failed!"));
		}
		return FALSE;
	}
	else
	{
		if (strRet.find(_T("error:"))!=tstring::npos)
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("%s"),strRet.c_str());
			}
			Sleep(m_nSwitchRetryInterval*1000);
			strSerial = _T("");
			strCmd = CMySpawnConsumer::RebootLoaderCmd(strAdb,strSerial);
			RunExe(strCmd,strRet,SC_ANSI);
		}
	}
	return TRUE;

}
BOOL CFactoryToolDlg::LoadSnRes()
{
	CString strSnDatFile;
	CFile file;
	BOOL bRet,bSuccess=FALSE;
	DWORD dwOldCrc,dwNewCrc,dwFileSize,dwRead;
	UINT i;
	PBYTE snBitmap;
	m_pSnFileData = NULL;
	m_pSnDatHead ;
	strSnDatFile = m_strModulePath + _T("sn.dat");
	file.m_hFile = INVALID_HANDLE_VALUE;
	bRet = file.Open(strSnDatFile,CFile::typeBinary|CFile::modeRead,NULL);
	if (!bRet)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:LoadSnRes->open %s failed,err=%d!"),strSnDatFile,GetLastError());
		}
		goto Exit_LoadSnRes;
	}
	dwFileSize = file.GetLength();
	m_pSnFileData = new BYTE[dwFileSize];
	if (!m_pSnFileData)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:LoadSnRes->new memory failed,err=%d!"),GetLastError());
		}
		goto Exit_LoadSnRes;
	}
	m_pSnDatHead = (PSTRUCT_SN_DAT_HEAD)m_pSnFileData;
	dwRead = file.Read(m_pSnFileData,dwFileSize);
	if (dwRead!=dwFileSize)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:LoadSnRes->read %s failed,err=%d!"),strSnDatFile,GetLastError());
		}
		goto Exit_LoadSnRes;
	}
	if (m_pSnDatHead->uiTag!=SN_DAT_HEADER_TAG)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:LoadSnRes->checking tag is wrong in the sn.dat!"));
		}
		goto Exit_LoadSnRes;
	}
	dwOldCrc = *(PDWORD)(m_pSnFileData+m_pSnDatHead->uiHeaderSize-4);
	dwNewCrc = cmAlgorithm::DoCrc32(m_pSnFileData,m_pSnDatHead->uiHeaderSize-4);
	if (dwOldCrc!=dwNewCrc)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:LoadSnRes->checking code is wrong in the sn.dat,old=0x%x,new=0x%x!"),dwOldCrc,dwNewCrc);
		}
		goto Exit_LoadSnRes;
	}
	
	if (m_pSnDatHead->uiHeaderSize+SN_MAX_LENGTH*m_pSnDatHead->uiTotal!=dwFileSize)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:LoadSnRes->checking sn count failed!"));
		}
		goto Exit_LoadSnRes;
	}
	dwOldCrc = m_pSnDatHead->uiSnDataCrc;
	dwNewCrc = cmAlgorithm::DoCrc32(m_pSnFileData+m_pSnDatHead->uiHeaderSize,dwFileSize-m_pSnDatHead->uiHeaderSize);
	if (dwOldCrc!=dwNewCrc)
	{
		if (m_pLog)
		{
			m_pLog->Record(_T("ERROR:LoadSnRes->checking data of sn is wrong in the sn.dat,old=0x%x,new=0x%x!"),dwOldCrc,dwNewCrc);
		}
		goto Exit_LoadSnRes;
	}
	m_nSnSize = m_pSnDatHead->uiSnLength;
	snBitmap = m_pSnDatHead->snBitmap;
	m_uiFindStartPos = 0;
	for (i=0;i<m_pSnDatHead->uiTotal;i++,snBitmap++)
	{
		if (*snBitmap)
		{
			m_uiFindStartPos++;
			continue;
		}
		else
			break;
	}
	bSuccess = TRUE;
Exit_LoadSnRes:
	if (file.m_hFile!=INVALID_HANDLE_VALUE)
	{
		file.Close();
	}
	return bSuccess;
}
VOID CFactoryToolDlg::FreeSnRes()
{
	if (m_pSnFileData)
	{
		delete []m_pSnFileData;
		m_pSnFileData = NULL;
	}
}
PBYTE CFactoryToolDlg::NextSn_Start(UINT &nIndex)
{
	PBYTE snBitmap;
	UINT i;
	m_csSnOperation.Lock();
	snBitmap = m_pSnDatHead->snBitmap;
	snBitmap += m_uiFindStartPos;
	for (i=m_uiFindStartPos;i<m_pSnDatHead->uiTotal;i++,snBitmap++)
	{
		if (*snBitmap)
		{
			continue;
		}
		else
		{
			m_uiFindStartPos = i+1;
			nIndex = i;
			m_pSnDatHead->uiFree--;
			*snBitmap = 1;
			m_csSnOperation.Unlock();
			return m_pSnFileData+m_pSnDatHead->uiHeaderSize+(SN_MAX_LENGTH*nIndex);
		}
	}
	m_csSnOperation.Unlock();
	return NULL;
}
VOID  CFactoryToolDlg::NextSn_End(UINT nIndex,BOOL bSuccess)
{
	CString strSnDatFile;
	CFile file;
	BOOL bRet;
	DWORD dwNewCrc,dwRead;
	PSTRUCT_SN_DAT_HEAD pSnDatHead=NULL;
	PBYTE snBitmap=NULL;
	file.m_hFile = INVALID_HANDLE_VALUE;
	if (bSuccess)
	{
		m_csSnDatFile.Lock();
		strSnDatFile = m_strModulePath + _T("sn.dat");
		bRet = file.Open(strSnDatFile,CFile::typeBinary|CFile::modeReadWrite,NULL);
		if (!bRet)
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("ERROR:NextSn_End->open %s failed,err=%d!"),strSnDatFile,GetLastError());
			}
			goto Exit_NextSnEnd;
		}
		pSnDatHead = (PSTRUCT_SN_DAT_HEAD)new BYTE[m_pSnDatHead->uiHeaderSize];
		if (!pSnDatHead)
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("ERROR:NextSn_End->new memory failed,err=%d!"),GetLastError());
			}
			goto Exit_NextSnEnd;
		}
		dwRead = file.Read((PBYTE)pSnDatHead,m_pSnDatHead->uiHeaderSize);
		if (dwRead!=m_pSnDatHead->uiHeaderSize)
		{
			if (m_pLog)
			{
				m_pLog->Record(_T("ERROR:NextSn_End->read %s failed,err=%d!"),strSnDatFile,GetLastError());
			}
			goto Exit_NextSnEnd;
		}
		pSnDatHead->uiFree--;
		snBitmap = pSnDatHead->snBitmap;
		*(snBitmap+nIndex) = 1;
		dwNewCrc = cmAlgorithm::DoCrc32((PBYTE)pSnDatHead,pSnDatHead->uiHeaderSize-4);
		*(PDWORD(((PBYTE)pSnDatHead)+pSnDatHead->uiHeaderSize-4)) = dwNewCrc;
		file.SeekToBegin();
		file.Write((PBYTE)pSnDatHead,pSnDatHead->uiHeaderSize);
	}
	else
	{
		m_csSnOperation.Lock();
		snBitmap = m_pSnDatHead->snBitmap;
		*(snBitmap+nIndex) = 0;
		m_pSnDatHead->uiFree++;
		if (m_uiFindStartPos>nIndex)
		{
			m_uiFindStartPos = nIndex;
		}
	}
Exit_NextSnEnd:
	if (bSuccess)
	{
		m_csSnDatFile.Unlock();
	}
	else
		m_csSnOperation.Unlock();
	if (pSnDatHead)
	{
		delete []((PBYTE)pSnDatHead);
		pSnDatHead = NULL;
	}
	if (file.m_hFile!=INVALID_HANDLE_VALUE)
	{
		file.Close();
	}
}