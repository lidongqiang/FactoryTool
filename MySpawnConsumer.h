#ifndef MYSPAWNCONSUMER_HEADER
#define MYSPAWNCONSUMER_HEADER

#include "cmSpawn.h"
#include "cmStrCode.h"
using namespace cm;
enum ENUM_STRING_CODE{SC_ANSI=0,SC_UNICODE,SC_UTF8};
#if defined(_UNICODE) || defined(UNICODE)
typedef wstring	tstring;
typedef wstringstream tstringstream;
#else
typedef string	tstring;
typedef stringstream tstringstream;
#endif

class CMySpawnConsumer
{
public:
	CMySpawnConsumer();
	static CString FindAdbProcess();
	static CString RebootLoaderCmd(CString strExe, CString strSerial);
	VOID SetSpawnObject(cmSpawn *pSpawn);
	BOOL GetSpawnString(tstring &strSpawnOutput,ENUM_STRING_CODE emCode=SC_ANSI);
private:
	 BOOL GetSpawnData(PBYTE &pData);
	 BOOL SpawnDataToString(tstring &strDst,PBYTE pSrc,ENUM_STRING_CODE emCode);
	 VOID ReleaseSpawnData(PBYTE pData);
	
private:
	cmSpawn *m_pSpawn;
};
#endif