#ifndef RKLOG_HEADER
#define RKLOG_HEADER
#include "DefineHeader.h"
#ifndef GET_FMT_STRING
	#ifndef _UNICODE
	#define GET_FMT_STRING(fmt, buf) \
		{ \
		va_list args; \
		va_start(args, fmt); \
		_vsnprintf(buf, sizeof(buf)-1, fmt, args); \
		va_end(args); \
		buf[sizeof(buf)-1] = 0x00; \
			};
	#else
	#define GET_FMT_STRING(fmt, buf) \
		{ \
		va_list args; \
		va_start(args, fmt); \
		_vsnwprintf(buf, sizeof(buf)/2-1, fmt, args); \
		va_end(args); \
		buf[sizeof(buf)/2-1] = 0x0000; \
			};
	#endif
#endif



class CRKLog
{
public:
	GETPROP(CString,LogSavePath);
	PROP(BOOL,EnableLog);
	GET(CString,LogSavePath);
	GET(BOOL,EnableLog);
	SET(BOOL,EnableLog);
	CRKLog(CString logFilePath,CString logFileName,BOOL enable=TRUE);
	~CRKLog();
	BOOL SaveBuffer(CString fileName,LPBYTE lpBuffer,DWORD dwSize);
	VOID PrintBuffer(CString &strOutput,LPBYTE lpBuffer,DWORD dwSize,UINT uiLineCount=16);
	VOID Record(LPCTSTR lpFmt,...);
	
protected:
private:
	CString m_path;
	CString m_name;
	BOOL    m_enable;
	BOOL    m_writeUnicodeHeadDone;
	CCriticalSection *m_csMutexLock;
	
	BOOL Write(CString text);
};
#endif