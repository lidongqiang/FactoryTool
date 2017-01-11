#ifndef RK28DEVICE_HEADER
#define RK28DEVICE_HEADER
#include "RKDevice.h"
#pragma pack(1)/* ��1�ֽڶ��� */
typedef	struct 
{
	DWORD	dwTag;
	BYTE	reserved[4];
	UINT	uiRc4Flag;
	USHORT	usBootCode1Offset;
	USHORT	usBootCode2Offset;
	BYTE	reserved1[490];
	USHORT  usBootDataSize;
	USHORT	usBootCodeSize;
	USHORT	usCrc;
}RK28_IDB_SEC0,*PRK28_IDB_SEC0;

typedef struct 
{
    USHORT  usSysReservedBlock;                 
    USHORT  usDisk0Size;				
	USHORT  usDisk1Size;			
	USHORT  usDisk2Size;				
	USHORT  usDisk3Size;					
	UINT	uiChipTag;
	UINT	uiMachineId;
	USHORT	usLoaderYear;
	USHORT	usLoaderDate;
	USHORT	usLoaderVer;
	USHORT  usLastLoaderVer;
	USHORT  usReadWriteTimes;
	DWORD	dwFwVer;
	USHORT  usMachineInfoLen;
	UCHAR	ucMachineInfo[30];
	USHORT	usManufactoryInfoLen;
	UCHAR	ucManufactoryInfo[30];
	USHORT	usFlashInfoOffset;
	USHORT	usFlashInfoLen;
	UCHAR	reserved[384];
	UINT	uiFlashSize;//��sectorΪ��λ
	BYTE    reserved1;
	BYTE    bAccessTime;
    USHORT  usBlockSize;
    BYTE    bPageSize;
    BYTE    bECCBits;
    BYTE    reserved2[8];
	USHORT  usIdBlock0;
	USHORT  usIdBlock1;
	USHORT  usIdBlock2;
	USHORT  usIdBlock3;
	USHORT  usIdBlock4;
}RK28_IDB_SEC1,*PRK28_IDB_SEC1;

typedef struct
{
    USHORT  usInfoSize;                        
    BYTE    bChipInfo[CHIPINFO_LEN];
	BYTE    reserved[RK28_SEC2_RESERVED_LEN];
	CHAR    szVcTag[3];
	USHORT  usSec0Crc;
	USHORT  usSec1Crc;
	UINT	uiBootCodeCrc;
	USHORT  usSec3CustomDataOffset;
	USHORT  usSec3CustomDataSize;
	CHAR    szCrcTag[4];
	USHORT  usSec3Crc;
}RK28_IDB_SEC2,*PRK28_IDB_SEC2;

typedef struct
{
    USHORT  usSNSize;                        
    BYTE    sn[RKDEVICE_SN_LEN];
	BYTE    reserved[RK28_SEC3_RESERVED_LEN];
	BYTE	wifiSize;
	BYTE	wifiAddr[RKDEVICE_WIFI_LEN];
	BYTE	imeiSize;
	BYTE	imei[RKDEVICE_IMEI_LEN];
	BYTE	uidSize;
	BYTE	uid[RKDEVICE_UID_LEN];
	BYTE    blueToothSize;
	BYTE	blueToothAddr[RKDEVICE_BT_LEN];
	BYTE	macSize;
	BYTE	macAddr[RKDEVICE_MAC_LEN];
}RK28_IDB_SEC3,*PRK28_IDB_SEC3;
typedef struct  
{
	DWORD  dwTag;
	USHORT usSnSize;
	BYTE   btSnData[30];
	BYTE   btReserve[RK28_SEC3_RESERVED_LEN+30+7-6];
	BYTE   btImeiSize;
	BYTE   btImeiData[RKDEVICE_IMEI_LEN];
	BYTE   btUidSize;
	BYTE   btUidData[RKDEVICE_UID_LEN];
	BYTE   btBlueToothSize;
	BYTE   btBlueToothData[RKDEVICE_BT_LEN];
	BYTE   btMacSize;
	BYTE   btMacData[RKDEVICE_MAC_LEN];
	USHORT usCrc;
	BYTE   btSpare[SPARE_SIZE];
}STRUCT_RK28_WBBUFFER,*PSTRUCT_RK28_WBBUFFER;
#pragma pack()

class CRK28Device :public CRKDevice
{
public:
	virtual int DownloadBoot();
	virtual int PrepareIDB(BOOL &bNeedJump);
	virtual int DownloadIDBlock();
	virtual int DownloadImage();
	virtual int EraseIDB();
	virtual int EraseAllBlocks();
	virtual int ReadSN();
	virtual int WriteSN();
	virtual int ReadMac();
	virtual int WriteMac();
	virtual int ReadWifi();
	virtual int WriteWifi();
	virtual int ReadImei();
	virtual int WriteImei();
	virtual int ReadBTAddr();
	virtual int WriteBTAddr();
	virtual int ReadUID();
	virtual int ReadCustomData();
	virtual int WriteCustomData();
	virtual int ReadAllInfo();
	virtual int WriteAllInfo();
	virtual int GetOldDiskSize();
	virtual int GetBlocksState();
	virtual BOOL BufferWriteBack();
	CRK28Device(STRUCT_RKDEVICE_DESC &device);
	~CRK28Device();
protected:
private:
	DWORD  m_dwLoaderSize;
	DWORD  m_dwLoaderDataSize;
	CHAR   m_oldIDBCounts;
	USHORT m_usFlashDataSec;
	USHORT m_usFlashBootSec;
	RK28_IDB_SEC0 *m_oldSec0;
	RK28_IDB_SEC1 *m_oldSec1;
	RK28_IDB_SEC2 *m_oldSec2;
	RK28_IDB_SEC3 *m_oldSec3;
	BOOL GetNewDiskSize();
	BOOL GetLoaderSize();
	BOOL GetLoaderDataSize();
	BOOL GetOldSectorData();
	BOOL CalcIDBCount(BOOL bUpdateLoader);
	BOOL IsExistSector3Crc(PRK28_IDB_SEC2 pSec);
	BOOL CheckIDBData(PBYTE pIDBData,BOOL bCheckBootCode=FALSE);
	VOID IDB528To512(PBYTE pIDB528,PBYTE pIDB512,PBYTE pSpare,DWORD dwSectorNum);
	VOID IDBRC4(PBYTE pIDB);
	virtual BOOL FindBackupBuffer();
	virtual CHAR FindIDBlock(CHAR pos,CHAR &IDBlockPos);
	virtual BOOL FindAllIDB();
	virtual BOOL ReserveIDBlock(CHAR iBlockIndex=0,CHAR iIdblockPos=0);
	virtual BOOL OffsetIDBlock(CHAR pos);
	virtual BOOL MakeSector0(PBYTE pSector);
	virtual VOID MakeSector1(PBYTE pSector);
	virtual BOOL MakeSector2(PBYTE pSector);
	virtual BOOL MakeSector3(PBYTE pSector);
	virtual int MakeIDBlockData(LPBYTE lpIDBlock);
	virtual BOOL MakeSpareData(LPBYTE lpIDBlock,DWORD dwSectorNum,LPBYTE lpSpareBuffer);
	virtual int WriteIDBlock(LPBYTE lpIDBlock,LPBYTE lpSpareData,DWORD dwSectorNum,BOOL bErase);
	BOOL Boot_VendorRequest(HANDLE hDevice, DWORD requestCode, PBYTE pBuffer, DWORD dwDataSize);
	BOOL ExportNoIDBData();
};
#endif