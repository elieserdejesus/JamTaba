#define MyAppName "JamTaba 2"
#define MyAppVersion "1"
#define MyAppPublisher "Jamtaba"
#define MyAppURL "http://www.jamtaba.com"
#define MyAppExeName "Jamtaba2.exe"


[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{ACBD9FA8-0FF5-484F-8BE1-EC577495851E}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=true
OutputDir=.
OutputBaseFilename={#InstallerFileName}
SetupIconFile=16x16.ico
Compression=lzma
SolidCompression=true
ShowLanguageDialog=no
WizardImageFile=jamtaba.bmp
WizardImageBackColor=clWhite
WizardImageStretch=false
WizardSmallImageFile=Jamtaba48x48.bmp
UsePreviousAppDir=false
UsePreviousUserInfo=false
UsePreviousGroup=false

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; OnlyBelowVersion: 0,6.1

[Messages]
WelcomeLabel1=Welcome to Jamtaba 2 Installer!
WelcomeLabel2=This will install Jamtaba 2 on your computer.

[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: {#BuildDir}\Jamtaba2.exe; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#QtDir}\Qt5Core.dll; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#QtDir}\Qt5Gui.dll; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#QtDir}\Qt5Network.dll; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#QtDir}\Qt5Widgets.dll; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#SslLibsDir}\ssleay32.dll; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#SslLibsDir}\libeay32.dll; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#MsvcRedistDir}\msvcp120.dll; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#MsvcRedistDir}\msvcr120.dll; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#QtDir}\..\plugins\platforms\qwindows.dll; DestDir: {app}\platforms; Flags: ignoreversion replacesameversion
Source: {#QtDir}\..\plugins\bearer\qgenericbearer.dll; DestDir: {app}\bearer; Flags: ignoreversion replacesameversion
Source: {#QtDir}\..\plugins\bearer\qnativewifibearer.dll; DestDir: {app}\bearer; Flags: ignoreversion replacesameversion
Source: {#QtDir}\..\plugins\imageformats\qgif.dll; DestDir: {app}\imageformats; Flags: ignoreversion replacesameversion
Source: 48x48.ico; DestDir: {app}; Flags: deleteafterinstall

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; IconFilename: {app}\48x48.ico; IconIndex: 0
Name: {group}\{cm:ProgramOnTheWeb,{#MyAppName}}; Filename: {#MyAppURL}
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {commondesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon; IconFilename: {app}\48x48.ico; IconIndex: 0
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: quicklaunchicon

[Run]
Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}; Flags: nowait postinstall skipifsilent
