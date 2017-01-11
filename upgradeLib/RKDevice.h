#ifndef RKDEVICE_HEADER
#define RKDEVICE_HEADER
#include "RKImage.h"
#include "RKComm.h"
#include "RKLog.h"
#include "DefineHeader.h"

#define RKNANO_SEC3_RESERVED_LEN 405
#define RK28_SEC3_RESERVED_LEN 382
#define RKANDROID_SEC3_RESERVED_LEN 382
#define RKDEVICE_SEC3_RESERVED_LEN 382

#define RK28_SEC2_RESERVED_LEN 473
#define RKCROWN_SEC2_RESERVED_LEN 484
#define RKANDROID_SEC2_RESERVED_LEN 473
#define RKNANO_SEC2_RESERVED_LEN 473

#define RKDEVICE_CUSTOMDATA_LEN 512
#define RKDEVICE_SN_LEN 60
#define RKDEVICE_UID_LEN 30
#define RKDEVICE_MAC_LEN 6
#define RKDEVICE_WIFI_LEN 6
#define RKDEVICE_BT_LEN 6
#define RKDEVICE_IMEI_LEN 15

#define SECTOR_SIZE 512
#define PAGE_SIZE 2048
#define SPARE_SIZE 16
#define IDB_BLOCKS 5
#define CHIPINFO_LEN 16
#define IDBLOCK_TOP 50
#define WBBUFFER_BOTTOM 5
#define WBBUFFER_TOP 10
#define CALC_UNIT(a, b)		((a>0)?((a-1)/b+1):(a))
#define BYTE2SECTOR(x)		(CALC_UNIT(x, SECTOR_SIZE))
#define PAGEALIGN(x)		(CALC_UNIT(x, 4))
//#define MAX_TEST_BLOCKS		512
//#define MAX_ERASE_BLOCKS	128
#define MAX_WRITE_SECTOR	16
#define IOCTL_TRANSFER_SIZE		4096



#define CRC_TAG "CRC"
#pragma pack(1)
typedef struct _STRUCT_FLASH_INFO
{
	TCHAR	szManufacturerName[16];
	UINT	uiFlashSize;  //MB
	USHORT	usBlockSize;//KB
	UINT	uiPageSize;   //KB
	UINT	uiSectorPerBlock;
	BYTE	blockState[IDBLOCK_TOP];
	UINT	uiBlockNum;
	BYTE	bECCBits;
	BYTE	bAccessTime;  //两次访问Flash的间隔时间
	BYTE	bFlashCS;  // Flash片选(Flash片选存在置1，否则置0)
	USHORT  usValidSecPerBlock;//每块可以使用的扇区数=块大小/页大小*4
	USHORT  usPhyBlokcPerIDB;//每个IDBlock占用的物理块数量
	UINT    uiSecNumPerIDB;//每个IDBlock占用的扇区数
}STRUCT_FLASH_INFO, *PSTRUCT_FLASH_INFO;
typedef struct _STRUCT_FLASHINFO_CMD
{
	UINT	uiFlashSize;	// Flash大小（以Sector为单位）
	USHORT	usBlockSize;	// 物理的Block大小（以Sector为单位）
	BYTE	bPageSize;		// 物理的Page大小（以Sector为单位）
	BYTE	bECCBits;		// 8/14
	BYTE	bAccessTime;	// 两次访问Flash的间隔时间
	BYTE	bManufCode;		// 厂商识别码
	BYTE	bFlashCS;		// Flash片选(若Flash片选存在，则将相应的Bit置1，否则置0)
	BYTE	reserved[501];
}STRUCT_FLASHINFO_CMD, *PSTRUCT_FLASHINFO_CMD;
typedef struct  
{
	BYTE bFlashCS;
	UINT uiBlockNum;
	USHORT usBlockStateSize;
	PBYTE pBlockStateData;
}STRUCT_BLOCK_STATE,*PSTRUCT_BLOCK_STATE;
#pragma pack()
typedef struct
{
	USHORT usVid;
	USHORT usPid;
}STRUCT_DEVICE_PROP;
typedef vector<STRUCT_DEVICE_PROP> DEVICE_PROP_SET;

