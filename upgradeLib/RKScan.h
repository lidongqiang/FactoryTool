#ifndef RKSCAN_HEADER
#define RKSCAN_HEADER
#include "DefineHeader.h"
#include "RKLog.h"
typedef enum 
{
		DeviceDescription,
		HardwareId,
		CompatibleIds,
		Service,
		Class,
		ClassGuid,
		Driver,
		Manufacturer,
		FriendlyName,
		LocationInfo,
		PDOName,
}ENUM_DEVPROP;
typedef struct  
{
	CString strHostPath;
	CString strRootHubPath;
	CString strHostLayer;
	BOOL	bUsb20;
	int		nPort;
}STRUCT_HOST_INFO,*PSTRUCT_HOST_INFO;
typedef struct  
{
	CString strHubPath;
	CString strHubLayer;
	BOOL	bUsb20;
	int		nPort;
}STRUCT_HUB_INFO,*PSTRUCT_HUB_INFO;
typedef struct  
{
	USHORT usVid;
	USHORT usPid;
	ENUM_RKDEVICE_TYPE emDeviceType;
}STRUCT_DEVICE_CONFIG,*PSTRUCT_DEVICE_CONFIG;

typedef struct  
{
	USHORT usVid;
	USHORT usPid;
	BOOL   bDeviceConnected;
	BOOL   bDriverInstalled;
}STRUCT_PORT_INFO,*PSTRUCT_PORT_INFO;
typedef int PORT_POSITION;

typedef struct  
{
	TCHAR tzDrive;
	UINT  uiDiskSizeMB;
}STRUCT_DRIVE_PROP,*PSTRUCT_DRIVE_PROP;

typedef vector<STRUCT_DEVICE_CONFIG> RKDEVICE_CONFIG_SET;
class CRKScan
{
public:
// 	PROP(USHORT,MSC_VID);
// 	PROP(USHORT,MSC_PID);
	PROP(UINT,MSC_TIMEOUT);
	PROP(UINT,RKUSB_TIMEOUT);
	PROP(BOOL,SUPPORT_LOWUSB);
	GETPROP(int,DEVICE_COUNTS);

// 	GET(USHORT,MSC_VID);
// 	GET(USHORT,MSC_PID);
// 	SET(USHORT,MSC_VID);
// 	SET(USHORT,MSC_PID);
	GET(UINT,MSC_TIMEOUT);
	GET(UINT,RKUSB_TIMEOUT);
	GET(BOOL,SUPPORT_LOWUSB);
	GET(int,DEVICE_COUNTS);
	SET(UINT,MSC_TIMEOUT);
	SET(UINT,RKUSB_TIMEOUT);
	SET(BOOL,SUPPORT_LOWUSB);

