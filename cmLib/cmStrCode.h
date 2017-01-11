#ifndef CM_STRCODE_H
#define CM_STRCODE_H

namespace cm
{
	class cmStrCode
	{
	public:
		static BOOL UnicodeToAnsi(LPSTR &lpszDst,int &nDstSize,LPCWSTR lpwzSrc);
		static BOOL AnsiToUnicode(LPWSTR &lpwzDst,int &nDstSize,LPCSTR lpszSrc);
		static BOOL UnicodeToUtf8(LPSTR &lpszDst,int &nDstSize,LPCWSTR lpwzSrc);
		static BOOL Utf8ToUnicode(LPWSTR &lpwzDst,int &nDstSize,LPCSTR lpszSrc);
		static BOOL Utf8ToAnsi(LPSTR &lpszDst,int &nDstSize,LPCSTR lpszSrc);
		static BOOL AnsiToUtf8(LPSTR &lpszDst,int &nDstSize,LPCSTR lpszSrc);
	};
}

#endif