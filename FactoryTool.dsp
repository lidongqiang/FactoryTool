# Microsoft Developer Studio Project File - Name="FactoryTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FactoryTool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FactoryTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FactoryTool.mak" CFG="FactoryTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FactoryTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FactoryTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FactoryTool - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "Grid" /I "ButtonST" /I "XGroupBox" /I "FontStatic" /I "ColumnTreeCtrl" /I "cmLib" /I "upgradeLib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"bin/FactoryTool.exe" /libpath:"cmLib" /libpath:"upgradeLib"

!ELSEIF  "$(CFG)" == "FactoryTool - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "Grid" /I "ButtonST" /I "XGroupBox" /I "FontStatic" /I "ColumnTreeCtrl" /I "cmLib" /I "upgradeLib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"bin/FactoryTool_Debug.exe" /pdbtype:sept /libpath:"cmLib" /libpath:"upgradeLib"

!ENDIF 

# Begin Target

# Name "FactoryTool - Win32 Release"
# Name "FactoryTool - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ButtonST\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\ColumnTreeCtrl\ColumnTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FactoryTool.cpp
# End Source File
# Begin Source File

SOURCE=.\FactoryTool.rc
# End Source File
# Begin Source File

SOURCE=.\FactoryToolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FontStatic\FontStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\Grid\GridButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Grid\GridEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Grid\GridListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Grid\TitleTip.cpp
# End Source File
# Begin Source File

SOURCE=.\Grid\VirtualGridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\XGroupBox\XGroupBox.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ButtonST\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\ColumnTreeCtrl\ColumnTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FactoryTool.h
# End Source File
# Begin Source File

SOURCE=.\FactoryToolDlg.h
# End Source File
# Begin Source File

SOURCE=.\FontStatic\FontStatic.h
# End Source File
# Begin Source File

SOURCE=.\Grid\GridButton.h
# End Source File
# Begin Source File

SOURCE=.\Grid\GridEdit.h
# End Source File
# Begin Source File

SOURCE=.\Grid\GridListBox.h
# End Source File
# Begin Source File

SOURCE=.\Grid\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Grid\TitleTip.h
# End Source File
# Begin Source File

SOURCE=.\Grid\VirtualGridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\XGroupBox\XGroupBox.h
# End Source File
# Begin Source File

SOURCE=.\XGroupBox\XVisualStyles.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Demo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\demo.ico
# End Source File
# Begin Source File

SOURCE=.\res\exit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\exit.ico
# End Source File
# Begin Source File

SOURCE=.\res\FactoryTool.ico
# End Source File
# Begin Source File

SOURCE=.\res\FactoryTool.rc2
# End Source File
# Begin Source File

SOURCE=.\res\firmware.bmp
# End Source File
# Begin Source File

SOURCE=.\res\firmware.ico
# End Source File
# Begin Source File

SOURCE=.\res\green_led.bmp
# End Source File
# Begin Source File

SOURCE=.\res\host.ico
# End Source File
# Begin Source File

SOURCE=.\res\hub.ico
# End Source File
# Begin Source File

SOURCE=.\res\language.bmp
# End Source File
# Begin Source File

SOURCE=.\res\language.ico
# End Source File
# Begin Source File

SOURCE=.\res\logo.ico
# End Source File
# Begin Source File

SOURCE=.\res\monitor.ico
# End Source File
# Begin Source File

SOURCE=.\res\pause.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pause.ico
# End Source File
# Begin Source File

SOURCE=.\res\port.ico
# End Source File
# Begin Source File

SOURCE=.\res\port2.ico
# End Source File
# Begin Source File

SOURCE=.\res\red_led.bmp
# End Source File
# Begin Source File

SOURCE=.\res\red_led.ico
# End Source File
# Begin Source File

SOURCE=.\res\right.ico
# End Source File
# Begin Source File

SOURCE=.\res\run.bmp
# End Source File
# Begin Source File

SOURCE=.\res\run.ico
# End Source File
# Begin Source File

SOURCE=.\res\TREEBTNS.bmp
# End Source File
# Begin Source File

SOURCE=.\res\usb.ico
# End Source File
# Begin Source File

SOURCE=.\res\wrong.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\FactoryTool.manifest
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
