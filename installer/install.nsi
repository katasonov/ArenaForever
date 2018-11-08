;Unicode true
RequestExecutionLevel user ;Require admin rights on NT6+ (When UAC is turned on)
;RequestExecutionLevel user ;

;!include "MUI2.nsh"
!include "MUI.nsh"
!include "LogicLib.nsh"
!include "nsResize.nsh"
!include "WordFunc.nsh"
!include "UAC.nsh"
!include "Registry.nsh"
!include "WinVer.nsh"


!macro Init thing
uac_tryagain:
!insertmacro UAC_RunElevated
${Switch} $0
${Case} 0
	${IfThen} $1 = 1 ${|} Quit ${|} ;we are the outer process, the inner process has done its work, we are done
	${IfThen} $3 <> 0 ${|} ${Break} ${|} ;we are admin, let the show go on
	${If} $1 = 3 ;RunAs completed successfully, but with a non-admin user
		MessageBox mb_YesNo|mb_IconExclamation|mb_TopMost|mb_SetForeground "Для установки требуются права Администратора" /SD IDNO IDYES uac_tryagain IDNO 0
	${EndIf}
	;fall-through and die
${Case} 1223
	MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "Для продолжения нужны права Администратора!"
	Quit
${Case} 1062
	MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "Logon service not running, aborting!"
	Quit
${Default}
	MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "Unable to elevate, error $0"
	Quit
${EndSwitch}
 
SetShellVarContext all
!macroend
Var HWND
Var DLGITEM
Var FONT

VAR USERDESKTOP
VAR USERSTARTMENU
VAR USERQUICKLAUNCH
VAR MYREALPATH


!define CharToASCII "!insertmacro CharToASCII" 
 
!macro CharToASCII AsciiCode Character
  Push "${Character}"
  Call CharToASCII
  Pop "${AsciiCode}"
!macroend
 
Function CharToASCII
  Exch $0 ; given character
  Push $1 ; current character
  Push $2 ; current Ascii Code   
 
  StrCpy $2 1 ; right from start
Loop:
  IntFmt $1 %c $2 ; Get character from current ASCII code
  ${If} $1 S== $0 ; case sensitive string comparison
     StrCpy $0 $2
     Goto Done
  ${EndIf}
  IntOp $2 $2 + 1
  StrCmp $2 255 0 Loop ; ascii from 1 to 255
  StrCpy $0 0 ; ASCII code wasn't found -> return 0
Done:         
  Pop $2
  Pop $1
  Exch $0
FunctionEnd

!define StrLoc "!insertmacro StrLoc"
 
!macro StrLoc ResultVar String SubString StartPoint
  Push "${String}"
  Push "${SubString}"
  Push "${StartPoint}"
  Call StrLoc
  Pop "${ResultVar}"
!macroend
 
Function StrLoc
/*After this point:
  ------------------------------------------
   $R0 = StartPoint (input)
   $R1 = SubString (input)
   $R2 = String (input)
   $R3 = SubStringLen (temp)
   $R4 = StrLen (temp)
   $R5 = StartCharPos (temp)
   $R6 = TempStr (temp)*/
 
  ;Get input from user
  Exch $R0
  Exch
  Exch $R1
  Exch 2
  Exch $R2
  Push $R3
  Push $R4
  Push $R5
  Push $R6
 
  ;Get "String" and "SubString" length
  StrLen $R3 $R1
  StrLen $R4 $R2
  ;Start "StartCharPos" counter
  StrCpy $R5 0
 
  ;Loop until "SubString" is found or "String" reaches its end
  ${Do}
    ;Remove everything before and after the searched part ("TempStr")
    StrCpy $R6 $R2 $R3 $R5
 
    ;Compare "TempStr" with "SubString"
    ${If} $R6 == $R1
      ${If} $R0 == `<`
        IntOp $R6 $R3 + $R5
        IntOp $R0 $R4 - $R6
      ${Else}
        StrCpy $R0 $R5
      ${EndIf}
      ${ExitDo}
    ${EndIf}
    ;If not "SubString", this could be "String"'s end
    ${If} $R5 >= $R4
      StrCpy $R0 ``
      ${ExitDo}
    ${EndIf}
    ;If not, continue the loop
    IntOp $R5 $R5 + 1
  ${Loop}
 
  ;Return output to user
  Pop $R6
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Exch
  Pop $R1
  Exch $R0