class CRKDevice
{
public:
	
	GETPROP(CRKComm*,CommObject);
	GET(CRKComm*,CommObject);

	PROP(USHORT,VendorID);
	PROP(USHORT,ProductID);
	PROP(ENUM_RKDEVICE_TYPE,DeviceType);
	PROP(ENUM_RKUSB_TYPE,UsbType);
	PROP(CString,LinkName);
	PROP(CString,LayerName);
	PROP(TCHAR,Drive);
	PROP(DWORD,DeviceInstance);
	PROP(USHORT,BCDUSB);
	PROP(BOOL,Usb20);
	GETPROP(USHORT,SysDiskSize);
	GETPROP(USHORT,CfgDiskSize);

	PROP(ENUM_OS_TYPE,OsType);
	
	SETPROP(BOOL,GetNewDiskSizeFlag);
	SET(BOOL,GetNewDiskSizeFlag);

	PROP(PSTRUCT_BLOCK_STATE,BlockStatePointer);
	SET(PSTRUCT_BLOCK_STATE,BlockStatePointer);
	GET(PSTRUCT_BLOCK_STATE,BlockStatePointer);

	PROP(BOOL,RemallocDisk);
	SET(BOOL,RemallocDisk);
	GET(BOOL,RemallocDisk);

	SETPROP(BOOL,PrepareEraseFlag);
	SET(BOOL,PrepareEraseFlag);

	SETPROP(UINT,WorkFlow);
	SET(UINT,WorkFlow);

// 	SETPROP(BOOL,MiscEraseDataFlag);
// 	SET(BOOL,MiscEraseDataFlag);

	SETPROP(ENUM_MISC_MODIFY_FLAG,MiscModifyFlag);
	SET(ENUM_MISC_MODIFY_FLAG,MiscModifyFlag);

	SETPROP(BOOL,QuickCheckMode);
	SET(BOOL,QuickCheckMode);
	
	SETPROP(UINT,ForceDataBand);
	SET(UINT,ForceDataBand);

	SETPROP(BOOL,NoCheckMode);
	SET(BOOL,NoCheckMode);

	GETPROP(BOOL,UidWriteOK);
	GET(BOOL,UidWriteOK);
	
	PROP(LPBYTE,Sn);
	PROP(BYTE,SnSize);
	SET(LPBYTE,Sn);
	SET(BYTE,SnSize);
	GET(LPBYTE,Sn);
	GET(BYTE,SnSize);

	PROP(LPBYTE,Mac);
	GETPROP(BYTE,MacSize);
	SET(LPBYTE,Mac);
	GET(LPBYTE,Mac);
	GET(BYTE,MacSize);

	PROP(LPBYTE,Wifi);
	GETPROP(BYTE,WifiSize);
	SET(LPBYTE,Wifi);
	GET(LPBYTE,Wifi);
	GET(BYTE,WifiSize);

	PROP(LPBYTE,Imei);
	GETPROP(BYTE,ImeiSize);
	SET(LPBYTE,Imei);
	GET(LPBYTE,Imei);
	GET(BYTE,ImeiSize);

	PROP(LPBYTE,BTAddr);
	GETPROP(BYTE,BTSize);
	SET(LPBYTE,BTAddr);
	GET(LPBYTE,BTAddr);
	GET(BYTE,BTSize);
	
	PROP(LPBYTE,Uid);
	GETPROP(BYTE,UidSize);
	SET(LPBYTE,Uid);
	GET(LPBYTE,Uid);
	GET(BYTE,UidSize);

