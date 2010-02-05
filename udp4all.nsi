;NSIS Modern User Interface
;Basic Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "udp4all-r3"
  OutFile "udp4all-r3.exe"
  Icon "C:\Marum\progs\udp4all\src\images\udp4all.ico"
  XPStyle on
  
  ;Default installation folder
  InstallDir "$PROGRAMFILES\Marum\udp4all"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Marum\udp4all" ""
  
;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "C:\Marum\progs\udp4all\lgpl-2.1.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"
;--------------------------------
;Version Information

  VIProductVersion "0.7.10.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "udp4all"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Quick and Dirty"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Marum"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright Marum"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "udp4all"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "0.1.3"

;--------------------------------


;--------------------------------
;Installer Sections

Section "udp4all" SecDummy

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  File C:\Marum\progs\udp4all\bin\udp4all.exe
  File C:\Programme\Qt\2010.01\qt\bin\QtGui4.dll 
  File C:\Programme\Qt\2010.01\qt\bin\QtCore4.dll
  File C:\Programme\Qt\2010.01\qt\bin\mingwm10.dll
  File C:\Programme\Qt\2010.01\qt\bin\QtNetwork4.dll
  File C:\Programme\Qt\2010.01\mingw\bin\libgcc_s_dw2-1.dll 
  File C:\Marum\progs\udp4all\u4a-example.conf
  

  ;Store installation folder
  WriteRegStr HKCU "Software\Marum\udp4all" "" $INSTDIR

  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\udp4all" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\udp4all" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\udp4all" "DisplayName" "udp4all"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\udp4all" "DisplayIcon" "$INSTDIR\udp4all.exe,0"


  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  SetShellVarContext All

  CreateDirectory $SMPROGRAMS\Marum
  CreateShortCut $SMPROGRAMS\Marum\udp4all.lnk $INSTDIR\udp4all.exe
  CreateShortCut $DESKTOP\udp4all.lnk $INSTDIR\udp4all.exe



SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "Quick and dirty udp distributor"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\udp4all.exe"
  Delete "$INSTDIR\*.dll"

  RMDir "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\Marum\udp4all"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\udp4all"

  SetShellVarContext All

  Delete "$SMPROGRAMS\Marum\udp4all.lnk" 
  Delete "$DESKTOP\udp4all.lnk"
  RMDir $SMPROGRAMS\Marum
  
SectionEnd