FunctionEnd


Function Slice
 Exch $R0 ; input string
 Exch
 Exch $R1 ; to cut
 Push $R2
 Push $R3
 Push $R4
 Push $R5
 
 StrLen $R3 $R1
 StrCpy $R4 -1
 StrCpy $R5 0
 
 Loop:
 
  IntOp $R4 $R4 + 1
  StrCpy $R2 $R0 $R3 $R4
  StrCmp $R2 "" Done
  StrCmp $R2 $R1 0 Loop
 
   StrCpy $R5 1
 
   StrCmp $R4 0 0 +3
    StrCpy $R1 ""
    Goto +2
   StrCpy $R1 $R0 $R4
   StrLen $R2 $R0
   IntOp $R4 $R2 - $R4
   IntOp $R4 $R4 - $R3
   IntCmp $R4 0 0 0 +3
    StrCpy $R2 ""
    Goto +2
   StrCpy $R2 $R0 "" -$R4
   StrCpy $R0 $R1$R2
 
 Done:
 StrCpy $R1 $R5
 
 Pop $R5
 Pop $R4
 Pop $R3
 Pop $R2
 Exch $R1 ; slice? 0/1
 Exch
 Exch $R0 ; output string
FunctionEnd

!include "Base64.nsh"


Function openLinkNewWindow
  Push $3
  Exch
  Push $2
  Exch
  Push $1
  Exch
  Push $0
  Exch
 
  ReadRegStr $0 HKCU "http\shell\open\command" ""
# Get browser path
    DetailPrint $0
  StrCpy $2 '"'
  StrCpy $1 $0 1
  StrCmp $1 $2 +2 # if path is not enclosed in " look for space as final char
    StrCpy $2 ' '
  StrCpy $3 1
  loop:
    StrCpy $1 $0 1 $3
    DetailPrint $1
    StrCmp $1 $2 found
    StrCmp $1 "" found
    IntOp $3 $3 + 1
    Goto loop
 
  found:
    StrCpy $1 $0 $3
    StrCmp $2 " " +2
      StrCpy $1 '$1"'
 
  Pop $0
  Exec '$1 $0'
  Pop $0
  Pop $1
  Pop $2
  Pop $3
FunctionEnd
 
!macro _OpenURL URL
Push "${URL}"
Call openLinkNewWindow
!macroend
 
!define OpenURL '!insertmacro "_OpenURL"'



!define MUI_BGCOLOR 00A3FF
!define MUI_ICON "QuakaWin.ico"

;!insertmacro MUI_HEADER_TEXT "" ""
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "header.logo.bmp"
!define MUI_HEADERIMAGE_LEFT


; These will do the initial window resizing.
!define MUI_CUSTOMFUNCTION_GUIINIT myGUIInit

!define VERSION "1.0.0"
!define NAME	"Quaka.online"

!define setup "QuakaOnlineSetup.exe"
 
; change this to wherever the files to be packaged reside
!define srcdir "."
 
;!define company "Technicat"
 
!define prodname "Quaka.online"
!define uninstname "Удалить (Uninstall)"
!define exec "quakal.exe"
 
; optional stuff
 
; text file to open in notepad after installation
; !define notefile "README.txt"
 
; license text file
; !define licensefile license.txt
 
; icons must be Microsoft .ICO files
;!define icon "logo-128.ico"
;!define uninsticon "uninstaller.ico"
 
; installer background screen
; !define screenimage background.bmp
 
; file containing list of file-installation commands
; !define files "files.nsi"
 
; file containing list of file-uninstall commands
; !define unfiles "unfiles.nsi"
 
