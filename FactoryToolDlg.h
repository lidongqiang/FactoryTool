// FactoryToolDlg.h : header file
//

#if !defined(AFX_FACTORYTOOLDLG_H__70E20E7B_63E8_43F5_B2B0_46F613CEFD29__INCLUDED_)
#define AFX_FACTORYTOOLDLG_H__70E20E7B_63E8_43F5_B2B0_46F613CEFD29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cmLog.h"
#include "cmIniFile.h"
#include "cmMultiLanguage.h"
#include "cmPath.h"
#include "cmFile.h"
#include "cmNumString.h"
#include "cmCommonDlg.h"
#include "cmStrCode.h"
#include "cmSpawn.h"
#include "cmAlgorithm.h"
using namespace cm;
#include "DefineHeader.h"
#include "RKScan.h"
#include "RKComm.h"
#include "RKImage.h"
#include "RKUpgrade.h"
#include "RKDevice.h"
#include "RK28Device.h"
#include "RKNanoDevice.h"
#include "RKAndroidDevice.h"

#include "afxwin.h"
#include "BtnST.h"
#include "XGroupBox.h"
#include "FontStatic.h"
#include "ColumnTreeCtrl.h"
#include "VirtualGridCtrl.h"
#include "MySpawnConsumer.h"
//定义sn文件结构
#define SN_DAT_HEADER_TAG 0x46444E53
#define SN_MAX_LENGTH 60
#pragma pack(1)
typedef struct {
	UINT uiTag;//'SNDF'
	UINT uiHeaderSize;//size of header
	UINT uiTotal;//total of sn
	UINT uiFree;//count of available sn
	UINT uiSnLength;//must be smaller than 60 
	UINT uiSnDataCrc;//crc of sn
	BYTE snBitmap[1];
}STRUCT_SN_DAT_HEAD,*PSTRUCT_SN_DAT_HEAD;
#pragma pack()
//定义Adbwinapi.dll接口
typedef void* ADBAPIHANDLE;
typedef struct _AdbInterfaceInfo 
{
	GUID          class_id;
	unsigned long flags;
	wchar_t       device_name[1];
} AdbInterfaceInfo;
typedef ADBAPIHANDLE (*AdbEnumInterfaces)(GUID class_id,bool exclude_not_present,bool exclude_removed, bool active_only);
typedef bool (*AdbNextInterface)(ADBAPIHANDLE adb_handle,AdbInterfaceInfo* info, unsigned long* size);
typedef bool (*AdbCloseHandle)(ADBAPIHANDLE adb_handle);
typedef bool (*AdbGetSerialNumber)(ADBAPIHANDLE adb_interface,void* buffer,unsigned long* buffer_char_size,bool ansi);
typedef ADBAPIHANDLE (*AdbCreateInterfaceByName)(const wchar_t* interface_name);


#define ANDROID_USB_CLASS_ID \
{0xf72fe0d4, 0xcbcb, 0x407d, {0x88, 0x14, 0x9e, 0xd6, 0x73, 0xd0, 0xdd, 0x6b}};

