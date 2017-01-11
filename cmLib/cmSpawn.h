#ifndef CM_SPAWN_H
#define CM_SPAWN_H

namespace cm
{
	#define BUFSIZE 512
	typedef struct  
	{
		PBYTE pData;
		DWORD dwDataSize;
	}STRUCT_PIPE_DATA,*PSTRUCT_PIPE_DATA;
	typedef vector<STRUCT_PIPE_DATA> PIPE_DATA_VECTOR;
	class cmSpawn  
	{
	public:
		cmSpawn();
		cmSpawn(CString& exe);

		virtual ~cmSpawn();
 
		bool Execute();
		bool Execute(CString& exe);

		bool TestProcess();
		BOOL TestThread();
		BOOL GetOutputData(PBYTE pBuffer,DWORD &bufSize);
	protected:
		bool CreateChildProcess(); 
		void WriteToPipe(LPCTSTR line); 
		void ReadFromPipe(); 
		void ErrMsg(LPTSTR, bool); 
		void TestAndCleanProcess();
		void Redirect();
 
	private:
		void	Init();
		static UINT ReadPipeThreadProc(LPVOID pParam);
		void Consume(BYTE*	pBuffer, DWORD dwBufLen);
		void ConsumeStart();
		void ConsumeEnd();

		PROCESS_INFORMATION m_piProcInfo; 
		STARTUPINFO			m_siStartInfo; 
		
		HANDLE	m_hChildStdinRd, 
				m_hChildStdinWr, 
				m_hChildStdoutRd, 
				m_hChildStdoutWr, 
				m_hSaveStdin, 
				m_hSaveStdout; 

		bool	m_bRedirected;
		DWORD m_dwDataSize;
		CString	m_szExecutable;
		CWinThread*	m_pReadThread;
		DWORD	m_dwProcessId;
		PIPE_DATA_VECTOR m_vecPipeData;
	};
}
#endif 
