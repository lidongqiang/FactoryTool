#ifndef RK27DEVICE_HEADER
#define RK27DEVICE_HEADER
#include "RKDevice.h"
#pragma pack(1)/* 按1字节对齐 */
typedef	struct 
{
	DWORD	dwTag;
	BYTE	reserved[8];
	USHORT	usBootCode1Offset;
	USHORT	usBootCode2Offset;
	BYTE	reserved1[492];
	USHORT	usBootCodeSize;
	USHORT	usCrc;
}RK27_IDB_SEC0,*PRK27_IDB_SEC0;

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
	UINT	uiFlashSize;//以sector为单位
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
}RK27_IDB_SEC1,*PRK27_IDB_SEC1;

typedef struct
{
    USHORT  usInfoSize;                        
    BYTE    bChipInfo[CHIPINFO_LEN];
	BYTE    reserved[494];
}RK27_IDB_SEC2,*PRK27_IDB_SEC2;



typedef struct
{
    USHORT  usSNSize;                        
    BYTE    sn[RKDEVICE_SN_LEN];
	BYTE    reserved[RK27_SEC3_RESERVED_LEN];
	BYTE    blueToothSize;
	BYTE	blueToothAddr[RKDEVICE_BT_LEN];
	BYTE	macSize;
	BYTE	macAddr[RKDEVICE_MAC_LEN];
}RK27_IDB_SEC3,*PRK27_IDB_SEC3;
typedef struct  
{
	DWORD  dwTag;
	USHORT usSnSize;
	BYTE   btSnData[RKDEVICE_SN_LEN];
	BYTE   btReserve[RK27_SEC3_RESERVED_LEN-6];
	BYTE   btBlueToothSize;
	BYTE   btBlueToothData[RKDEVICE_BT_LEN];
	BYTE   btMacSize;
	BYTE   btMacData[RKDEVICE_MAC_LEN];
	USHORT usCrc;
	BYTE   btSpare[SPARE_SIZE];
}STRUCT_RK27_WBBUFFER,*PSTRUCT_RK27_WBBUFFER;
#pragma pack()
class CRK27Device :public CRKDevice
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
	CRK27Device(STRUCT_RKDEVICE_DESC &device);
	~CRK27Device();
protected:
private:
	DWORD  m_dwLoaderSize;
	CHAR   m_oldIDBCounts;
	RK27_IDB_SEC0 *m_oldSec0;
	RK27_IDB_SEC1 *m_oldSec1;
	RK27_IDB_SEC2 *m_oldSec2;
	RK27_IDB_SEC3 *m_oldSec3;
	BOOL GetNewDiskSize();
	BOOL GetLoaderSize();
	BOOL GetOldSectorData();
	virtual BOOL FindBackupBuffer();
	virtual CHAR FindIDBlock(CHAR pos,CHAR &IDBlockPos);
	virtual CHAR FindAllIDB();
	virtual BOOL ReserveIDBlock(CHAR iBlockIndex=0,CHAR iIdblockPos=0);
	virtual BOOL OffsetIDBlock(CHAR pos);
	virtual MakeSector0(PBYTE pSector);
	virtual MakeSector1(PBYTE pSector);
	virtual BOOL MakeSector2(PBYTE pSector);
	virtual BOOL MakeSector3(PBYTE pSector);
	virtual int MakeIDBlockData(LPBYTE lpIDBlock);
	virtual BOOL MakeSpareData(LPBYTE lpIDBlock,DWORD dwSectorNum,LPBYTE lpSpareBuffer);
	virtual int WriteIDBlock(LPBYTE lpIDBlock,LPBYTE lpSpareData,DWORD dwSectorNum,BOOL bErase);
	BOOL Boot_VendorRequest(HANDLE hDevice, DWORD requestCode, PBYTE pBuffer, DWORD dwDataSize);
};

#endif