#ifndef CM_PATH_H
#define CM_PATH_H

namespace cm
{
	class cmPath
	{
	public:
		static CString FixPath(CString strPath,BOOL bDir=TRUE);
		static VOID SplitPath(CString strPath,CString &strDrive,CString &strDir,CString &strFile,CString &strFileExt);
		static CString GetFileName(CString strPath);
		static CString GetFileTitle(CString strPath);
		static CString GetFileExt(CString strPath);
		static CString GetFileDir(CString strPath);
		static CString GetModulePath();
		static CString GetSystemTempPath();
		static CString GetCurrentDir();
		static BOOL IsRelativePath(CString strPath);
	};
}

#endif