	CRKScan(UINT uiMscTimeout,UINT uiRKusbTimeout,BOOL bSupportLowUsb=FALSE);
	void SetVidPid(USHORT mscVid=0,USHORT mscPid=0,USHORT adbVid=0,USHORT adbPid=0);
	BOOL FindRockusbVidPid(ENUM_RKDEVICE_TYPE type,USHORT &usVid,USHORT &usPid);
	int Search(UINT type);
//	BOOL MultiWait(STRUCT_RKDEVICE_DESC &device,ENUM_RKUSB_TYPE usbType,CONST RKDEVICE_CONFIG_SET &waitDeviceSet);
	BOOL Wait(STRUCT_RKDEVICE_DESC &device,ENUM_RKUSB_TYPE usbType,USHORT usVid=0,USHORT usPid=0);
	BOOL MutexWaitPrepare(STRING_VECTOR &vecExistedDevice,CString &strOfflineDevice);
	BOOL MutexWait(STRING_VECTOR &vecExistedDevice,STRUCT_RKDEVICE_DESC &device,ENUM_RKUSB_TYPE usbType,USHORT usVid=0,USHORT usPid=0);
	int GetPos(CString strLayer);
	BOOL GetDevice(STRUCT_RKDEVICE_DESC &device,int pos);
	VOID GetHostInfo(vector<STRUCT_HOST_INFO> &vecHost);
	VOID GetExternalHubInfo(vector<STRUCT_HUB_INFO> &vecHub);
	static	BOOL GetDrive(STRUCT_RKDEVICE_DESC &device,CRKLog *pLog=NULL);
	static	int GetAllDrive(STRUCT_RKDEVICE_DESC &device,PSTRUCT_DRIVE_PROP pDriveArray,int iDriveArraySize,CRKLog *pLog=NULL);
	BOOL SetLogObject( CRKLog *pLog );
	BOOL GetPortStatus(CString strLayer,STRUCT_PORT_INFO &portInfo);
	PORT_POSITION GetFirstPostStatus();
	PORT_POSITION GetNextPortStatus(PORT_POSITION pos,CString &strLayer,STRUCT_PORT_INFO &portInfo);
	~CRKScan();	
protected:
private:
// 	USHORT m_mscVid;
// 	USHORT m_mscPid;
	map<CString,STRUCT_PORT_INFO> m_mapPortStatus;
	vector<STRUCT_HOST_INFO> m_vecHost;
	vector<STRUCT_HUB_INFO> m_vecHub;
	vector<STRUCT_HUB_INFO> m_vecHubNew;
	OSVERSIONINFO m_osvi;
	UINT   m_waitRKusbSecond;
	UINT   m_waitMscSecond;
	BOOL   m_supportLowUsb;
	CRKLog *m_log;
	RKDEVICE_DESC_SET m_list;
	RKDEVICE_CONFIG_SET m_deviceConfigSet;
	RKDEVICE_CONFIG_SET m_deviceMscConfigSet;
	RKDEVICE_CONFIG_SET m_deviceAdbConfigSet;
	static UINT GetDiskSpace(TCHAR drive,CRKLog *pLog);
	static BOOL GetDemoDrive(STRUCT_RKDEVICE_DESC &device,DWORD devInst,CRKLog *pLog);
	static BOOL GetProperty(CONST STRUCT_RKDEVICE_DESC &device,CString &diskPath,ENUM_DEVPROP propertyIndex,CString &propertyValue,CRKLog *pLog);
	static TCHAR QueryVolume(CString strPDOName);
	int FindConfigSetPos(RKDEVICE_CONFIG_SET &devConfigSet,USHORT vid,USHORT pid);
	int FindWaitSetPos(CONST RKDEVICE_CONFIG_SET &waitDeviceSet,USHORT vid,USHORT pid);
	BOOL IsCompositeDevice(DWORD devInst);
	BOOL IsRockusbDevice(ENUM_RKDEVICE_TYPE &type,USHORT vid,USHORT pid);
/*usbenum code*/
	VOID DriverNameToDeviceInstance(TCHAR* DriverName, PDWORD pDevInst);
	BOOL GetDeviceIDByDriveLetter(TCHAR *driveID,TCHAR drive);
	TCHAR GetDriveLetterByDeviceID(const TCHAR* deviceID);
	DWORD GetDrivesDevInstByDeviceNumber(long DeviceNumber,UINT DriveType,TCHAR* szDosDeviceName);
	VOID EnumerateHostControllers();
	VOID EnumerateHostControllers2();
	VOID EnumerateHostController(int nHostIndex);
	VOID EnumerateHostController2 (int nHostIndex);
	VOID EnumerateHub (int nHubIndex,BOOL bExternal);
	VOID EnumerateHubPorts (int nHubIndex,BOOL bExternal);
	TCHAR* GetRootHubName (HANDLE HostController);
	TCHAR* GetExternalHubName (HANDLE Hub,ULONG ConnectionIndex);
	TCHAR* Usb_GetDriverKeyName (HANDLE Hub,ULONG ConnectionIndex);
	VOID BuildDevicePath(TCHAR *buf, CString& DevicePath);
	PCHAR Usb_WideStrToMultiStr(PWCHAR WideStr);
	int FindHostInfo(CString strDevicePath);
	int FindHubInfo(CString strDevicePath);
	void InitHubInfo(STRUCT_HUB_INFO &hub);
	void InitHostInfo(STRUCT_HOST_INFO &host);
};
#endif