# Microsoft Developer Studio Project File - Name="tptlib_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tptlib_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tptlib_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tptlib_static.mak" CFG="tptlib_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tptlib_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tptlib_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tptlib_static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD -I../inc /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "tptlib_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ -I../inc /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib\tptlib_static_debug.lib"

!ENDIF 

# Begin Target

# Name "tptlib_static - Win32 Release"
# Name "tptlib_static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\buffer.cxx

!IF  "$(CFG)" == "tptlib_static - Win32 Release"

!ELSEIF  "$(CFG)" == "tptlib_static - Win32 Debug"

# PROP Intermediate_Dir "../dbg"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\lexical.cxx

!IF  "$(CFG)" == "tptlib_static - Win32 Release"

!ELSEIF  "$(CFG)" == "tptlib_static - Win32 Debug"

# PROP Intermediate_Dir "../dbg"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\parse.cxx

!IF  "$(CFG)" == "tptlib_static - Win32 Release"

!ELSEIF  "$(CFG)" == "tptlib_static - Win32 Debug"

# PROP Intermediate_Dir "../dbg"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\parse_impl.cxx
# End Source File
# Begin Source File

SOURCE=..\src\symbols.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\inc\tptlib\buffer.h
# End Source File
# Begin Source File

SOURCE=..\src\lexical.h
# End Source File
# Begin Source File

SOURCE=..\src\macro.h
# End Source File
# Begin Source File

SOURCE=..\inc\tptlib\parse.h
# End Source File
# Begin Source File

SOURCE=..\src\parse_impl.h
# End Source File
# Begin Source File

SOURCE=..\inc\tptlib\symbols.h
# End Source File
# Begin Source File

SOURCE=..\src\token.h
# End Source File
# Begin Source File

SOURCE=..\src\ttypes.h
# End Source File
# Begin Source File

SOURCE=..\src\ttypes.inl
# End Source File
# End Group
# End Target
# End Project
