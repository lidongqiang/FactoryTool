#ifndef RKCOMM_HEADER
#define RKCOMM_HEADER
#include "DefineHeader.h"
#include "RKLog.h"
#define FILE_DEVICE_ROCKUSB		0x8000
#define FILE_DEVICE_INDEX		0x800
#define DOWNLOAD_471 \
	CTL_CODE(FILE_DEVICE_ROCKUSB, FILE_DEVICE_INDEX, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define DOWNLOAD_472 \
	CTL_CODE(FILE_DEVICE_ROCKUSB, FILE_DEVICE_INDEX+1, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define RESET_PIPE \
	CTL_CODE(FILE_DEVICE_ROCKUSB, FILE_DEVICE_INDEX+2, METHOD_BUFFERED, FILE_WRITE_ACCESS)
typedef enum{
		USB_BULK_READ=0,
		USB_BULK_WRITE,
		USB_CONTROL,
}USB_ACCESS_TYPE;
typedef enum
{
	TU_NONE_SUBCODE=0,
	TU_ERASESYSTEM_SUBCODE=0xFE,
	TU_LOWERFORMAT_SUBCODE=0xFD,
	TU_ERASEUSERDATA_SUBCODE=0xFB,
	TU_GETUSERSECTOR_SUBCODE=0xF9
}TESTUNIT_SUBCODE;
typedef enum
{
	RST_NONE_SUBCODE=0,
	RST_RESETMSC_SUBCODE,
	RST_POWEROFF_SUBCODE,
	RST_RESETMASKROM_SUBCODE,
	RST_DISCONNECTRESET_SUBCODE
}RESET_SUBCODE;
typedef enum
{
	RWMETHOD_IMAGE=0,
	RWMETHOD_LBA,
}RW_SUBCODE;
typedef enum
{
	EFUSE_KEYHASH=0,
	EFUSE_EXTEND_RW
}EFUSE_SUBCODE;
/* 命令 */
typedef enum
{
		TEST_UNIT_READY=0,
		READ_FLASH_ID=0x01,
		TEST_BAD_BLOCK=0x03,
		READ_SECTOR=0x04,
		WRITE_SECTOR=0x05,
		ERASE_NORMAL=0x06,
		ERASE_FORCE=0x0B,
		READ_LBA=0x14,
		WRITE_LBA=0x15,
		ERASE_SYSTEMDISK=0x16,
		READ_SDRAM=0x17,
		WRITE_SDRAM=0x18,
		EXECUTE_SDRAM=0x19,
		READ_FLASH_INFO=0x1A,
		READ_CHIP_INFO=0x1B,
		SET_RESET_FLAG=0x1E,
		WRITE_EFUSE=0x1F,
		READ_EFUSE = 0x20,
		READ_SPI_FLASH=0x21,
		WRITE_SPI_FLASH=0x22,
		WRITE_NEW_EFUSE=0x23,
		READ_NEW_EFUSE=0x24,
//		SWITCH_MASKROM=0xFE,
		DEVICE_RESET=0xFF
}USB_OPERATION_CODE;

#pragma pack(1)/* 按1字节对齐 */

typedef struct
{
	UCHAR	ucOperCode;		// 操作码
	UCHAR	ucReserved;
	DWORD	dwAddress;		// 地址（取决于操作码）
	UCHAR	ucReserved2;
	USHORT	usLength;		// 长度（取决于操作码）
	UCHAR	ucReserved3[7];
}CBWCB, *PCBWCB;

typedef struct
{
	DWORD	dwCBWSignature;			// 标志，固定为0x43425355("USBC")
	DWORD	dwCBWTag;				// 随机数，检测与返回值的匹配
	DWORD	dwCBWTransferLength;		// 为0
	UCHAR	ucCBWFlags;				// 数据传送方向： 0x80 - 设备到主机，0x00 － 主机到设备
	UCHAR	ucCBWLUN;				// Flash片选
	UCHAR	ucCBWCBLength;			// 命令块长度：6/10/16
	CBWCB	cbwcb;					// 命令块
}CBW, *PCBW;

typedef struct 
{
	DWORD	dwCSWSignature;
	DWORD	dwCSWTag;
	DWORD	dwCBWDataResidue;
	UCHAR	ucCSWStatus;
}CSW, *PCSW;

#pragma pack()
/* 命令标志 */
#define CBW_SIGN			0x43425355	/* "USBC" */
#define CSW_SIGN			0x53425355	/* "USBS" */

/* 传输方向 */
#define DIRECTION_OUT		0x00	// 主机 => 设备
#define DIRECTION_IN		0x80	// 设备 => 主机
#define MAX_TEST_BLOCKS		512		// 一次可测试的最大块数，1bit代表1个块，需64个Bytes
#define MAX_ERASE_BLOCKS	128
#define  MAX_CLEAR_LEN	16*1024

/* 函数返回的错误码 */
#define ERR_SUCCESS				0
#define ERR_DEVICE_READY		ERR_SUCCESS
#define ERR_DEVICE_OPEN_FAILED	-1
#define ERR_CSW_OPEN_FAILED		-2
#define ERR_DEVICE_WRITE_FAILED	-3
#define ERR_DEVICE_READ_FAILED	-4
#define ERR_CMD_NOTMATCH		-5
#define ERR_DEVICE_UNREADY		-6
#define ERR_FOUND_BAD_BLOCK		-7
#define ERR_FAILED				-8
#define ERR_CROSS_BORDER		-9

#define UFI_CHECK_SIGN(cbw, csw) ((CSW_SIGN == (csw).dwCSWSignature) && ((csw).dwCSWTag == (cbw).dwCBWTag))
class CRKComm
{
public:
	static BOOL RKU_ResetPipe(LPCTSTR lpDevPath,UCHAR ucResetValue,CRKLog *pLog=NULL);
	virtual int RKU_EraseBlock(LPCTSTR lpDevPath,UCHAR ucFlashCS,DWORD dwPos,DWORD dwCount,UCHAR ucEraseType)=0;
	virtual int RKU_ReadChipInfo(LPCTSTR lpDevPath,LPBYTE lpBuffer)=0;	
	virtual int RKU_ReadFlashID(LPCTSTR lpDevPath,UCHAR ucFlashCS,LPBYTE lpBuffer)=0;
	virtual int RKU_ReadFlashInfo(LPCTSTR lpDevPath,LPBYTE lpBuffer,UINT *puiRead=NULL)=0;
	virtual int RKU_ReadLBA(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer,BYTE bySubCode=RWMETHOD_IMAGE)=0;				
	virtual int RKU_ReadSector(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer)=0;
	virtual int RKU_ResetDevice(LPCTSTR lpDevPath,BYTE bySubCode=RST_NONE_SUBCODE)=0;
//	virtual int RKU_SwitchToMaskRom(LPCTSTR lpDevPath)=0;
	virtual int RKU_SetResetFlag(LPCTSTR lpDevPath)=0;
	virtual int RKU_EraseSystemDisk(LPCTSTR lpDevPath)=0;
	virtual int RKU_TestBadBlock(LPCTSTR lpDevPath,UCHAR ucFlashCS,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer)=0;
	virtual int RKU_TestDeviceReady(LPCTSTR lpDevPath,DWORD *dwTotal=NULL,DWORD *dwCurrent=NULL,BYTE bySubCode=TU_NONE_SUBCODE)=0;
	virtual int RKU_WriteLBA(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer,BYTE bySubCode=RWMETHOD_IMAGE)=0;
	virtual int RKU_WriteSector(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer)=0;
	virtual int RKU_ReadEfuse(LPCTSTR lpDevPath,LPBYTE lpBuffer,USHORT dwReadSize)=0;
	virtual int RKU_WriteEfuse(LPCTSTR lpDevPath,LPBYTE lpBuffer,USHORT dwWriteSize)=0;
	virtual int RKU_ReadSpiFlash(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer)=0;
	virtual int RKU_WriteSpiFlash(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer)=0;
	virtual int RKU_ReadNewEfuse(LPCTSTR lpDevPath,USHORT usPos,USHORT usSize,LPBYTE lpBuffer,BYTE bySubCode=EFUSE_KEYHASH)=0;
	virtual int RKU_WriteNewEfuse(LPCTSTR lpDevPath,USHORT usPos,USHORT usSize,LPBYTE lpBuffer,BYTE bySubCode=EFUSE_KEYHASH)=0;
	static int RandomInteger(int low, int high);
	static DWORD MakeCBWTag();
protected:
	CRKLog *m_pLog;
	CRKComm(CRKLog *pLog=NULL);
private:
	virtual HANDLE RKU_Open(LPCTSTR lpDevPath,USB_ACCESS_TYPE accessMode)=0;
	virtual BOOL RKU_Close(HANDLE hDev)=0;
	virtual BOOL RKU_Write(HANDLE hDev,LPBYTE lpBuffer,DWORD dwSize)=0;
	virtual BOOL RKU_Read(HANDLE hDev, LPBYTE lpBuffer, DWORD dwSize)=0;
};
class CRKUsbComm: public CRKComm
{
public:
	virtual	int RKU_EraseBlock(LPCTSTR lpDevPath,UCHAR ucFlashCS,DWORD dwPos,DWORD dwCount,UCHAR ucEraseType);
	virtual int RKU_ReadChipInfo(LPCTSTR lpDevPath,LPBYTE lpBuffer);
	virtual int RKU_ReadFlashID(LPCTSTR lpDevPath,UCHAR ucFlashCS,LPBYTE lpBuffer);
	virtual int RKU_ReadFlashInfo(LPCTSTR lpDevPath,LPBYTE lpBuffer,UINT *puiRead=NULL);
	virtual int RKU_ReadLBA(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer,BYTE bySubCode=RWMETHOD_IMAGE);
	virtual int RKU_ReadSector(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer);
	virtual int RKU_ResetDevice(LPCTSTR lpDevPath,BYTE bySubCode=RST_NONE_SUBCODE);
//	virtual int RKU_SwitchToMaskRom(LPCTSTR lpDevPath);
	virtual int RKU_SetResetFlag(LPCTSTR lpDevPath);
	virtual int RKU_EraseSystemDisk(LPCTSTR lpDevPath);
	virtual int RKU_TestBadBlock(LPCTSTR lpDevPath,UCHAR ucFlashCS,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer);
	virtual int RKU_TestDeviceReady(LPCTSTR lpDevPath,DWORD *dwTotal=NULL,DWORD *dwCurrent=NULL,BYTE bySubCode=TU_NONE_SUBCODE);
	virtual int RKU_WriteLBA(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer,BYTE bySubCode=RWMETHOD_IMAGE);
	virtual int RKU_WriteSector(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer);
	virtual int RKU_ReadEfuse(LPCTSTR lpDevPath,LPBYTE lpBuffer,USHORT usReadSize);
	virtual int RKU_WriteEfuse(LPCTSTR lpDevPath,LPBYTE lpBuffer,USHORT usWriteSize);
	virtual int RKU_ReadSpiFlash(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer);
	virtual int RKU_WriteSpiFlash(LPCTSTR lpDevPath,DWORD dwPos,DWORD dwCount,LPBYTE lpBuffer);
	virtual int RKU_ReadNewEfuse(LPCTSTR lpDevPath,USHORT usPos,USHORT usSize,LPBYTE lpBuffer,BYTE bySubCode=EFUSE_KEYHASH);
	virtual int RKU_WriteNewEfuse(LPCTSTR lpDevPath,USHORT usPos,USHORT usSize,LPBYTE lpBuffer,BYTE bySubCode=EFUSE_KEYHASH);
	CRKUsbComm(CRKLog *pLog=NULL);
	
protected:
	
private:
	virtual HANDLE RKU_Open(LPCTSTR lpDevPath,USB_ACCESS_TYPE accessMode);
	virtual BOOL RKU_Close(HANDLE hDev);
	virtual BOOL RKU_Write(HANDLE hDev,LPBYTE lpBuffer,DWORD dwSize);
	virtual BOOL RKU_Read(HANDLE hDev, LPBYTE lpBuffer, DWORD dwSize);
	int RKU_ClearBuffer(LPCTSTR lpDevPath);
	void InitializeCBW(PCBW pCBW, USB_OPERATION_CODE code);
	/*int RandomInteger(int low, int high);
	DWORD MakeCBWTag();*/
};

#endif