#define IDM_LANG_MENU 500
#define IDM_LANG_MENUITEM 5000
#define MAX_MENU_ITEM	127
typedef enum{USB_TYPE_NONE=0,USB_TYPE_HOST,USB_TYPE_HOST20,USB_TYPE_HUB,USB_TYPE_HUB20,USB_TYPE_PORT,USB_TYPE_PORT20};
#define  IMAGE_INDEX_COMPUTER 0
#define  IMAGE_INDEX_HOST 1
#define  IMAGE_INDEX_HUB 2
#define  IMAGE_INDEX_PORT 3
#define  IMAGE_INDEX_PORT_OFF 4
#define  MAX_DEVICE	16
#define WM_LOADFW_MSG		(WM_USER+111)
#define WM_UPDATE_GRID_MSG  (WM_USER+112)
typedef map<CString,DWORD> LAYER_MAP;
typedef map<CString,HTREEITEM> TREEITEM_MAP;
typedef list<STRUCT_RKDEVICE_DESC> DEVICE_LIST;
typedef list<STRUCT_HUB_INFO> HUB_LIST;
typedef DEVICE_LIST::iterator device_list_iter;
typedef HUB_LIST::iterator hub_list_iter;
typedef struct  
{
	BOOL bRun;
	BOOL bUse;
	time_t tmStart;
	UINT uiSnIndex;
	STRUCT_RKDEVICE_DESC dev;
}STRUCT_RUN_CONTEXT,*PSTRUCT_RUN_CONTEXT;
typedef struct
{
	CString strLayer;
	DWORD dwElapseSecond;
}STRUCT_RESULT_ITEM,*PSTRUCT_RESULT_ITEM;
typedef struct
{
	CString strLayer;
	CString strErrorText;
}STRUCT_FAIL_ITEM,*PSTRUCT_FAIL_ITEM;
#pragma pack(1)
typedef struct
{
	UINT uiTag;//'DEMO'
	UCHAR ucImageFS;//0:fat32;1:ext4
	UCHAR ucPartition[32];
	BOOL bCheckFlag;
	ULONGLONG ullImageSize;
	UCHAR ucReserved[207];
}STRUCT_DEMOIMAGE_HDR,*PSTRUCT_DEMOIMAGE_HDR;
typedef struct
{
	char command[32];
	char status[32];
	char recovery[768];
	char stage[32];
	char reserved[224];
}STRUCT_BOOTLOADER_MSG,*PSTRUCT_BOOTLOADER_MSG;
typedef struct
{
	UINT uiFlashOffset;
	ULONGLONG ullFileSize;
	CString strFilePath;
}STRUCT_DEMO_ITEM,*PSTRUCT_DEMO_ITEM;
typedef vector<STRUCT_DEMO_ITEM> DEMO_VECTOR;
typedef DEMO_VECTOR::iterator demo_vector_iter;
typedef struct  
{
	BOOL bCheck;
	int nCount;
	ULONGLONG ullTotalSize;
	DEMO_VECTOR vecDemo;
}STRUCT_DCF,*PSTRUCT_DCF;

#pragma pack() 
/////////////////////////////////////////////////////////////////////////////
// CFactoryToolDlg dialog

class CFactoryToolDlg : public CDialog
{
// Construction
public:
	VOID ScanThreadProc();
	VOID LoadFwProc();
	VOID UpgradeThreadProc(int index);
	VOID HandlePromptCB(DWORD deviceLayer,ENUM_UPGRADE_PROMPT promptID,DWORD oldDeviceLayer);
	VOID HandleProgressCB(DWORD deviceLayer,ENUM_PROGRESS_PROMPT promptID,ULONG64 totalValue,ULONG64 currentValue,ENUM_CALL_STEP emCall);
	CFactoryToolDlg(CWnd* pParent = NULL);	// standard constructor
// Dialog Data
	//{{AFX_DATA(CFactoryToolDlg)
	enum { IDD = IDD_FACTORYTOOL_DIALOG };
	CColumnTreeCtrl	m_treeDeviceInfo;
	CFontStatic	m_staticHelpText;
	CFontStatic		m_staticLoaderVer;

	CFontStatic 	m_staticFwVersion;
	CFontStatic 	m_staticFwChip;
	CFontStatic 	m_staticFirmware;


	CFontStatic m_staticSuccess;
	CFontStatic m_staticSuccessCount;
	CFontStatic m_staticFail;
	CFontStatic m_staticFailCount;
	CFontStatic m_staticTotal;
	CFontStatic m_staticTotalCount;