; registry stuff





!define regkey "Software\${prodname}"
!define uninstkey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}"
!define autorunkey "Software\Microsoft\Windows\CurrentVersion\Run"
 
!define startmenu "$SMPROGRAMS\${prodname}"
!define uninstaller "Удаление (Uninstall).exe"

 ;Interface Settings

  !define MUI_ABORTWARNING
	
SetCompressor lzma

; The default installation directory
InstallDir "$LOCALAPPDATA\Квака Онлайн"
OutFile "${setup}"
InstProgressFlags smooth
Var PrevInstallDir
;Var PrevUninstaller

; Increase the width and height by these units:
!define AddWidth 80p
!define AddHeight 55p


; detect Winamp path from uninstall string if available
;InstallDirRegKey HKCU "${uninstkey}" "UninstallString"

		  
;Name "Federal.tv"	

; The text to prompt the user to enter a directory
;DirText " asdsadsad"

;DetailsButtonText "Показать лог"

;CompletedText " "

;ChangeUI all "${NSISDIR}\Contrib\UIs\modern.exe"

BrandingText " "

; automatically close the installer when done.
AutoCloseWindow false

; adds xp style support
XPStyle on

ShowInstDetails show

RequestExecutionLevel user

;--------------------------------

;Pages
  #!insertmacro MUI_PAGE_WELCOME
  #!insertmacro MUI_PAGE_LICENSE "license.rtf"
  ;Page Custom YandexPage MyYandexPageValidate
  ;Page Custom YandexBrowserPage MyYandexBrowserPageValidate  

  
  ;!insertmacro MUI_PAGE_DIRECTORY
  ;!insertmacro MUI_PAGE_INSTFILES
  
  ; 2 Checkboxes for Quick launch and Desktop.
  ;!define MUI_PAGE_CUSTOMFUNCTION_PRE CreateControls
  ;!define MUI_PAGE_CUSTOMFUNCTION_SHOW SetControlColours
  ;!define MUI_PAGE_CUSTOMFUNCTION_LEAVE DisplayQuickLaunchAndDesktopIcon

ShowInstDetails nevershow
ShowUninstDetails nevershow

!define MUI_INSTFILESPAGE_FINISHHEADER_TEXT " "
!define MUI_INSTFILESPAGE_FINISHHEADER_SUBTEXT "Установка завершена"

  ;!define MUI_PAGE_CUSTOMFUNCTION_PRE HideThisWindow
!define MUI_PAGE_CUSTOMFUNCTION_PRE "HideButtons"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW "HideButtons"
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "HideButtons"
!insertmacro MUI_PAGE_INSTFILES

 ; !define MUI_FINISHPAGE_TEXT_LARGE
  ;!define FINISH_TITLE 'Finished the extra 3 lines test setup wizard. \
;This page has extra space for the finish title!'
;!define MUI_FINISHPAGE_TITLE '${FINISH_TITLE}'
  ;!define MUI_FINISHPAGE_AUTOCLOSE
;!define MUI_FINISHPAGE_TEXT "Удалить программу можно одним следующих способов способов: \
;\n\n1) Пуск > Программы > Скриншотер > Удалить (Uninstall)\n\n2) Пуск > Панель управления > Установка и удаление программ" 

	Page Custom CustomFinishPage CustomFinishPageValidate
  ;!insertmacro MUI_PAGE_FINISH



;Page Custom FinishPage FinishPageLeave

  #!insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "Russian"  
  



