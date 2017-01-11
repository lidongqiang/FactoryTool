#ifndef RKNANODEVICE_HEADER
#define RKNANODEVICE_HEADER
#include "RKDevice.h"

#define IDB_ONCE_TRANS_SEC	4

#pragma pack(1)/* 按1字节对齐 */
typedef	struct 
{
	DWORD	dwTag;
	BYTE	reserved[4];
	UINT	uiRc4Flag;
	BYTE	reserved2[44];
	DWORD   dwNandBoot2Offset;
	DWORD	dwNandBoot2Size;
	DWORD	dwNandBoot1stOffset;
	DWORD	dwNandBoot1stSize;
	DWORD	dwNandBoot2stOffset;
	DWORD	dwNandBoot2stSize;
	USHORT	usCrc[8];
	BYTE	reserved1[416];
}RKNANO_IDB_SEC0,*PRKNANO_IDB_SEC0;

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
	USHORT	usFirmwareMasterVer;		// (24-25)	BCD编码 固件主版本
    USHORT	usFirmwareSlaveVer;			// (26-27)	BCD编码 固件从版本
    USHORT  usFirmwareSmallVer;			// (28-29)	BCD编码 固件子版本  
	USHORT  usLastLoaderVer;
	USHORT  usReadWriteTimes;
	USHORT  usMachineInfoLen;
	UCHAR	ucMachineInfo[30];
	USHORT	usManufactoryInfoLen;
	UCHAR	ucManufactoryInfo[30];
	USHORT	usFlashInfoOffset;
	USHORT	usFlashInfoLen;
    BYTE	Reserved2[382];				// (102-483)保留
	UINT	uiFlashSize;				//以sector为单位
	USHORT  usBlockSize;
	BYTE    bPageSize;
	BYTE    bECCBits;
	BYTE    bAccessTime;
    BYTE    reserved2[5];
	USHORT  usFirstReservedBlock;		// (498-499)Idblock之后第一个保留块的的位置		(单位:block)
    USHORT  usLastReservedBlock;		// (500-501)Idblock之后最后一个保留块的的位置	(单位:block)
	USHORT  usIdBlock0;
	USHORT  usIdBlock1;
	USHORT  usIdBlock2;
	USHORT  usIdBlock3;
	USHORT  usIdBlock4;
}RKNANO_IDB_SEC1,*PRKNANO_IDB_SEC1;

typedef struct
{
    USHORT  usInfoSize;                        
    BYTE    bChipInfo[CHIPINFO_LEN];
	BYTE    reserved[RKNANO_SEC2_RESERVED_LEN];
	CHAR    szVcTag[3];
	USHORT  usSec0Crc;
	USHORT  usSec1Crc;
	UINT	uiBootCodeCrc;
	USHORT  usSec3CustomDataOffset;
	USHORT  usSec3CustomDataSize;
	CHAR    szCrcTag[4];
	USHORT  usSec3Crc;
}RKNANO_IDB_SEC2,*PRKNANO_IDB_SEC2;


typedef struct
{
    USHORT  usSNSize;                        
    BYTE    sn[RKDEVICE_SN_LEN];
	BYTE    reserved[RKNANO_SEC3_RESERVED_LEN];
	BYTE	uidSize;
	BYTE	uid[RKDEVICE_UID_LEN];
	BYTE    blueToothSize;
	BYTE	blueToothAddr[RKDEVICE_BT_LEN];
	BYTE	macSize;
	BYTE	macAddr[RKDEVICE_MAC_LEN];
}RKNANO_IDB_SEC3,*PRKNANO_IDB_SEC3;
typedef struct  
{
	DWORD  dwTag;
	USHORT usSnSize;
	BYTE   btSnData[30];
	BYTE   btReserve[RKNANO_SEC3_RESERVED_LEN+30-6];
	BYTE   btUidSize;
	BYTE   btUidData[RKDEVICE_UID_LEN];
	BYTE   btBlueToothSize;
	BYTE   btBlueToothData[RKDEVICE_BT_LEN];
	BYTE   btMacSize;
	BYTE   btMacData[RKDEVICE_MAC_LEN];
	USHORT usCrc;
	BYTE   btSpare[SPARE_SIZE];
}STRUCT_NANO_WBBUFFER,*PSTRUCT_NANO_WBBUFFER;//这里定义为512+16结构，回写时需要调整成2048+64的结构
#pragma pack()

class CRKNanoDevice :public CRKDevice
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
	CRKNanoDevice(STRUCT_RKDEVICE_DESC &device);
	~CRKNanoDevice();
protected:
private:
	DWORD  m_dwLoaderSize;
	CHAR   m_oldIDBCounts;
	BOOL   m_bClearUpgradeTimes;
	RKNANO_IDB_SEC0 *m_oldSec0;
	RKNANO_IDB_SEC1 *m_oldSec1;
	RKNANO_IDB_SEC2 *m_oldSec2;
	RKNANO_IDB_SEC3 *m_oldSec3;
	BOOL GetNewDiskSize();
	BOOL GetLoaderSize();
	BOOL GetOldSectorData();
	BOOL IsExistSector3Crc(PRKNANO_IDB_SEC2 pSec);
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
};


#endif