	PROP(LPBYTE,ReservedData);
	PROP(USHORT,ReservedDataSize);
	SET(LPBYTE,ReservedData);
	SET(USHORT,ReservedDataSize);
	GET(LPBYTE,ReservedData);
	GET(USHORT,ReservedDataSize);

	PROP(LPBYTE,CustomData);
	PROP(USHORT,CustomDataSize);
	PROP(USHORT,CustomDataOffset);
	SET(LPBYTE,CustomData);
	SET(USHORT,CustomDataSize);
	SET(USHORT,CustomDataOffset);
	GET(LPBYTE,CustomData);
	GET(USHORT,CustomDataSize);
	GET(USHORT,CustomDataOffset);



	GETPROP(CRKLog*,LogObjectPointer);
	GET(CRKLog*,LogObjectPointer);

	GET(USHORT,SysDiskSize);
	GET(USHORT,CfgDiskSize);
	GET(USHORT,VendorID);
	GET(USHORT,ProductID);
	GET(ENUM_RKDEVICE_TYPE,DeviceType);
	GET(ENUM_RKUSB_TYPE,UsbType);
	GET(CString,LinkName);
	GET(CString,LayerName);
	GET(TCHAR,Drive);
	GET(DWORD,DeviceInstance);
	GET(USHORT,BCDUSB);
	GET(ENUM_OS_TYPE,OsType);
	GET(BOOL,Usb20);
	
	SET(USHORT,VendorID);
	SET(USHORT,ProductID);
	SET(ENUM_RKDEVICE_TYPE,DeviceType);
	SET(ENUM_RKUSB_TYPE,UsbType);
	SET(CString,LinkName);
	SET(CString,LayerName);
	SET(TCHAR,Drive);
	SET(DWORD,DeviceInstance);
	SET(USHORT,BCDUSB);
	SET(ENUM_OS_TYPE,OsType);
	SET(BOOL,Usb20);

	PROP(CString,DiskVolume);
	PROP(CString,DataPath);
	PROP(CString,DataDiskVolume);
	PROP(CString,DataDiskPath);
	SETPROP(ProgressPromptCB,CallBackPointer);
	SET(ProgressPromptCB,CallBackPointer);
	SET(CString,DiskVolume);
	SET(CString,DataPath);
	GET(CString,DiskVolume);
	GET(CString,DataPath);

	SET(CString,DataDiskVolume);
	SET(CString,DataDiskPath);
	GET(CString,DataDiskVolume);
	GET(CString,DataDiskPath);

	static BOOL FormatDisk(TCHAR drive,LPCTSTR lpDiskVolume,CRKLog *pLog=NULL);
	static BOOL CopyData(TCHAR drive,LPCTSTR lpDataFrom,CRKLog *pLog=NULL);
	static BOOL SendExtCommand(TCHAR drive,DWORD dwCode,CRKLog *pLog=NULL,PBYTE pOutData=NULL,UINT uiOutDataLen=0);
	static UINT GetRockusbVidPid(DEVICE_PROP_SET &allVidPid,ENUM_RKDEVICE_TYPE devType=RKNONE_DEVICE);
	static BOOL IsRockusbPlug(DEVICE_PROP_SET &allVidPid,CString &strDeviceID);
	static int  InstallRockusbDriver(CString strHardwareID,CString strInfFile,CRKLog *pLog=NULL);
	//static BOOL SwitchAdbToRockusb(CString strInterfaceName,CRKLog *pLog=NULL);
	virtual int DownloadBoot()=0;
	virtual int DownloadIDBlock()=0;
	virtual int DownloadImage()=0;
	virtual int PrepareIDB(BOOL &bNeedJump)=0;
	virtual BOOL BufferWriteBack()=0;
	
