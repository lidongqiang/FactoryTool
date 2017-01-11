#ifndef RKANDROIDDEVICE_HEADER
#define RKANDROIDDEVICE_HEADER
#include "RKDevice.h"
#pragma pack(1)/* 按1字节对齐 */
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
}RKANDROID_IDB_SEC0,*PRKANDROID_IDB_SEC0;

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
}RKANDROID_IDB_SEC1,*PRKANDROID_IDB_SEC1;

typedef struct
{
    USHORT  usInfoSize;                        
    BYTE    bChipInfo[CHIPINFO_LEN];
	BYTE    reserved[RKANDROID_SEC2_RESERVED_LEN];
	CHAR    szVcTag[3];
	USHORT  usSec0Crc;
	USHORT  usSec1Crc;
	UINT	uiBootCodeCrc;
	USHORT  usSec3CustomDataOffset;
	USHORT  usSec3CustomDataSize;
	CHAR    szCrcTag[4];
	USHORT  usSec3Crc;
}RKANDROID_IDB_SEC2,*PRKANDROID_IDB_SEC2;

typedef struct
{
    USHORT  usSNSize;                        
    BYTE    sn[RKDEVICE_SN_LEN];
	BYTE    reserved[RKANDROID_SEC3_RESERVED_LEN];
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
}RKANDROID_IDB_SEC3,*PRKANDROID_IDB_SEC3;
typedef struct  
{
	DWORD  dwTag;
	USHORT usSnSize;
	BYTE   btSnData[30];
	BYTE   btReserve[RKANDROID_SEC3_RESERVED_LEN+30+7-6];
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
}STRUCT_RKANDROID_WBBUFFER,*PSTRUCT_RKANDROID_WBBUFFER;
CONST BYTE Wipe_Data[]={0x72,0x65,0x63,0x6F,0x76,0x65,
						0x72,0x79,0x0A,0x2D,0x2D,0x77,
						0x69,0x70,0x65,0x5F,0x64,0x61,0x74,0x61,0x00};
CONST BYTE Wipe_All[]={0x72,0x65,0x63,0x6F,0x76,0x65,
						0x72,0x79,0x0A,0x2D,0x2D,0x77,
						0x69,0x70,0x65,0x5F,0x61,0x6C,0x6C,0x00};
#define LBA_TRANSFER_SIZE		16*1024
#define LBA_LOOP_SIZE	1024*1024
#define QUICK_CHECK_INTERVAL 512*1024
#define QUICK_CHECK_HEAD_SIZE 8*1024
#define QUICK_CHECK_REAR_SIZE 8*1024
#pragma pack()

class CRKAndroidDevice :public CRKDevice
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
	CRKAndroidDevice(STRUCT_RKDEVICE_DESC &device);
	~CRKAndroidDevice();
protected:
private:
	DWORD  m_dwLoaderSize;
	DWORD  m_dwLoaderDataSize;
	DWORD  m_dwBackupOffset;
	BOOL   m_bNeedLowFormat;
	CHAR   m_oldIDBCounts;
	USHORT m_usFlashDataSec;
	USHORT m_usFlashBootSec;
	BYTE   *m_paramBuffer;
	UINT   m_uiParamFileSize;
	UINT   m_uiResevedBlockSize;
	RKANDROID_IDB_SEC0 *m_oldSec0;
	RKANDROID_IDB_SEC1 *m_oldSec1;
	RKANDROID_IDB_SEC2 *m_oldSec2;
	RKANDROID_IDB_SEC3 *m_oldSec3;
	BOOL m_bPhoneFlag;
	UINT m_uiLBATimes;
	UINT m_uiUserSectors;
	BOOL GetNewDiskSize();
	BOOL GetLoaderSize();
	BOOL GetLoaderDataSize();
	BOOL GetOldSectorData();
	BOOL CalcIDBCount(BOOL bUpdateLoader); 
	BOOL IsExistSector3Crc(PRKANDROID_IDB_SEC2 pSec);
	BOOL CheckIDBData(PBYTE pIDBData,BOOL bCheckBootCode=FALSE);
	VOID IDB528To512(PBYTE pIDB528,PBYTE pIDB512,PBYTE pSpare,DWORD dwSectorNum);
	VOID IDBRC4(PBYTE pIDB);
	BOOL EraseSystemPart(DWORD dwLayerID);
	BOOL EraseUserDataPart(DWORD dwLayerID);
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
	BOOL RKA_Param_Download(STRUCT_RKIMAGE_ITEM &entry,UCHAR &loopCount,ULONG64 &currentByte,ULONG64 totalByte,ENUM_CALL_STEP &emCallStep);
	BOOL RKA_Param_Check(STRUCT_RKIMAGE_ITEM &entry,UCHAR &loopCount,ULONG64 &currentByte,ULONG64 totalByte,ENUM_CALL_STEP &emCallStep);
	BOOL RKA_File_Download(STRUCT_RKIMAGE_ITEM &entry,UCHAR &loopCount,ULONG64 &currentByte,ULONG64 totalByte,ENUM_CALL_STEP &emCallStep);
	BOOL RKA_File_Check(STRUCT_RKIMAGE_ITEM &entry,UCHAR &loopCount,ULONG64 &currentByte,ULONG64 totalByte,ENUM_CALL_STEP &emCallStep);
// 	BOOL RKA_Param_Quick_Check(STRUCT_RKIMAGE_ITEM &entry,UCHAR &loopCount,UINT &currentByte,UINT totalByte,ENUM_CALL_STEP &emCallStep);
// 	BOOL RKA_File_Quick_Check(STRUCT_RKIMAGE_ITEM &entry,UCHAR &loopCount,UINT &currentByte,UINT totalByte,ENUM_CALL_STEP &emCallStep);


	BOOL RKA_IsSysImage_Encrypt(STRUCT_RKIMAGE_ITEM &entry,LONG64 &Key);
	BOOL GetParameterFlag(STRUCT_RKIMAGE_ITEM &paramItem);
	BOOL GetParameterPartSize(STRUCT_RKIMAGE_ITEM &paramItem);
	BOOL ParsePartitionInfo(string &strPartInfo,string &strName,UINT &uiOffset,UINT &uiLen);
	BOOL GetReservedBlockByte();
	BOOL MakeParamFileBuffer(STRUCT_RKIMAGE_ITEM &entry);
	BOOL CheckParamPartSize(STRUCT_RKIMAGE_ITEM &paramItem);
	BOOL ExportNoIDBData();
#ifdef _UNICODE
	CString CharToString(char *szInput);
#endif
};
extern void  DES_decrypt(unsigned char* cipher, unsigned char* plain, int len, LONG64 key);
#endif