	CEdit	m_edtDemo;
	CXGroupBox	m_groupboxhelp;
	CXGroupBox	m_groupbox9;
	CXGroupBox	m_groupbox8;
	CXGroupBox	m_groupbox7;
	CXGroupBox	m_groupbox6;
	CXGroupBox	m_groupbox5;
	CXGroupBox	m_groupbox4;
	CXGroupBox	m_groupbox3;
	CXGroupBox	m_groupbox2;
	CXGroupBox	m_groupbox12;
	CXGroupBox	m_groupbox11;
	CXGroupBox	m_groupbox10;
	CXGroupBox	m_groupbox1;
	CEdit		m_edtFirmware;
	CButtonST	m_btnRun;
	CButtonST	m_btnLanguage;
	CButtonST	m_btnExit;
	CButtonST	m_btnDemo;
	CButtonST	m_btnFirmware;
	int		m_nUpgradeRadio;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFactoryToolDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	CVirtualGridCtrl m_resultGrid;
	CVirtualGridCtrl m_failGrid;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFactoryToolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnButtonFirmware();
	afx_msg void OnButtonRun();
	afx_msg void OnButtonDemo();
	afx_msg void OnButtonLanguage();
	afx_msg void OnButtonExit();
	afx_msg LRESULT OnDeviceChange(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	afx_msg LRESULT OnHandleLoadFwMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnHandleUpdateGridMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnLangSelect(UINT uid);
	afx_msg void OnGridGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGridSetDispInfo(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
private:
	vector<STRUCT_RESULT_ITEM> m_resultItems;
	vector<STRUCT_FAIL_ITEM> m_failItems;
	vector<STRUCT_RKDEVICE_DESC> m_backupDev;
	STRUCT_RUN_CONTEXT m_runContext[MAX_DEVICE];
	CCriticalSection m_csResultItems;
	CCriticalSection m_csFailItems;
	CCriticalSection m_csUpdateCount;
	DWORD m_layerID;
	LAYER_MAP m_layerMap;
	TREEITEM_MAP m_treeItemMap;
	list<STRUCT_RKDEVICE_DESC> m_deviceList;
	list<STRUCT_HUB_INFO> m_hubList;
	STRUCT_DCF m_dcf;
	BOOL m_bMutexResetFlag;
	BOOL m_bRunFlag;
	BOOL m_bScanDevice;
	int m_nScanDevice;
	CCriticalSection m_csScanDevice;
	BOOL m_bCheckFw;
	int m_nRestartTimeout;
	int m_nSwitchRetryInterval;
	BOOL m_bSupportMsc;
	BOOL m_bLoopTest;
	CWinThread *m_pScanThread;
	CEvent  *m_pScanEvent;
	BOOL m_bContinueScan;
	CRKScan *m_pScan;
	CRKLog *m_pLog;
	CRKImage *m_pImage;
	CString m_strModulePath;
	CString m_strLogPath;
	CString m_LangPath;
	cmIniFile m_iniConfig;
	cmMultiLanguage *m_pLangObject;
	LANG_PROP_VECTOR m_langProp;
	CHAR m_curLang;
	BOOL m_bSupportFullUsb;
	BOOL m_bResetAfterDemo;
	BOOL m_bResetAfterUpgrade;
	BOOL m_bFwVerHex;
	BOOL m_bUidCreate;
	int  m_mscVid;
	int  m_mscPid;
	int  m_mscTimeout;
	int  m_rockusbTimeout;
	UINT m_uiForceDataBand;
	ENUM_MISC_MODIFY_FLAG m_emMiscModifyFlag;
	CString m_strDiskVolume;
	CString m_strDefaultDemo;
	CString m_strDefaultFirmware;
	CString m_strUidHead;
	CFont m_font;
	BOOL	m_bRedLedLight;
	HBITMAP m_hGreenLedBitmap;
	HBITMAP m_hRedLedBitmap;
	CImageList m_imgList;
	CString m_strFwVerText;
	CString m_strLoaderVerText;
	CString m_strFwChipText;
	CString m_strDeviceListText;
	CString m_strDeviceTypeText;
	CString m_strIDText;
	CString m_strProcessText;
	CString m_strPromptText;
	CString m_strComputerText;
	CString m_strRunText;
	CString m_strStopText;
	CString m_strFailText;
	CString m_strSuccessText;
	CString m_strResultText;
	CString m_strTimeText;
// 	CString m_strHighControllerText;
// 	CString m_strFullControllerText;
// 	CString m_strRootHubText;
// 	CString m_strPortText;
// 	CString m_strEhciText;
// 	CString m_strUhicText;
// 	CString m_strHubText;
	CString m_strHelpText;
	CMenu *m_popMenu;
	CEvent  *m_pWaitEvent;
	CString m_strFwPath;
	CString m_strDemoPath;
	void InitToolButton();
	void InitGroupbox();
	void InitStatic();
	void InitTree();
	void InitGrid();
	void RefreshUsbInteface();
	void UpdateUsbInterface(PSTRUCT_HUB_INFO pHub, PSTRUCT_RKDEVICE_DESC pDevice, BOOL bDevice, BOOL bRemove);
	void RemoveHubInterface(CString strLayer);
	void AddHubInterface(CString strLayer,int nPort);
	void InitLangString();
	BOOL LoadConfig();
	CString GetLocalString(CString strKey);
	void RefreshTreeText();
	void RefreshGridText();
	void EnableCtrl(BOOL bFwFlag,BOOL bFwOn,BOOL bRunFlag,BOOL bRunOn,BOOL bUpgradeFlag,BOOL bUpgradeOn,BOOL bOtherFlag,BOOL bOtherOn);
	BOOL UpdateFwInfo(CString strFWPath);
	VOID ShowFwInfo();
	VOID InitRunContext();
	VOID HandleNewDevice(STRUCT_RKDEVICE_DESC &device);
	VOID HandleRemoveDevice(STRUCT_RKDEVICE_DESC &device);
	CHAR GetIndexFromLayer(DWORD dwLayerID);
	BOOL CreateUid(PBYTE pUid);
	int RandomInteger(int low, int high);
	void AddBackupToRunContext();
	void ReRunContext();
	void SetRKDeviceData(STRUCT_RKDEVICE_DESC &dst,PSTRUCT_RKDEVICE_DESC pSrc=NULL);
	void UpdateResultItem(CString strLayer,DWORD dwElapseSecond,BOOL bRemove=FALSE);
	void UpdateFailItem(CString strLayer,CString strError,BOOL bRemove=FALSE);
	BOOL LoadLayerMap(LAYER_MAP &layerMap);
	DWORD GetIDFromLayerMap(CString strLayer);
	BOOL IsDeviceAlive(CString strLayer,BOOL &bAlive);
	BOOL CheckPartitionSize(CRKImage *pImage);
	BOOL GetPartitionInfo(CRKImage *pImage,char *pszName,UINT &uiPartOffset,UINT &uiPartSize);
	BOOL ParseParameterForPartInfo(PCHAR pParam,PCHAR pPartName,UINT &uiOffset,UINT &uiSize);
	BOOL ParsePartInfo(string &strPartInfo,string &strName,UINT &uiOffset,UINT &uiLen);
	BOOL IsDemoImage();
	BOOL CheckDemoImageDownload(UINT &uiDemoPartOffset,UINT &uiDemoPartSize);
	BOOL DownloadDemoImage(DWORD dwLayer,CString strLinkName,CRKComm *pComm,UINT uiPartOffset);
	BOOL DownloadDCF(DWORD dwLayer,CString strLinkName,CRKComm *pComm);
	BOOL HandleDemoMiscUser(DWORD dwLayer,CString strLinkName,CRKComm *pComm);
	BOOL HandleDemoMiscExt4(DWORD dwLayer,CString strLinkName,CRKComm *pComm,char *szPartition);
	BOOL RunExe(CString strCmd,tstring &strRet,ENUM_STRING_CODE emRetStringCode);
	BOOL GetAdbSerialno(CString strDevPath,CString &strSerialno);
	BOOL AdjustPrivileges();
	BOOL KillAdbProcess();
	BOOL AdbToRockusb(CString strDevPath);
	BOOL LoadDemoConfigFile(CString strDcfFile);
public:
	CButton m_checkDemo;
private:
	BOOL m_bSnDllOn;
	PBYTE m_pSnFileData;
	PSTRUCT_SN_DAT_HEAD m_pSnDatHead;
	BYTE m_nSnSize;
	UINT m_uiFindStartPos;
	CCriticalSection m_csSnOperation;
	CCriticalSection m_csSnDatFile;
	BOOL LoadSnRes();
	VOID FreeSnRes();
	PBYTE NextSn_Start(UINT &nIndex);
	VOID  NextSn_End(UINT nIndex,BOOL bSuccess);
};
BOOL GetExeVersion(CString &strVer);
BOOL WatiObjectWithMsg(LPHANDLE pObjHandle);
DWORD GetLayerID(CString strLayer);
CString GetLayerString(DWORD layerID);
CString StringToWideString(char *pszSrc);
VOID PromptInfoProc(DWORD deviceLayer,ENUM_UPGRADE_PROMPT promptID,DWORD oldDeviceLayer);
VOID ProgressInfoProc(DWORD deviceLayer,ENUM_PROGRESS_PROMPT promptID,ULONG64 totalValue,ULONG64 currentValue,ENUM_CALL_STEP emCall);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FACTORYTOOLDLG_H__70E20E7B_63E8_43F5_B2B0_46F613CEFD29__INCLUDED_)
