# Microsoft Developer Studio Project File - Name="libtpt_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libtpt_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libtpt_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtpt_static.mak" CFG="libtpt_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtpt_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libtpt_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libtpt_static - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD -I../inc /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libtpt_static - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ -I../inc /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib\libtpt_static_debug.lib"

!ENDIF 

# Begin Target

# Name "libtpt_static - Win32 Release"
# Name "libtpt_static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\lib\buffer.cxx

!IF  "$(CFG)" == "libtpt_static - Win32 Release"

!ELSEIF  "$(CFG)" == "libtpt_static - Win32 Debug"

# PROP Intermediate_Dir "../dbg"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\lib\eval.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\func_math.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\func_misc.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\func_str.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\lexical.cxx

!IF  "$(CFG)" == "libtpt_static - Win32 Release"

!ELSEIF  "$(CFG)" == "libtpt_static - Win32 Debug"

# PROP Intermediate_Dir "../dbg"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\lib\object.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse.cxx

!IF  "$(CFG)" == "libtpt_static - Win32 Release"

!ELSEIF  "$(CFG)" == "libtpt_static - Win32 Debug"

# PROP Intermediate_Dir "../dbg"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_funcs.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_impl.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_impl_foreach.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_impl_if.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_impl_include.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_impl_macro.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_impl_rd.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_impl_set.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_impl_while.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\symbols.cxx
# End Source File
# Begin Source File

SOURCE=..\src\lib\symbols_impl.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\inc\libtpt\buffer.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\conf.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\funcs.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\lex_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\lexical.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\macro.h
# End Source File
# Begin Source File

SOURCE=..\inc\libtpt\object.h
# End Source File
# Begin Source File

SOURCE=..\inc\libtpt\parse.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\parse_impl.h
# End Source File
# Begin Source File

SOURCE=..\inc\libtpt\smartptr.h
# End Source File
# Begin Source File

SOURCE=..\inc\libtpt\symbols.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\symbols_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\token.h
# End Source File
# Begin Source File

SOURCE=..\inc\libtpt\tptexcept.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\ttypes.h
# End Source File
# Begin Source File

SOURCE=..\src\lib\ttypes.inl
# End Source File
# Begin Source File

SOURCE=..\src\lib\vars.h
# End Source File
# End Group
# End Target
# End Project