Function CustomFinishPage


    Push $R0

    GetDlgItem $R0 $HWNDPARENT 1
    
    CreateFont $0 "Arial" 12
    SendMessage $R0 ${WM_SETFONT} $0 1
    System::Call 'user32::MoveWindow(i$R0,i 230,i380,i 110,i 30,i0)'
    ;System::Call 'user32::SetWindowPos(i$R0,i,i,i,i 110,i 30,i 0x16)'  

    GetDlgItem $R0 $HWNDPARENT 3
    
    System::Call 'user32::MoveWindow(i$R0,i 30,i380,i 80,i 30,i0)'

    ;SendMessage $R0 ${WM_SETTEXT} 0 "STR:Install"
    Pop $R0

	ReserveFile "finishpage.ini"
	!insertmacro MUI_HEADER_TEXT "Завершение установки" ""  
	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "finishpage.ini"

    !insertmacro INSTALLOPTIONS_INITDIALOG "finishpage.ini"
    Pop $HWND
    !insertmacro INSTALLOPTIONS_READ $DLGITEM "finishpage.ini" "Field 1" "HWND"
    CreateFont $FONT "Tahoma" 8 700 
    SendMessage $DLGITEM ${WM_SETFONT} $FONT 0
 
    !insertmacro INSTALLOPTIONS_SHOW

FunctionEnd

Function CustomFinishPageValidate

FunctionEnd




Function HideButtons
;MessageBox MB_OK "Test"
;Quit

    Push $R0
    GetDlgItem $R0 $HWNDPARENT 2
    ShowWindow $R0 0
    
    GetDlgItem $R0 $HWNDPARENT 3    
    ShowWindow $R0 0
    System::Call 'user32::MoveWindow(i$R0,i -1000,i380,i 110,i 30,i0)'
	
	
   Pop $R0
FunctionEnd


Function FinishPage
  
    Push $R0
    GetDlgItem $R0 $HWNDPARENT 3    
    ShowWindow $R0 0
    System::Call 'user32::MoveWindow(i$R0,i -1000,i380,i 110,i 30,i0)'
   Pop $R0


    Push $R0

      

    GetDlgItem $R0 $HWNDPARENT 1
    
    CreateFont $0 "Arial" 14
    SendMessage $R0 ${WM_SETFONT} $0 1
    System::Call 'user32::MoveWindow(i$R0,i 230,i380,i 110,i 30,i0)'
    ;System::Call 'user32::SetWindowPos(i$R0,i,i,i,i 110,i 30,i 0x16)'  

    ;SendMessage $R0 ${WM_SETTEXT} 0 "STR:Install"



    Pop $R0

     #If you need to skip the page depending on a condition, call Abort.
     ;MessageBox MB_OK "YandexPage"
    WriteINIStr "finishpage.ini" "Settings" "InstallType" "1"
    
    ReserveFile "finishpage.ini"
    !insertmacro MUI_HEADER_TEXT "" "Выберите тип установки"
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT "finishpage.ini"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "finishpage.ini"   

FunctionEnd

Function FinishPageLeave
  
  !insertmacro MUI_HEADER_TEXT " " " "

FunctionEnd

;Icon ${icon}

;UninstallIcon unsetup.ico
  
;Pre instalation (for unistaller)
!define UninstLog "uninstall.log"
Var UninstLog
 
; Uninstall log file missing.
LangString UninstLogMissing ${LANG_RUSSIAN} "${UninstLog} не найден!$\r$\nУдаление не может быть продолжено!"
 
; AddItem macro
!macro AddItem Path
 FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define AddItem "!insertmacro AddItem"
 
; File macro
!macro File FilePath FileName
 IfFileExists "$OUTDIR\${FileName}" +2
  FileWrite $UninstLog "$OUTDIR\${FileName}$\r$\n"
 File "${FilePath}${FileName}"
!macroend
!define File "!insertmacro File"
 
; Copy files macro
!macro CopyFiles SourcePath DestPath
 IfFileExists "${DestPath}" +2
  FileWrite $UninstLog "${DestPath}$\r$\n"
 CopyFiles "${SourcePath}" "${DestPath}"
!macroend
!define CopyFiles "!insertmacro CopyFiles"
 
; Rename macro
!macro Rename SourcePath DestPath
 IfFileExists "${DestPath}" +2
  FileWrite $UninstLog "${DestPath}$\r$\n"
 Rename "${SourcePath}" "${DestPath}"
!macroend
!define Rename "!insertmacro Rename"
 
