#ifndef CM_LOG_H
#define CM_LOG_H

#ifndef _UNICODE
#define MAKE_FMT_STRING(fmt, buf) \
	{ \
	va_list args; \
	va_start(args, fmt); \
	_vsnprintf(buf, sizeof(buf)-1, fmt, args); \
	va_end(args); \
	buf[sizeof(buf)-1] = 0x00; \
		};
#else
#if _MSC_VER>1200
#define MAKE_FMT_STRING(fmt, buf) \
	{ \
	va_list args; \
	va_start(args, fmt); \
	_vsnwprintf_s(buf, sizeof(buf)/2-1, fmt, args); \
	va_end(args); \
	buf[sizeof(buf)/2-1] = 0x0000; \
		};
#else
#define MAKE_FMT_STRING(fmt, buf) \
	{ \
	va_list args; \
	va_start(args, fmt); \
	_vsnwprintf(buf, sizeof(buf)/2-1, fmt, args); \
	va_end(args); \
	buf[sizeof(buf)/2-1] = 0x0000; \
		};
#endif
#endif

namespace cm
{
	class cmLog
	{
	public:
		GETPROP(CString,LogSavePath);
		SETPROP(CString,LogFileName);
		PROP(BOOL,EnableLog);
		GET(CString,LogSavePath);
		GET(BOOL,EnableLog);
		SET(BOOL,EnableLog);
		SET(CString,LogFileName);
		cmLog(CString logFilePath,BOOL &bSuccess);
		~cmLog();
		BOOL SaveBuffer(CString fileName,LPBYTE lpBuffer,DWORD dwSize,BOOL bCreate=TRUE);
		static VOID BufferToHexString(CString &strOutput,LPBYTE lpBuffer,DWORD dwSize,UINT uiLineCount=16);
		VOID Record(LPCTSTR lpFmt,...);
		BOOL Write(CString text);
	protected:
	private:
		CString m_path;
		CString m_name;
		BOOL    m_enable;
#ifdef _UNICODE
		BOOL    m_writeUnicodeHeadDone;
#endif
		CCriticalSection *m_csMutexLock;
		
		
	};
}
#endif