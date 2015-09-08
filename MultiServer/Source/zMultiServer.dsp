# Microsoft Developer Studio Project File - Name="zMultiServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=zMultiServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zMultiServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zMultiServer.mak" CFG="zMultiServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zMultiServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "zMultiServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zMultiServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib ws2_32.lib /nologo /subsystem:windows /pdb:"../Release/zMultiServer.pdb" /map /debug /machine:I386 /out:"../Release/zMultiServer.exe" /libpath:"C:\SDK\Lib"
# SUBTRACT LINK32 /pdb:none /force

!ELSEIF  "$(CFG)" == "zMultiServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib ws2_32.lib /nologo /subsystem:windows /pdb:"..\..\BUILD\eDataServer\zMultiServer.pdb" /debug /machine:I386 /out:"..\..\BUILD\eDataServer\zMultiServer.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "zMultiServer - Win32 Release"
# Name "zMultiServer - Win32 Debug"
# Begin Group "Server MNG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Server.cpp
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# End Group
# Begin Group "IOCP"

# PROP Default_Filter ""
# Begin Group "Packet Queue"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WzQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\WzQueue.h
# End Source File
# End Group
# Begin Group "WzUdp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WzUdp.cpp
# End Source File
# Begin Source File

SOURCE=.\WzUdp.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\giocp.cpp
# End Source File
# Begin Source File

SOURCE=.\giocp.h
# End Source File
# End Group
# Begin Group "Servers"

# PROP Default_Filter ""
# Begin Group "JoinServer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\JoinServer.cpp
# End Source File
# Begin Source File

SOURCE=.\JoinServer.h
# End Source File
# End Group
# Begin Group "DataServer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DataServer.cpp
# End Source File
# Begin Source File

SOURCE=.\DataServer.h
# End Source File
# End Group
# Begin Group "RankingServer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ranking.cpp
# End Source File
# Begin Source File

SOURCE=.\Ranking.h
# End Source File
# End Group
# Begin Group "EventServer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EventServer.cpp
# End Source File
# Begin Source File

SOURCE=.\EventServer.h
# End Source File
# End Group
# Begin Group "CashShopServer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CashShopServer.cpp
# End Source File
# Begin Source File

SOURCE=.\CashShopServer.h
# End Source File
# End Group
# End Group
# Begin Group "Database MNG"

# PROP Default_Filter ""
# Begin Group "dbClass"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\CQuery.h
# End Source File
# End Group
# Begin Group "JoinServerDB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\JoinServerDB.cpp
# End Source File
# Begin Source File

SOURCE=.\JoinServerDB.h
# End Source File
# End Group
# Begin Group "DataServerDB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DataServerDB.cpp
# End Source File
# Begin Source File

SOURCE=.\DataServerDB.h
# End Source File
# End Group
# Begin Group "PartyClass"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PartyClass.cpp
# End Source File
# Begin Source File

SOURCE=.\PartyClass.h
# End Source File
# End Group
# Begin Group "GuildClass"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GuildClass.cpp
# End Source File
# Begin Source File

SOURCE=.\GuildClass.h
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\zMultiServer.ico
# End Source File
# Begin Source File

SOURCE=.\zMultiServer.rc
# End Source File
# Begin Source File

SOURCE=.\zMultiServerSmall.ico
# End Source File
# End Group
# Begin Group "Utils"

# PROP Default_Filter ""
# Begin Group "MD5"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MD5.cpp
# End Source File
# Begin Source File

SOURCE=.\MD5.h
# End Source File
# Begin Source File

SOURCE=.\MD5_KEYVAL.H
# End Source File
# End Group
# Begin Source File

SOURCE=.\CallStackTrace.cpp
# End Source File
# Begin Source File

SOURCE=.\CallStackTrace.h
# End Source File
# Begin Source File

SOURCE=.\winutil.cpp
# End Source File
# Begin Source File

SOURCE=.\winutil.h
# End Source File
# End Group
# Begin Group "Window"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Window.cpp
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\zMultiServer.cpp
# End Source File
# Begin Source File

SOURCE=.\zMultiServer.h
# End Source File
# End Target
# End Project