	BOOL TestDevice();
	BOOL LowerFormatDevice(BOOL &bSupport);
	BOOL ResetDevice();
	BOOL SetResetFlag();
	BOOL PowerOffDevice();
	BOOL DisconnectUsbReset();
	BOOL CheckChip();
	BOOL GetFlashInfo();
	virtual int EraseIDB()=0;
	virtual int EraseAllBlocks()=0;
	virtual int ReadSN()=0;
	virtual int WriteSN()=0;
	virtual int ReadMac()=0;
	virtual int WriteMac()=0;
	virtual int ReadWifi()=0;
	virtual int WriteWifi()=0;
	virtual int ReadImei()=0;
	virtual int WriteImei()=0;
	virtual int ReadBTAddr()=0;
	virtual int WriteBTAddr()=0;
	virtual int GetOldDiskSize()=0;
	virtual int GetBlocksState()=0;
	virtual int ReadUID()=0;
	virtual int ReadCustomData()=0;
	virtual int WriteCustomData()=0;
	virtual int ReadAllInfo()=0;
	virtual int WriteAllInfo()=0;
	BOOL SetObject(CRKImage *pImage,CRKComm *pComm,CRKLog *pLog);
	DWORD GetLayerID();
	CRKDevice(STRUCT_RKDEVICE_DESC &device);
	~CRKDevice();
protected:
	STRUCT_FLASH_INFO m_flashInfo;
	PBYTE	m_pFlashInfoData;
	USHORT	m_usFlashInfoDataOffset;
	USHORT  m_usFlashInfoDataLen;
	DWORD m_idBlockOffset[IDB_BLOCKS];
	PBYTE  m_chipData;
	CRKImage *m_pImage;
	CRKComm  *m_pComm;
	CRKLog   *m_pLog;
	PBYTE  m_customData;
	USHORT m_customDataSize;
	USHORT m_customDataOffset;
	PBYTE  m_sn;
	BYTE   m_snSize;
	PBYTE  m_mac;
	PBYTE  m_blueTooth;
	PBYTE  m_uid;
	PBYTE  m_imei;
	PBYTE  m_wifi;
	PBYTE  m_reservedData;
	USHORT m_reservedDataSize;
	USHORT m_sysDiskSize;
	USHORT m_cfgDiskSize;
	BOOL   m_bGetNewDiskSizeFlag;
	BOOL   m_bExistSector3Crc;
	USHORT m_usSector3Crc;
	USHORT m_usWriteBackCrc;
	USHORT m_usWriteBackCustomDataOffset;
	USHORT m_usWriteBackCustomDataSize;
	PSTRUCT_BLOCK_STATE m_pBlockState;
	BYTE   m_backupBuffer[SECTOR_SIZE+SPARE_SIZE];
	ProgressPromptCB m_callBackProc;
	BOOL m_bWriteBack;
	UINT m_uiWorkFlow;
	BOOL m_bEraseInPrepare;
	BOOL m_bUidUseFlag;
	BOOL m_bUidWriteOK;
	BOOL  m_remallocDisk;
	BOOL   m_bEmmc;
	ENUM_MISC_MODIFY_FLAG m_emMiscModifyFlag;
	BOOL m_bQuickCheckMode;
	BOOL m_bNoCheckMode;
	UINT m_uiForceDataBand;
	BOOL BuildBlockStateMap(BYTE bFlashCS);
	int ReadMutilSector(DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer);
	BOOL EraseMutilBlock(BYTE bFlashCS,DWORD dwPos,DWORD dwCount,BOOL bForce);
	CHAR FindValidBlocks(CHAR bBegin, CHAR bLen);
	BYTE RandomByte(BYTE bLowLimit,BYTE bHighLimit);
	BOOL CheckCrc16(PBYTE pCheckData,USHORT usDataLength,USHORT usOldCrc);
	BOOL CheckUid(BYTE uidSize,BYTE *pUid);
	BOOL GetWriteBackData(UINT uiIDBCount,LPBYTE lpBuf);
	BOOL GetIDBData(UINT uiIDBCount,LPBYTE lpBuf,UINT uiSecCount);
	int  EraseEmmcBlock(UCHAR ucFlashCS,DWORD dwPos,DWORD dwCount);
	BOOL EraseEmmc();
	int EraseEmmcByWriteLBA(DWORD dwSectorPos,DWORD dwCount);
	virtual BOOL FindBackupBuffer()=0;
	virtual BOOL FindAllIDB()=0;
	virtual CHAR FindIDBlock(CHAR pos,CHAR &IDBlockPos)=0;
	virtual BOOL ReserveIDBlock(CHAR iBlockIndex=0,CHAR iIdblockPos=0)=0;
	virtual BOOL OffsetIDBlock(CHAR pos)=0;		
	virtual BOOL MakeSector0(PBYTE pSector)=0;
	virtual VOID MakeSector1(PBYTE pSector)=0;
	virtual BOOL MakeSector2(PBYTE pSector)=0;
	virtual BOOL MakeSector3(PBYTE pSector)=0;
	virtual int MakeIDBlockData(LPBYTE lpIDBlock)=0;
	virtual BOOL MakeSpareData(LPBYTE lpIDBlock,DWORD dwSectorNum,LPBYTE lpSpareBuffer)=0;
	virtual int WriteIDBlock(LPBYTE lpIDBlock,LPBYTE lpSpareData,DWORD dwSectorNum,BOOL bErase)=0;
	
private:
	USHORT m_vid;
	USHORT m_pid;
	ENUM_RKDEVICE_TYPE m_device;
	ENUM_OS_TYPE m_os;
	ENUM_RKUSB_TYPE m_usb;
	DWORD m_deviceInstance;
	USHORT m_bcdUsb;
	BOOL   m_bUsb20;
	
