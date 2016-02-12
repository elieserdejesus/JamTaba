#define MyAppName "JamTaba 2"
#define MyAppVersion "1"
#define MyAppPublisher "Jamtaba"
#define MyAppURL "http://www.jamtaba.com"
#define MyAppExeName "Jamtaba2.exe"

#define MsvcRedistributablesPath "../../../../"

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
SetupIconFile=..\..\PROJECTS\Jamtaba2.ico
Compression=lzma
SolidCompression=true
ShowLanguageDialog=no
WizardImageFile=Jamtaba.bmp
WizardImageBackColor=clWhite
WizardImageStretch=false
WizardSmallImageFile=Jamtaba48x48.bmp
UsePreviousAppDir=true
UsePreviousUserInfo=true
UsePreviousGroup=true
DirExistsWarning=no
;EnableDirDoesntExistWarning=false


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
Source: {#MsvcRedistributablesPath}\{#Redistributable}; DestDir: {tmp}; Flags: deleteafterinstall

Source: {#BuildDir}\Standalone\release\Jamtaba2.exe; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#BuildDir}\VstScanner\release\VstScanner.exe; DestDir: {app}; Flags: ignoreversion replacesameversion
Source: {#BuildDir}\VstPlugin\release\JamtabaVST2.dll; DestDir: {code:GetVST2Dir}; Flags: ignoreversion replacesameversion
Source: ..\..\PROJECTS\Jamtaba2.ico; DestDir: {app}; Flags: ignoreversion

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; IconFilename: {app}\Jamtaba2.ico; IconIndex: 0
Name: {group}\{cm:ProgramOnTheWeb,{#MyAppName}}; Filename: {#MyAppURL}; IconFilename: {app}\Jamtaba2.ico; IconIndex: 0
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}; IconFilename: {app}\Jamtaba2.ico; IconIndex: 0
Name: {commondesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon; IconFilename: {app}\Jamtaba2.ico; IconIndex: 0; Languages: 
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: quicklaunchicon; IconFilename: {app}\Jamtaba2.ico; IconIndex: 0

[Run]
; add the Parameters, WorkingDir and StatusMsg as you wish, just keep here the conditional installation Check
Filename: {tmp}\{#Redistributable}; Check: VCRedistNeedsInstall; Parameters: "/install /quiet /norestart "

Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}; Flags: nowait postinstall skipifsilent


[Registry]
Root: HKCU; Subkey: Software\Jamtaba; ValueType: string; ValueName: VST2InstallDir; Flags: uninsdeletekey
[Code]
var
  VST2DirPage: TInputDirWizardPage;

procedure InitializeWizard;
begin
  if IsWin64 then begin
    VST2DirPage := CreateInputDirPage(wpSelectDir,
    'Confirm 64-Bit VST Plugin Directory', '',
    'Select the folder in which setup should install the 64-bit VST Plugin, then click Next.',
    False, '');
    VST2DirPage.Add('');
    //try recoder the VST directory used in last install. If this value is not founded use the VST path in windows registry.
    VST2DirPage.Values[0] := GetPreviousData('VST2InstallDir', ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{pf}\Steinberg\VSTPlugins}\'));
  end else begin
    VST2DirPage := CreateInputDirPage(wpSelectDir,
      'Confirm 32-Bit VST Plugin Directory', '',
      'Select the folder in which setup should install the 32-bit VST Plugin, then click Next.',
      False, '');
    VST2DirPage.Add('');
    VST2DirPage.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{pf}\Steinberg\VSTPlugins}\');
  end;
end;

function GetVST2Dir(Param: String): String;
begin
  Result := VST2DirPage.Values[0]
end;

//+++++++++++++++++++++++++
//Remember VST2 dir in next installs
procedure RegisterPreviousData(PreviousDataKey: Integer);
begin
  SetPreviousData(PreviousDataKey, 'VST2InstallDir', VST2DirPage.Values[0]);

  //store vst path in registry too, so Jamtaba can this path and use to scan vst plugins
  RegWriteStringValue(HKEY_CURRENT_USER, 'Software\Jamtaba', 'VST2InstallDir', VST2DirPage.Values[0]);
end;
//+++++++++++++++++++++++++

// MSVC REDISTRIBUTABLE
#IFDEF UNICODE
  #DEFINE AW "W"
#ELSE
  #DEFINE AW "A"
#ENDIF
type
  INSTALLSTATE = Longint;
const
  INSTALLSTATE_INVALIDARG = -2;  // An invalid parameter was passed to the function.
  INSTALLSTATE_UNKNOWN = -1;     // The product is neither advertised or installed.
  INSTALLSTATE_ADVERTISED = 1;   // The product is advertised but not installed.
  INSTALLSTATE_ABSENT = 2;       // The product is installed for a different user.
  INSTALLSTATE_DEFAULT = 5;      // The product is installed for the current user.

  // Visual C++ 2013 Redistributable 12.0.21005
  VC_2013_REDIST_X86_MIN = '{13A4EE12-23EA-3371-91EE-EFB36DDFFF3E}';
  VC_2013_REDIST_X64_MIN = '{A749D8E6-B613-3BE3-8F5F-045C84EBA29B}';

  VC_2013_REDIST_X86_ADD = '{F8CFEB22-A2E7-3971-9EDA-4B11EDEFC185}';
  VC_2013_REDIST_X64_ADD = '{929FBD26-9020-399B-9A7A-751D61F0B942}';

//  // Visual C++ 2015 Redistributable 14.0.23026
//  VC_2015_REDIST_X86_MIN = '{A2563E55-3BEC-3828-8D67-E5E8B9E8B675}';
//  VC_2015_REDIST_X64_MIN = '{0D3E9E15-DE7A-300B-96F1-B4AF12B96488}';
//
//  VC_2015_REDIST_X86_ADD = '{BE960C1C-7BAD-3DE6-8B1A-2616FE532845}';
//  VC_2015_REDIST_X64_ADD = '{BC958BD2-5DAC-3862-BB1A-C1BE0790438D}';

function MsiQueryProductState(szProduct: string): INSTALLSTATE;
  external 'MsiQueryProductState{#AW}@msi.dll stdcall';

function VCVersionInstalled(const ProductID: string): Boolean;
begin
  Result := MsiQueryProductState(ProductID) = INSTALLSTATE_DEFAULT;
end;

function VCRedistNeedsInstall: Boolean;
begin
  // here the Result must be True when you need to install your VCRedist or False when you don't need to
   if IsWin64 then begin
		Result := not VCVersionInstalled(VC_2013_REDIST_X64_ADD);
   end else begin
		Result := not VCVersionInstalled(VC_2013_REDIST_X86_ADD);
   end;
end;


//removing Jamtaba 2 from AppData
procedure CurUninstallStepChanged (CurUninstallStep: TUninstallStep);
 var
     msgBoxResult : integer;
 begin
    case CurUninstallStep of
      usPostUninstall:
        begin
          msgBoxResult := MsgBox('Do you want to delete Jamtaba settings?', mbConfirmation, MB_YESNO or MB_DEFBUTTON2)
          if msgBoxResult = IDYES then begin
            MsgBox('trying to remove ' + ExpandConstant('{localappdata}\Jamtaba 2'), mbConfirmation, MB_YESNO or MB_DEFBUTTON2)
            DelTree(ExpandConstant('{localappdata}\Jamtaba 2'), True, True, True);
          end;
       end;
   end;
end;


