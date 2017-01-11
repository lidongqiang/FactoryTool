#ifndef RKBOOT_HEADER
#define RKBOOT_HEADER
#include "DefineHeader.h"


#define  BOOT_RESERVED_SIZE 57
#pragma pack(1)
typedef struct  
{
	UINT uiTag;
	USHORT usSize;
	DWORD  dwVersion;
	DWORD  dwMergeVersion;
	STRUCT_RKTIME stReleaseTime;
	ENUM_RKDEVICE_TYPE emSupportChip;
	UCHAR uc471EntryCount;
	DWORD dw471EntryOffset;
	UCHAR uc471EntrySize;
	UCHAR uc472EntryCount;
	DWORD dw472EntryOffset;
	UCHAR uc472EntrySize;
	UCHAR ucLoaderEntryCount;
	DWORD dwLoaderEntryOffset;
	UCHAR ucLoaderEntrySize;
	UCHAR ucSignFlag;
	UCHAR ucRc4Flag;
	UCHAR reserved[BOOT_RESERVED_SIZE];
}STRUCT_RKBOOT_HEAD,*PSTRUCT_RKBOOT_HEAD;
typedef struct  
{
	UCHAR ucSize;
	ENUM_RKBOOTENTRY emType;
	TCHAR szName[20];
	DWORD dwDataOffset;
	DWORD dwDataSize;
	DWORD dwDataDelay;//以秒为单位
}STRUCT_RKBOOT_ENTRY,*PSTRUCT_RKBOOT_ENTRY;
#pragma pack()
class CRKBoot
{
public:
	GETPROP(BOOL,Rc4DisableFlag);
	GETPROP(BOOL,SignFlag);
	GETPROP(DWORD,Version);
	GETPROP(DWORD,MergeVersion);
	GETPROP(STRUCT_RKTIME,ReleaseTime);
	GETPROP(ENUM_RKDEVICE_TYPE,SupportDevice);
	GETPROP(DWORD,Entry471Count);
	GETPROP(DWORD,Entry472Count);
	GETPROP(DWORD,EntryLoaderCount);
	GET(BOOL,Rc4DisableFlag);
	GET(BOOL,SignFlag);
	GET(DWORD,Version);
	GET(DWORD,MergeVersion);
	GET(STRUCT_RKTIME,ReleaseTime);
	GET(ENUM_RKDEVICE_TYPE,SupportDevice);
	GET(DWORD,Entry471Count);
	GET(DWORD,Entry472Count);
	GET(DWORD,EntryLoaderCount);
	BOOL CrcCheck();
	BOOL SaveEntryFile(ENUM_RKBOOTENTRY type,UCHAR ucIndex,CString fileName);
	BOOL GetEntryProperty(ENUM_RKBOOTENTRY type,UCHAR ucIndex,DWORD &dwSize,DWORD &dwDelay,TCHAR *pName=NULL);
	CHAR GetIndexByName(ENUM_RKBOOTENTRY type,TCHAR *pName);
	BOOL GetEntryData(ENUM_RKBOOTENTRY type,UCHAR ucIndex,LPBYTE lpData);
	CRKBoot(LPBYTE lpBootData,DWORD dwBootSize,BOOL &bCheck);
	~CRKBoot();
protected:
private:
	BOOL  m_bRc4Disable;
	BOOL  m_bSignFlag;
	DWORD m_version;
	DWORD m_mergeVersion;
	STRUCT_RKTIME m_releaseTime;
	ENUM_RKDEVICE_TYPE m_supportDevice;
	DWORD m_471Offset;
	UCHAR m_471Size;
	UCHAR m_471Count;
	DWORD m_472Offset;
	UCHAR m_472Size;
	UCHAR m_472Count;
	DWORD m_loaderOffset;
	UCHAR m_loaderSize;
	UCHAR m_loaderCount;
	BYTE  m_crc[4];
	LPBYTE m_BootData;
	DWORD m_BootSize;
	USHORT m_BootHeadSize;
	
};
#endif