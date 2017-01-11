#ifndef RKIMAGE_HEADER
#define RKIMAGE_HEADER
#include "DefineHeader.h"
#include "RKBoot.h"
#define  IMAGE_RESERVED_SIZE 61
#pragma pack(1)
typedef struct  
{
	UINT uiTag;
	USHORT usSize;
	DWORD  dwVersion;
	DWORD  dwMergeVersion;
	STRUCT_RKTIME stReleaseTime;
	ENUM_RKDEVICE_TYPE emSupportChip;
	DWORD  dwBootOffset;
	DWORD  dwBootSize;
	DWORD  dwFWOffset;
	DWORD  dwFWSize;
	BYTE   reserved[IMAGE_RESERVED_SIZE];
}STRUCT_RKIMAGE_HEAD,*PSTRUCT_RKIMAGE_HEAD;
#define MAX_PACKAGE_FILES			16
#define RKIMAGE_TAG				0x46414B52
#define PARTNAME_BOOTLOADER		"bootloader"
#define PARTNAME_PARAMETER		"parameter"
#define PARTNAME_KERNEL			"kernel"
#define PARTNAME_BOOT			"boot"
#define PARTNAME_RECOVERY		"recovery"
#define PARTNAME_SYSTEM			"system"
#define PARTNAME_MISC			"misc"
#define PARTNAME_BACKUP			"backup"
#define PARTNAME_USERDATA		"userdata"
#define PARTNAME_USER			"user"

#define MAX_MANUFACTURER		60
#define MAX_MACHINE_INFO		30
#define MAX_MACHINE_MODEL		34
#define RELATIVE_PATH			60
#define PART_NAME				32
typedef struct 
{
	char name[PART_NAME];// 分区名称
	char file[RELATIVE_PATH];// 相对路径名，提取文件时用到
	unsigned int part_size;//分区占用扇区数
	unsigned int offset;// 文件在Image中的偏移
	unsigned int flash_offset;// 烧写到Flash中的位置(以sector为单位)
	unsigned int usespace;// 文件占用空间（按PAGE对齐)
	unsigned int size;// 字节数，实际文件大小
}STRUCT_RKIMAGE_ITEM,*PSTRUCT_RKIMAGE_ITEM;

typedef struct tagRKIMAGE_HDR
{
	unsigned int tag;
	unsigned int size;// 文件大小，不含末尾的CRC校验码
	char machine_model[MAX_MACHINE_MODEL];
	char machine_info[MAX_MACHINE_INFO];
	char manufacturer[MAX_MANUFACTURER];
	unsigned int dwFWVer;
	int item_count;
	STRUCT_RKIMAGE_ITEM item[MAX_PACKAGE_FILES];
}STRUCT_RKIMAGE_HDR,*PSTRUCT_RKIMAGE_HDR;
#pragma pack()
class CRKImage
{
public:
	GETPROP(DWORD,Version);
	GETPROP(DWORD,MergeVersion);
	GETPROP(STRUCT_RKTIME,ReleaseTime);
	GETPROP(ENUM_RKDEVICE_TYPE,SupportDevice);
	GETPROP(ENUM_OS_TYPE,OsType);
	GETPROP(ENUM_FS_TYPE,FsType);
	GETPROP(USHORT,BackupSize);
	GETPROP(DWORD,BootOffset);
	GETPROP(DWORD,BootSize);
	GETPROP(DWORD,FWOffset);
	GETPROP(ULONG64,FWSize);
	GETPROP(int,ParamPos);
	GETPROP(PBYTE,ParamBuffer);
	GETPROP(PSTRUCT_RKIMAGE_HDR,AndroidImageHead);
	GETPROP(BOOL,SignFlag);
	GETPROP(BOOL,SingleBootFlag);

	GET(DWORD,Version);
	GET(DWORD,MergeVersion);
	GET(STRUCT_RKTIME,ReleaseTime);
	GET(ENUM_RKDEVICE_TYPE,SupportDevice);
	GET(ENUM_OS_TYPE,OsType);
	GET(ENUM_FS_TYPE,FsType);
	GET(USHORT,BackupSize);
	GET(DWORD,BootOffset);
	GET(DWORD,BootSize);
	GET(DWORD,FWOffset);
	GET(ULONG64,FWSize);
	GET(int,ParamPos);
	GET(PBYTE,ParamBuffer);
	GET(PSTRUCT_RKIMAGE_HDR,AndroidImageHead);
	GET(BOOL,SignFlag);
	GET(BOOL,SingleBootFlag);
	CRKBoot *m_bootObject;
	
	BOOL SaveBootFile(CString filename);
	BOOL SaveFWFile(CString filename);
	BOOL SaveMd5Data(CString filename);
	BOOL GetData(ULONG64 dwOffset,DWORD dwSize,LPBYTE lpBuffer);
	VOID GetReservedData(LPBYTE &lpData,USHORT &usSize);
	int GetMd5Data(LPBYTE &lpMd5,LPBYTE &lpSignMd5);
	ULONG64 GetImageSize();
	CRKImage(CString filename,BOOL &bCheck);
	CRKImage(const CRKImage &srcImage,BOOL &bCheck);
	~CRKImage();
protected:

private:
	DWORD m_version;
	DWORD m_mergeVersion;
	STRUCT_RKTIME m_releaseTime;
	ENUM_RKDEVICE_TYPE m_supportDevice;
	DWORD m_bootOffset;
	DWORD m_bootSize;
	DWORD m_fwOffset;
	ULONG64 m_fwSize;
	ULONG64 m_fileSize;
	CCriticalSection m_mutex;
	BYTE  m_md5[32];
	BYTE  m_signMd5[256];
	BYTE  m_reserved[IMAGE_RESERVED_SIZE];
	CFile *m_pFile;
	PBYTE m_paramBuf;
	int	  m_paramPos;
	PSTRUCT_RKIMAGE_HDR m_androidImageHead;
	BOOL  m_bSignFlag;
	BOOL m_bSingleBoot;
	int   m_signMd5Size;
	BOOL  InitParamInfo();
	BOOL Md5Check(ULONG64 nCheckSize);
};
#endif