; CreateDirectory macro
!macro CreateDirectory Path
 CreateDirectory "${Path}"
 FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define CreateDirectory "!insertmacro CreateDirectory"
 
; SetOutPath macro
!macro SetOutPath Path
 SetOutPath "${Path}"
 FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define SetOutPath "!insertmacro SetOutPath"
 
; WriteUninstaller macro
!macro WriteUninstaller Path
 WriteUninstaller "${Path}"
 FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define WriteUninstaller "!insertmacro WriteUninstaller"
 
Function myGUIInit
  ${nsResize_Window} ${AddWidth} ${AddHeight}
  
Push $R0  
  ; Changing color of title text
    GetDlgItem $R0 $HWNDPARENT 1037
	SetCtlColors $R0 0xFFFFFF 0x00A3FF
    GetDlgItem $R0 $HWNDPARENT 1038
	SetCtlColors $R0 0xFFFFFF 0x00A3FF
Pop $R0  
FunctionEnd

!define StrStr "!insertmacro StrStr"
 
!macro StrStr ResultVar String SubString
  Push `${String}`
  Push `${SubString}`
  Call StrStr
  Pop `${ResultVar}`
!macroend
 
Function StrStr
/*After this point:
  ------------------------------------------
  $R0 = SubString (input)
  $R1 = String (input)
  $R2 = SubStringLen (temp)
  $R3 = StrLen (temp)
  $R4 = StartCharPos (temp)
  $R5 = TempStr (temp)*/
 
  ;Get input from user
  Exch $R0
  Exch
  Exch $R1
  Push $R2
  Push $R3
  Push $R4
  Push $R5
 
  ;Get "String" and "SubString" length
  StrLen $R2 $R0
  StrLen $R3 $R1
  ;Start "StartCharPos" counter
  StrCpy $R4 0
 
  ;Loop until "SubString" is found or "String" reaches its end
  ${Do}
    ;Remove everything before and after the searched part ("TempStr")
    StrCpy $R5 $R1 $R2 $R4
 
    ;Compare "TempStr" with "SubString"
    ${IfThen} $R5 == $R0 ${|} ${ExitDo} ${|}
    ;If not "SubString", this could be "String"'s end
    ${IfThen} $R4 >= $R3 ${|} ${ExitDo} ${|}
    ;If not, continue the loop
    IntOp $R4 $R4 + 1
  ${Loop}
 
/*After this point:
  ------------------------------------------
  $R0 = ResultVar (output)*/
 
  ;Remove part before "SubString" on "String" (if there has one)
  StrCpy $R0 $R1 `` $R4
 
  ;Return output to user
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Exch $R0
FunctionEnd

!define StrRep "!insertmacro StrRep"
!macro StrRep output string old new
    Push "${string}"
    Push "${old}"
    Push "${new}"
    !ifdef __UNINSTALL__
        Call un.StrRep
    !else
        Call StrRep
    !endif
    Pop ${output}
!macroend
 
!macro Func_StrRep un
    Function ${un}StrRep
        Exch $R2 ;new
        Exch 1
        Exch $R1 ;old
        Exch 2
        Exch $R0 ;string
        Push $R3
        Push $R4
        Push $R5
        Push $R6
        Push $R7
        Push $R8
        Push $R9
 
        StrCpy $R3 0
        StrLen $R4 $R1
        StrLen $R6 $R0
        StrLen $R9 $R2
        loop:
            StrCpy $R5 $R0 $R4 $R3
            StrCmp $R5 $R1 found
            StrCmp $R3 $R6 done
            IntOp $R3 $R3 + 1 ;move offset by 1 to check the next character
            Goto loop
        found:
            StrCpy $R5 $R0 $R3
            IntOp $R8 $R3 + $R4
            StrCpy $R7 $R0 "" $R8
            StrCpy $R0 $R5$R2$R7
            StrLen $R6 $R0
            IntOp $R3 $R3 + $R9 ;move offset by length of the replacement string
            Goto loop
        done:
 
        Pop $R9
        Pop $R8
        Pop $R7
        Pop $R6
        Pop $R5
        Pop $R4
        Pop $R3
        Push $R0
        Push $R1
        Pop $R0
        Pop $R1
        Pop $R0
        Pop $R2
        Exch $R1
    FunctionEnd
