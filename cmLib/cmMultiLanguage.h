#ifndef MULTILANG_HEADER
#define MULTILANG_HEADER
#include "cmIniFile.h"
#include "cmLog.h"
namespace cm
{
	typedef struct  
	{
		CString strFileName;
		CString strFontName;
		UINT     uiFontSize;
	}STRUCT_LANG_PROPERTY;
	typedef vector<STRUCT_LANG_PROPERTY> LANG_PROP_VECTOR;

	typedef struct
	{
		HWND hwnd;
		int  id;
	}STRUCT_DIALOG_PROPERTY;
	typedef vector<STRUCT_DIALOG_PROPERTY> DIALOG_PROP_VECTOR;

	typedef struct
	{
		HMENU hwnd;
		int  id;
	}STRUCT_MENU_PROPERTY;
	typedef vector<STRUCT_MENU_PROPERTY> MENU_PROP_VECTOR;

	typedef struct  
	{
		cm::cmIniSection *pSection;
		HFONT  hFont;
	}STRUCT_SETSTRING_PARAM,*PSTRUCT_SETSTRING_PARAM;
	class cmMultiLanguage
	{
	public:
		GETPROP(CHAR,LangKinds);
		GET(CHAR,LangKinds);
		PROP(CHAR,CurrentLang);
		GET(CHAR,CurrentLang);
		SET(CHAR,CurrentLang);
		cmMultiLanguage(CONST CString &langFilePath,CONST LANG_PROP_VECTOR &langProperty,cm::cmLog *pLog,BOOL &bSuccess);
		~cmMultiLanguage();
		BOOL GetString(CString strSection,CString strKey,CString &strValue);
		BOOL SaveLanguageString();
		BOOL SetLanguageString();
		BOOL AddDialogProp(STRUCT_DIALOG_PROPERTY prop);
		BOOL AddMenuProp(STRUCT_MENU_PROPERTY prop);
	private:
		CCriticalSection m_csMutex;
		CHAR m_langKinds;
		CHAR m_curLang;
		CString m_strLangFilePath;
		LANG_PROP_VECTOR m_vecLangProperty;
		cm::cmIniFile *m_pIni;
		cm::cmLog *m_pLog;
		DIALOG_PROP_VECTOR m_vecDialogProperty;
		MENU_PROP_VECTOR m_vecMenuProperty;
		HFONT m_fontHwnd;
	};
}
VOID SetListViewString(cm::cmIniSection *pSection,CWnd *pWnd,CString strMainKeyPart);
VOID SetTreeViewString(cm::cmIniSection *pSection,CWnd *pWnd,CString strMainKeyPart);
VOID SetTabCtrlString(cm::cmIniSection *pSection,CWnd *pWnd,CString strMainKeyPart);
VOID TreeViewVisitForSet(cm::cmIniSection *pSection,CTreeCtrl *pTreeView,HTREEITEM hVisitItem,CString strMainKeyPart,int index);
VOID SaveListViewString(cm::cmIniSection *pSection,CWnd *pWnd,CString strMainKeyPart);
VOID SaveTreeViewString(cm::cmIniSection *pSection,CWnd *pWnd,CString strMainKeyPart);
VOID SaveTabCtrlString(cm::cmIniSection *pSection,CWnd *pWnd,CString strMainKeyPart);
VOID TreeViewVisit(cm::cmIniSection *pSection,CTreeCtrl *pTreeView,HTREEITEM hVisitItem,CString strMainKeyPart,int index);
VOID MenuVist(cm::cmIniSection *pSection,CMenu *pMenu,CString strMainKeyPart);
VOID MenuVistForSet(cm::cmIniSection *pSection,CMenu *pMenu,CString strMainKeyPart);
#endif