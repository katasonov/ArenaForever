!define prodname "Квака Онлайн"

#!define rusname "Квака Онлайн"

!define setup "QuakaOnlineSetup.exe"

!define uninstname "Удалить (Uninstall)"

!define exec "AFLauncher.exe"


!define uninstkey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}"
 
#!define startmenu "$SMPROGRAMS\${prodname}"
!define uninstaller "${uninstname}.exe"


!define MUI_ICON "icon128.ico"

# define name of installer
SetCompressor lzma

; The default installation directory
OutFile "${setup}"
InstProgressFlags smooth

 
# define installation directory
InstallDir $LOCALAPPDATA\Quaka.online
 
# For removing Start Menu shortcut in Windows 7
RequestExecutionLevel user


; WriteUninstaller macro
!macro WriteUninstaller Path
 WriteUninstaller "${Path}"
 FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define WriteUninstaller "!insertmacro WriteUninstaller"

# start default section
Section ""
 
   
	SetOverwrite on
 
    # set the installation directory as the destination for the following actions
    SetOutPath $INSTDIR
 
	# specify file to go in output path
	File sciter.dll
	File 7za.exe
	File AFLauncher.exe
	File afmvbins.exe
	File afq3mod.exe
	File icon128.ico

	 
    # create a shortcut named "new shortcut" in the start menu programs directory
    # presently, the new shortcut doesn't call anything (the second field is blank)
    CreateShortCut "$SMPROGRAMS\${prodname}.lnk" "$INSTDIR\AFLauncher.exe"
	
	#CreateShortCut "$SMPROGRAMS\${uninstname}.lnk" "$INSTDIR\${uninstaller}"
	
	WriteRegStr HKCU "${uninstkey}" "DisplayName" "${prodname}"
	WriteRegStr HKCU "${uninstkey}" "UninstallString" "$INSTDIR\${uninstaller}"
	WriteRegStr HKCU "${uninstkey}" "DisplayIcon" "$INSTDIR\icon128.ico"
	WriteRegStr HKCU "${uninstkey}" "Publisher" "Alexander Katasonov"

    # create the uninstaller
    WriteUninstaller "$INSTDIR\${uninstaller}"

	Exec "$INSTDIR\${exec}"
	
	Quit
	
SectionEnd
 
# uninstaller section start
Section Uninstall
 
    # first, delete the uninstaller
    Delete "$INSTDIR\${uninstaller}"
 
    # second, remove the link from the start menu
    Delete "$SMPROGRAMS\${prodname}.lnk"
	
	# now delete installed file
	Delete $INSTDIR\sciter.dll
	Delete $INSTDIR\7za.exe
	Delete $INSTDIR\AFLauncher.exe
	Delete $INSTDIR\afmvbins.exe
	Delete $INSTDIR\afq3mod.exe
	Delete $INSTDIR\icon128.ico
	
	DeleteRegKey    HKCU "${uninstkey}"
 
# uninstaller section end
SectionEnd