#ifndef CM_BASE_H
#define CM_BASE_H
#pragma warning(disable: 4786)
#define PROP(T,X) __declspec(property(get=Get##X,put=Set##X))T X;//读写属性
#define GETPROP(T,X) __declspec(property(get=Get##X))T X; //只读属性
#define SETPROP(T,X) __declspec(property(put=Set##X))T X; //只写属性
#define GET(T,X) T Get##X() 
#define SET(T,X) VOID Set##X(T value)
#include <vector>
#include <list>
#include <map>
#include <string>
#include <wchar.h>
using namespace std;
typedef vector<CString> CmStringVec;
typedef vector<int> CmIntVec;
#include <wininet.h>
#pragma comment(lib,"wininet.lib")
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <winioctl.h>
#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")

#endif