!macroend
!insertmacro Func_StrRep ""

	 
Section -openlogfile
 CreateDirectory "$INSTDIR"
 IfFileExists "$INSTDIR\${UninstLog}" +3
  FileOpen $UninstLog "$INSTDIR\${UninstLog}" w
 Goto +4
  SetFileAttributes "$INSTDIR\${UninstLog}" NORMAL
  FileOpen $UninstLog "$INSTDIR\${UninstLog}" a
  FileSeek $UninstLog 0 END
SectionEnd



; The stuff to install
Section ""

check_run:
  #FindProc $0 "${exec}"
  FindProcDLL::FindProc "${exec}"
  IntCmp $R0 1 0 no_app
	 KillProcDLL::KillProc "${exec}"
	 Sleep 2000
no_app:

IfFileExists $INSTDIR\${exec} 0 +5
Delete $INSTDIR\${exec}
IfErrors 0 +3
MessageBox MB_OK "Перед установкой необходимо закрыть старую версию приложения Квака Онлайн!"
Quit
 
  SetOverwrite on
  ${SetOutPath} "$INSTDIR"

  ${WriteUninstaller} "${uninstaller}"

  ; File to extract
  SetOverwrite try
  
  
  ${File} "" "sciter.dll"
  ${File} "" "7za.exe"
   

  SetOverwrite on

  ${File} "" "quakal.exe"
  ${File} "" "QuakaWin.ico"
  ${File} "" "afmvbins.exe"
  ${File} "" "afq3mod.exe"

  

  WriteRegStr HKCU "${uninstkey}" "DisplayName" "Квака Онлайн"
  WriteRegStr HKCU "${uninstkey}" "UninstallString" "$INSTDIR\${uninstaller}"
  WriteRegStr HKCU "${uninstkey}" "DisplayIcon" "$INSTDIR\QuakaWin.ico"
  WriteRegStr HKCU "${uninstkey}" "Publisher" "Alexander Katasonov"
  ;WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Run" \
   ; "Federal.tv" "$INSTDIR\Federal.tv.exe -h"
  ;WriteRegStr HKCU "${autorunkey}" "${prodname}" "$INSTDIR\${exec}"
  
  SetOverwrite off
  
  SetShellVarContext all

SectionEnd

;Call CreateGUID
;Pop $0 ;contains GUID
Function CreateGUID
  System::Call 'ole32::CoCreateGuid(g .s)'
FunctionEnd


!define CreateGUID `!insertmacro _CreateGUID`
!macro _CreateGUID _RetVar
    Call CreateGUID
    !if ${_RetVar} != s
        Pop ${_RetVar}
    !endif
!macroend

Function GetMacAddress
  IpConfig::GetEnabledNetworkAdaptersIDs
  Pop $0
  Pop $0
  StrCpy $2 0
  StrCpy $4 0
  ClearErrors

    StrCpy $3 $0
    ${WordFind} "$0" " " "+1{" $R0
    IpConfig::GetNetworkAdapterDescription $R0
    Pop $1
    Pop $1
    ${If} $2 == 0
      StrCpy $2 $1
      StrCpy $4 $R0
    ${EndIf}
    ${WordReplace} "$0" "$R0 " "" "E+1" $0
  ClearErrors

  IpConfig::GetNetworkAdapterMACAddress $4
  Pop $1
  Pop $1
  StrCpy $0 $1
  Push $0
FunctionEnd



Function .onInstSuccess


    Exec "$INSTDIR\${exec}"
 

FunctionEnd




; create shortcuts
Section

  CreateDirectory "$USERSTARTMENU"
  ${SetOutPath} $INSTDIR ; for working directory
