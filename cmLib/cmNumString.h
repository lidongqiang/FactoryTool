#ifndef CM_NUMSTRING_H
#define CM_NUMSTRING_H

namespace cm
{
	class cmNumString
	{
	public:
		static CString NumToStr(int value,int radix);
		static CString NumToStr(INT64 value,int radix);
		static CString NumToStr(long value,int radix);
		static CString NumToStr(ULONG value,int radix);
		static CString NumToStr(double value,unsigned int precision=1);

		static long StrToSLong(CString value,int radix);
		static ULONG StrToULong(CString value,int radix);
		static int StrToInt32(CString value);
		static INT64 StrToInt64(CString value);
		static double StrToDouble(CString value);

		static UINT SwapEndian(UINT value);
		static USHORT SwapEndian(USHORT value);

		static CString HexStrIncrease(CString strValue);
		static CString IntStrIncrease(CString strValue);
		static BOOL IsHexString(CString strValue);
		static BOOL IsNumberString(CString strValue);
		static BOOL IsFloatString(CString strValue);
		
		static CString IntervalInsert(CString strValue,int nSkip,CString strDelimiter);
		static VOID Split(CmStringVec &vecSubPart,CString strValue,int nSkip );
		static VOID Split(CmStringVec &vecSubPart,CString strValue,CString strDelimiter);
		static CString Merge(CmStringVec &vecSubPart);
		static CString Merge(CmStringVec &vecSubPart,CString strDelimiter);
	};
}


#endif