	CString m_linkName;
	CString m_layerName;
	CString m_oldLayerName;
	CString m_diskVolume;
	CString m_dataPath;
	CString m_dataDiskVolume;
	CString m_dataDiskPath;
	TCHAR m_drive;	
	
};
typedef struct 
{
	BOOL	bFound;
	CString strName;
	HWND	hWnd;
}STRUCT_CTRLINFO, *PSTRUCT_CTRLINFO;

typedef struct 
{
	CString	strTitle;
	STRUCT_CTRLINFO ctrlInfo;
}STRUCT_ENUMPARAM, *PSTRUCT_ENUMPARAM;

typedef struct 
{
	PVOID	pNotifyEvent;
	CString strHardwareID;
	CString strInfFile;
	CRKLog  *pLog;
	int iRetCode;
}STRUCT_INSTALLDRIVER_PARAM, *PSTRUCT_INSTALLDRIVER_PARAM;
 BOOL GetSystemVersion(DWORD *dwMajorVer, DWORD *dwMinorVer);
 //BOOL IsWinusbDllExisted();
 //VOID KillADB();
 BOOL CloseFoundDlg_NHW();
 BOOL CALLBACK CloseWindowsProc(HWND hWnd, LPARAM lParam);
 BOOL CALLBACK MatchChildWindowsProc(HWND hWnd, LPARAM lParam);
 BOOL ConfirmSafeDlg(int &installRet);
 BOOL CALLBACK ClickCtrlProc(HWND hWnd, LPARAM lParam);
 int InstallDriver(CString strHardwareID, CString strInfFile,CRKLog *pLog=NULL);
 UINT InstallDriverThread(LPVOID lpParam);

USHORT UshortToBCD(USHORT num);
BYTE   ByteToBCD(BYTE num);
extern USHORT CRC_16(BYTE * aData, ULONG aSize);
extern ULONG CRC_32(PBYTE pData, ULONG ulSize);
extern void P_RC4(BYTE * buf, USHORT len);
extern void bch_encode(BYTE *encode_in, BYTE *encode_out);
extern USHORT CRC_CCITT(UCHAR *p, UINT CalculateNumber);
extern void generate_gf();
extern void gen_poly();
#endif