!ifdef icon
  CreateShortCut "$USERSTARTMENU\${prodname}.lnk" "$INSTDIR\${exec}" "" "$INSTDIR\${icon}"
  CreateShortCut "$USERDESKTOP\${prodname}.lnk" "$INSTDIR\${exec}" "" "$INSTDIR\${icon}"
  CreateShortCut "$USERQUICKLAUNCH\${prodname}.lnk" "$INSTDIR\${exec}" "" "$INSTDIR\${icon}"
!else
  CreateShortCut "$USERSTARTMENU\${prodname}.lnk" "$INSTDIR\${exec}"
  CreateShortCut "$USERDESKTOP\${prodname}.lnk" "$INSTDIR\${exec}"
  CreateShortCut "$USERQUICKLAUNCH\${prodname}.lnk" "$INSTDIR\${exec}"
!endif
 
!ifdef uninsticon
  CreateShortCut "$USERSTARTMENU\${uninstname}.lnk" "$INSTDIR\${uninstaller}" "" "$INSTDIR\${uninsticon}"
!else
  CreateShortCut "$USERSTARTMENU\${uninstname}.lnk" "$INSTDIR\${uninstaller}"
!endif

 
 !ifdef notefile
  CreateShortCut "$USERSTARTMENU\Release Notes.lnk "$INSTDIR\${notefile}"
!endif
 
!ifdef helpfile
  CreateShortCut "$USERSTARTMENU\Documentation.lnk "$INSTDIR\${helpfile}"
!endif
 
 ;create desktop shortcut
  ;CreateShortCut "$DESKTOP\${prodname}.lnk" "$INSTDIR\${exec}" "" "$INSTDIR\${icon}"
  
!ifdef website
WriteINIStr "$USERSTARTMENU\web site.url" "InternetShortcut" "URL" ${website}
 ; CreateShortCut "${startmenu}\Web Site.lnk "${website}" "URL"
!endif
 
!ifdef notefile
ExecShell "open" "$INSTDIR\${notefile}"
!endif
 


SectionEnd
 
 
;unistall
Section -closelogfile
 FileClose $UninstLog
 SetFileAttributes "$INSTDIR\${UninstLog}" READONLY|SYSTEM|HIDDEN
SectionEnd
 
Section Uninstall

 KillProcDLL::KillProc "${exec}"
 Sleep 5000

 Delete /REBOOTOK $INSTDIR\${exec}


#FindProc $0 "${exec}"

	;Call UnregisterShellExtensions
 
 Push $R0
 Push $R1
 Push $R2
 SetFileAttributes "$INSTDIR\${UninstLog}" NORMAL
 FileOpen $UninstLog "$INSTDIR\${UninstLog}" r
 StrCpy $R1 0
 
 GetLineCount:
  ClearErrors
   FileRead $UninstLog $R0
   IntOp $R1 $R1 + 1
   IfErrors 0 GetLineCount
 
 LoopRead:
  FileSeek $UninstLog 0 SET
  StrCpy $R2 0
  FindLine:
   FileRead $UninstLog $R0
   IntOp $R2 $R2 + 1
   StrCmp $R1 $R2 0 FindLine
 
   StrCpy $R0 $R0 -2
   IfFileExists "$R0\*.*" 0 +3
    RMDir $R0  #is dir
   Goto +3
   IfFileExists $R0 0 +2
    Delete $R0 #is file
 
  IntOp $R1 $R1 - 1
  StrCmp $R1 0 LoopDone
  Goto LoopRead
 LoopDone:
 FileClose $UninstLog
 Delete "$INSTDIR\${UninstLog}"
 RMDir "$INSTDIR"
 Pop $R2
 Pop $R1
 Pop $R0

 Delete /REBOOTOK $INSTDIR\${exec}

SectionEnd

Function .onInit

;MessageBox mb_YesNo|mb_IconExclamation|mb_TopMost|mb_SetForeground $EXEPATH /SD IDNO IDYES uac_tryagain IDNO 0
InitPluginsDir
SetOutPath $pluginsdir
SetAutoClose true
;File LauncherSRF.exe


