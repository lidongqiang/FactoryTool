#ifndef CM_ALGORITHM_H
#define CM_ALGORITHM_H
namespace cm
{
	class cmAlgorithm
	{
	public:
		static BOOL DoHash256(LPBYTE pOut,LPBYTE pIn,ULONGLONG nInBufferSize);
		static BOOL DoHash160(LPBYTE pOut,LPBYTE pIn,ULONGLONG nInBufferSize);
		static BOOL DoMd5(LPBYTE pOut,LPBYTE pIn,ULONGLONG nInBufferSize);
		static USHORT DoCrc16(LPBYTE pData,UINT uiDataSize);
		static USHORT DoCCITT(LPBYTE pData,UINT uiDataSize);
		static UINT DoCrc32(LPBYTE pData,UINT uiDataSize);
		static void DoRC4(LPBYTE pData,UINT uiDataSize);
		static void DoAesCBCEncrypt(LPBYTE pData,UINT uiDataSize,int keyLength=0,PBYTE pKey=NULL,PBYTE pVec=NULL);
		static void DoAesCBCDecrypt(LPBYTE pData,UINT uiDataSize,int keyLength=0,PBYTE pKey=NULL,PBYTE pVec=NULL);
		static BOOL DoFileCrc32(UINT &uiCrc,CString strFile,ULONGLONG nComputeSize=-1);
		static BOOL DoFileHash256(LPBYTE pOut,CString strFile,ULONGLONG nComputeSize=-1);
		static BOOL DoFileHash160(LPBYTE pOut,CString strFile,ULONGLONG nComputeSize=-1);
		static BOOL DoFileMd5(LPBYTE pOut,CString strFile,ULONGLONG nComputeSize=-1);
		static BOOL DoRsa(BYTE *pOut,UINT *pOutSize,BYTE * pIn,UINT nInSize,BYTE *pKey,UINT nKeySize);
		static USHORT SwapU16(USHORT usValue);
		static UINT SwapU32(UINT uiValue);
	};
}
#endif