#ifndef RKPACKAGE_HEADER
#define RKPACKAGE_HEADER
#include "DefineHeader.h"
#include "RKImage.h"
#define  PACKAGE_HEAD_RESERVED_SIZE 22
#define  PACKAGE_ENTRY_RESERVED_SIZE 7

typedef enum
{
	FW_RKNANOA=0x30,
	FW_RKNANOB
}ENUM_FW_TYPE;
#pragma pack(1)
typedef struct  
{
	UINT   uiTag;
	USHORT usSize;
	DWORD  dwMergeVersion;
	STRUCT_RKTIME stReleaseTime;
	ENUM_RKDEVICE_TYPE emSupportChip;
	UCHAR ucDefaultLoadEntry;
	UCHAR ucFwEntryCount;
	DWORD dwFwEntryOffset;
	UCHAR ucFwEntrySize;
	UCHAR reserved[PACKAGE_HEAD_RESERVED_SIZE];
}STRUCT_RKPACKAGE_HEAD,*PSTRUCT_RKPACKAGE_HEAD;
typedef struct  
{
	UCHAR ucSize;
	ENUM_FW_TYPE emType;
	DWORD dwDataOffset;
	DWORD dwDataSize;
	UCHAR reserved[PACKAGE_ENTRY_RESERVED_SIZE];
}STRUCT_RKPACKAGE_FWENTRY,*PSTRUCT_RKPACKAGE_FWENTRY;

#pragma pack()
class CRKPackage
{
public:
	GETPROP(DWORD,MergeVersion);
	GETPROP(STRUCT_RKTIME,ReleaseTime);
	GETPROP(ENUM_RKDEVICE_TYPE,SupportDevice);
	GETPROP(UCHAR,DefaultLoadEntry);
	GETPROP(UCHAR,FwEntryCount);
	GET(DWORD,MergeVersion);
	GET(STRUCT_RKTIME,ReleaseTime);
	GET(ENUM_RKDEVICE_TYPE,SupportDevice);
	GET(UCHAR,DefaultLoadEntry);
	GET(UCHAR,FwEntryCount);
	
	LPVOID *m_pImage;
	CStringArray  m_imagePathArray;
//	BOOL Md5Check();
	BOOL SaveFwFile(UCHAR index,CString strSaveName);
	BOOL GetFwEntry(UCHAR index,STRUCT_RKPACKAGE_FWENTRY &entry);
	VOID GetReservedData(LPBYTE &lpData,USHORT &usSize);
	CRKPackage(CString filename,BOOL &bCheck);
	~CRKPackage();
protected:
	
private:
	DWORD m_mergeVersion;
	STRUCT_RKTIME m_releaseTime;
	ENUM_RKDEVICE_TYPE m_supportDevice;
	UCHAR m_defaultLoadEntry;
	UCHAR m_fwEntryCount;
	DWORD m_fwEntryOffset;
	UCHAR m_fwEntrySize;
	CCriticalSection m_mutex;
	BYTE  m_md5[32];
	BYTE  m_reserved[PACKAGE_HEAD_RESERVED_SIZE];
	CFile *m_pFile;
};

#endif