;Save these paths because after installer will work under admin user and we need
;then install files and shortcuts under local user
StrCpy $INSTDIR "$LOCALAPPDATA\Quaka.online"
StrCpy $USERDESKTOP "$DESKTOP"
StrCpy $USERSTARTMENU "${startmenu}"
StrCpy $USERQUICKLAUNCH "$QUICKLAUNCH"
StrCpy $MYREALPATH "$EXEPATH"


;Because we need Federal.tv.exe runned as not admin user, but current user.
;Exec '$pluginsdir\LauncherSRF.exe "$INSTDIR\Federal.tv.exe"'
;Now we need to run installer as admin
;!insertmacro Init "installer"

FunctionEnd

Function un.onInit

	 ; Can't uninstall if uninstall log is missing!
	 IfFileExists "$INSTDIR\${UninstLog}" +3
	  MessageBox MB_OK|MB_ICONSTOP "$(UninstLogMissing)"
	   Abort


	  RMDir /r /REBOOTOK ${startmenu}

	 ;delete desktop shortcut
	  Delete "$DESKTOP\${prodname}.lnk"
	 ;delete quicklaunch shortcut
	  Delete "$QUICKLAUNCH\${prodname}.lnk"  

  
	  DeleteRegKey    HKCU "${uninstkey}"


	;!insertmacro Init "uninstaller"
  ;DeleteRegValue 	HKLM "${autorunkey}" "Federal.tv"


  ;ExecWait 'regsvr32 "/s" "/u" "$INSTDIR\SimpleExt.dll"'
  ;ExecWait 'regsvr32 "/s" "/u" "$INSTDIR\SimpleExt64.dll"'

FunctionEnd

Function CreateControls

  ;move up text control
  WriteINIStr "iospecial.ini" "Field 3" "Bottom" "80"

  WriteINIStr "iospecial.ini" "Field 3" "Text" "Нажмите 'Готово' для выхода из программы установки. Если вы выбрали для установки Яндекс.Бар, то он будет установлен в автономном режиме."

  ; Quicklaunch short cut.
  WriteINIStr "iospecial.ini" "Settings" "NumFields" "4"
  WriteINIStr "iospecial.ini" "Field 4" "Type" "CheckBox"
  WriteINIStr "iospecial.ini" "Field 4" "Text" "&Добавить ярлык на панель быстрого старта"
  WriteINIStr "iospecial.ini" "Field 4" "Left" "120"
  WriteINIStr "iospecial.ini" "Field 4" "Right" "-10"
  WriteINIStr "iospecial.ini" "Field 4" "Top" "100"
  WriteINIStr "iospecial.ini" "Field 4" "Bottom" "112"
  WriteINIStr "iospecial.ini" "Field 4" "State" "1"

  ; Desktop short cut.
  WriteINIStr "iospecial.ini" "Settings" "NumFields" "5"
  WriteINIStr "iospecial.ini" "Field 5" "Type" "CheckBox"
  WriteINIStr "iospecial.ini" "Field 5" "Text" "&Добавить ярлык на рабочий стол"
  WriteINIStr "iospecial.ini" "Field 5" "Left" "120"
  WriteINIStr "iospecial.ini" "Field 5" "Right" "-10"
  WriteINIStr "iospecial.ini" "Field 5" "Top" "114"
  WriteINIStr "iospecial.ini" "Field 5" "Bottom" "126"
  WriteINIStr "iospecial.ini" "Field 5" "State" "1"
  
FunctionEnd

Function SetControlColours
  ReadINIStr $0 "iospecial.ini" "Field 4" "HWND"
  SetCtlColors $0 0x000000 0xFFFFFF
  ReadINIStr $0 "iospecial.ini" "Field 5" "HWND"
  SetCtlColors $0 0x000000 0xFFFFFF

  GetDlgItem $0 $HWNDPARENT 3
  EnableWindow $0 0
FunctionEnd

