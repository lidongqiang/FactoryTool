#include "stdafx.h"
#include "MySpawnConsumer.h"

CMySpawnConsumer::CMySpawnConsumer() 
{
	m_pSpawn = NULL;
}

CString CMySpawnConsumer::RebootLoaderCmd(CString strExe, CString strSerial)
{
	CString strCmd;
	if (strSerial.IsEmpty())
	{
		strCmd = _T("\"") + strExe + _T("\" reboot bootloader") ;
	}
	else
	{
		strCmd = _T("\"") + strExe + _T("\" -s ") + 
			 strSerial + _T(" reboot bootloader") ;
	}
	

	TRACE(_T("RebootLoaderCmd:%s\r\n"),strCmd);
	return strCmd;
}
CString CMySpawnConsumer::FindAdbProcess()
{
	CString strCmd;
	strCmd = _T("netstat -ano") ;		
	TRACE(_T("FindAdbProcess:%s\r\n"),strCmd);
	return strCmd;
}


BOOL CMySpawnConsumer::GetSpawnData(PBYTE &pData)
{
	if (m_pSpawn==NULL)
	{
		return FALSE;
	}
	BOOL bRet=FALSE;
	DWORD dwSize;
	pData = NULL;
	bRet = m_pSpawn->GetOutputData(NULL,dwSize);
	if (bRet)
	{
		if (dwSize>0)
		{
			pData = new BYTE[dwSize];
			if (pData)
			{
				bRet = m_pSpawn->GetOutputData(pData,dwSize);
			}
		}
		else
			bRet = FALSE;
	}
	return bRet;
}
BOOL CMySpawnConsumer::SpawnDataToString(tstring &strDst,PBYTE pSrc,ENUM_STRING_CODE emCode)
{
	if (!pSrc)
	{
		return FALSE;
	}
	BOOL bRet=FALSE;
	LPTSTR lpDst;
	int nSize;
	if (emCode==SC_UNICODE)
	{
		strDst = (PWCHAR)pSrc;
		bRet = TRUE;
	}
	else if (emCode==SC_ANSI)
	{
		bRet =  cmStrCode::AnsiToUnicode(lpDst,nSize,(PCHAR)pSrc);
		if (bRet)
		{
			strDst = lpDst;
			delete []lpDst;
		}
	}
	else if (emCode==SC_UTF8)
	{
		bRet =  cmStrCode::Utf8ToUnicode(lpDst,nSize,(PCHAR)pSrc);
		if (bRet)
		{
			strDst = lpDst;
			delete []lpDst;
		}
	}
	return bRet;
}
VOID CMySpawnConsumer::ReleaseSpawnData(PBYTE pData)
{
	if (pData)
	{
		delete []pData;
	}
}

VOID CMySpawnConsumer::SetSpawnObject(cmSpawn *pSpawn)
{
	if (pSpawn)
	{
		m_pSpawn = pSpawn;
	}
}
BOOL CMySpawnConsumer::GetSpawnString(tstring &strSpawnOutput,ENUM_STRING_CODE emCode)
{
	BOOL bRet;
	PBYTE pSpawnData;
	bRet = GetSpawnData(pSpawnData);
	if (bRet)
	{
		bRet = SpawnDataToString(strSpawnOutput,pSpawnData,emCode);
		ReleaseSpawnData(pSpawnData);
	}
	return bRet;
}