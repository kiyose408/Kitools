!define PRODUCT_NAME "PC效率工具箱"
!define PRODUCT_VERSION "1.4.1"
!define PRODUCT_PUBLISHER "Kitools Team"
!define PRODUCT_WEB_SITE "https://gitee.com/kitools"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "UninstallString"

; Modern UI
!include "MUI2.nsh"
!include "FileFunc.nsh"
!insertmacro GetParameters
!insertmacro GetOptions

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\win.bmp"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "LICENSE"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Language
!insertmacro MUI_LANGUAGE "SimpChinese"

; Name and output file
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "PCProductivityToolbox_v${PRODUCT_VERSION}_Setup.exe"
InstallDir "$PROGRAMFILES64\PCProductivityToolbox"
RequestExecutionLevel admin

; Version info
VIProductVersion "1.4.1.0"
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "Comments" "A modular productivity tool collection for Windows desktop users"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "Copyright 2026 ${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileDescription" "${PRODUCT_NAME} Setup"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}"

; ==================== Installation ====================
Section "Main Program" SecMain
    SetOutPath $INSTDIR
    
    ; Install all files
    File /r "*.*"
    
    ; Create desktop shortcut
    CreateShortCut "$DESKTOP\PC效率工具箱.lnk" "$INSTDIR\PCProductivityToolbox.exe" "" "" 0
    
    ; Create start menu shortcuts
    CreateDirectory "$SMPROGRAMS\PC效率工具箱"
    CreateShortCut "$SMPROGRAMS\PC效率工具箱\PC效率工具箱.lnk" "$INSTDIR\PCProductivityToolbox.exe" "" "" 0
    CreateShortCut "$SMPROGRAMS\PC效率工具箱\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "" 0
    
    ; Write registry keys for Add/Remove Programs
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "DisplayName" "${PRODUCT_NAME}"
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "DisplayIcon" "$INSTDIR\PCProductivityToolbox.exe,0"
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "Publisher" "${PRODUCT_PUBLISHER}"
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
    WriteRegDWORD HKLM "${PRODUCT_DIR_REGKEY}" "NoModify" 1
    WriteRegDWORD HKLM "${PRODUCT_DIR_REGKEY}" "NoRepair" 1
    
    ; Write uninstaller
    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

; ==================== Uninstallation ====================
Section "Uninstall"
    ; Remove installed files
    RMDir /r "$INSTDIR"
    
    ; Remove shortcuts
    Delete "$DESKTOP\PC效率工具箱.lnk"
    Delete "$SMPROGRAMS\PC效率工具箱\PC效率工具箱.lnk"
    Delete "$SMPROGRAMS\PC效率工具箱\Uninstall.lnk"
    RMDir "$SMPROGRAMS\PC效率工具箱"
    
    ; Remove registry keys
    DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
SectionEnd
