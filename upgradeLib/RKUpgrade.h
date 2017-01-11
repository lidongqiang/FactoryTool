#ifndef RKUPGRADE_HEADER
#define RKUPGRADE_HEADER
#include "DefineHeader.h"
#include "RKDevice.h"
#include "RK28Device.h"
#include "RKNanoDevice.h"
#include "RKAndroidDevice.h"
#include "RKScan.h"

typedef enum
{
	DOWNLOADBOOT_STEP=1,
	DOWNLOADIDBLOCK_STEP=4,
	DOWNLOADIMAGE_STEP=7,
	TESTDEVICE_STEP=10,
	RESETDEVICE_STEP=13,
	FORMATDISK_STEP=16,
	COPYDATA_STEP=19,
	WAITMSC_STEP=22,
	WAITLOADER_STEP=25,
	WAITMASKROM_STEP=28,
	ERASEIDB_STEP=31,
	SWITCHMSC_STEP=34,
	CHECKCHIP_STEP=37,
	PREPAREIDB_STEP=40,
	MUTEXRESETDEVICE_STEP=43,
	GETOLDDISKSIZE_STEP=46,
	READSN_STEP=49,
	WRITESN_STEP=52,
	ERASEALLBLOCKS_STEP=55,
	GETBLOCKSTATE_STEP=58,
	GETFLASHINFO_STEP=61,
	WRITEBACK_STEP=64,
	FINDUSERDISK_STEP=67,
	SHOWUSERDISK_STEP=70,
	READMAC_STEP=73,
	WRITEMAC_STEP=76,
	READBT_STEP=79,
	WRITEBT_STEP=82,
	LOWERFORMAT_STEP=85,
	READIMEI_STEP=88,
	WRITEIMEI_STEP=91,
	SHOWDATADISK_STEP=94,
	FINDDATADISK_STEP=97,
	FORMATDATADISK_STEP=100,
	COPYDATADISK_STEP=103,
	READUID_STEP=106,
	READCUSTOMDATA_STEP=109,
	WRITECUSTOMDATA_STEP=112,
	SETRESETFLAG_STEP=115,
	POWEROFF_STEP=118,
	READALLINFO_STEP=121,
	WRITEALLINFO_STEP=124,
	RESETMSC_STEP=127,
	READWIFI_STEP=130,
	WRITEWIFI_STEP=133
}ENUM_UPGRADE_STEP;
typedef struct  
{
	ENUM_UPGRADE_STEP emStep;
	UINT    uiSupport;//表明loader或者maskrom设备是否需要执行此步骤
	PVOID	pMutexCS;
	BOOL    bMutex;
	BOOL    bSync;
	BOOL    bJump;
	BYTE    bJumpParam;
	BOOL    bWriteBack;
	UINT	uiParam;
}STRUCT_UPGRADE_STEP,*PSTRUCT_UPGRADE_STEP;
typedef vector<STRUCT_UPGRADE_STEP>  RK_UPGRADE_SET;
class CDeviceFactory
{
public:
	static CRKDevice* CreateDevice(STRUCT_RKDEVICE_DESC &device,ENUM_RKDEVICE_TYPE devType,ENUM_OS_TYPE osType);
	static VOID FreeDevice(CRKDevice *pDevice,ENUM_OS_TYPE osType);
};
class CRKUpgrade
{
public:
	
	PROP(CRKDevice*,DeviceObject);
	SETPROP(PVOID,ScanObject);
	SETPROP(PVOID,SyncLockObject);
	SETPROP(PVOID,SyncCSObject);
	SETPROP(PUCHAR,SyncCount);
	SETPROP(PUCHAR,SyncCountCopy);
	SETPROP(BOOL,DBMutexFlag);
	SETPROP(BOOL,SyncFlag);
	SETPROP(UpgradeStepPromptCB,CallBackPointer);
	SET(UpgradeStepPromptCB,CallBackPointer);
	GET(CRKDevice*,DeviceObject);
	SET(CRKDevice*,DeviceObject);
	SET(PVOID,ScanObject);
	SET(PVOID,SyncLockObject);
	SET(PVOID,SyncCSObject);
	SET(PUCHAR,SyncCount);
	SET(PUCHAR,SyncCountCopy);
	SET(BOOL,DBMutexFlag);
	SET(BOOL,SyncFlag);
	CRKUpgrade(CONST RK_UPGRADE_SET &upgradeStepSet,ENUM_WORKFLOW workFlow);
	~CRKUpgrade();
	BOOL InitializeStep(CONST RK_UPGRADE_SET &upgradeStepSet,ENUM_WORKFLOW workFlow);
	BOOL SetObject(CRKDevice *pDevice,CRKScan *pScan,CSingleLock *pSingleLock,CCriticalSection *pCS,PUCHAR pCount,PUCHAR pCountCopy);
	static BOOL CreateUpgradeStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet,UINT uiMscOper=MSC_NONE_OPER);
	static BOOL CreateRestoreStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet,UINT uiMscOper=MSC_NONE_OPER);
	static BOOL CreateDownloadIDBStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateDownloadImageStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateGetDiskSizeStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateReadSnStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateWriteSnStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateReadMacStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateWriteMacStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateReadWifiStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateWriteWifiStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateReadImeiStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateWriteImeiStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateReadBtStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateWriteBtStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateEraseIDBStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateEraseAllBlocksStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateGetBlockStateStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateReadUidStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateReadCustomDataStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateWriteCustomDataStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateReadAllInfoStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	static BOOL CreateWriteAllInfoStep(ENUM_RKDEVICE_TYPE type,RK_UPGRADE_SET& upgradeStepSet);
	BOOL SyncUpgradeThread(BOOL bGoOn);
	BOOL SetStep(ENUM_UPGRADE_STEP emStep,UINT uiSupport);
	BOOL  Run();
protected:
private:
	CRKDevice *m_pDevice;
	CRKScan   *m_pScan;
	CSingleLock *m_pSyncSingleLock;
	CCriticalSection *m_pSyncCS;
	PUCHAR m_pSyncCount;
	PUCHAR m_pSyncCountCopy;
	RK_UPGRADE_SET m_UpgradeProcedure;
	ENUM_WORKFLOW m_workFlow;
	BOOL m_bDBMutexFlag;
	BOOL m_bSyncFlag;
	HANDLE m_hDBMutex;
	HANDLE m_hResetMutex;
	UpgradeStepPromptCB m_callBackProc;
	BOOL FindUserDisk();
};
#define MAX_DRIVE_NUM	5
#endif