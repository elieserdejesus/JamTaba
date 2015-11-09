#define MyAppName "JamTaba 2"
#define MyAppVersion "1"
#define MyAppPublisher "Jamtaba"
#define MyAppURL "http://www.jamtaba.com"
#define MyAppExeName "Jamtaba2.exe"

#define MsvcRedistributablesPath "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\1033\"

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
SetupIconFile=E:\Jamtaba2\jamtaba2\Client\Jamtaba2.ico
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
Source: {#BuildDir}\VstPlugin\release\JamtabaVST2.dll; DestDir: {code:GetVST2Dir}; Flags: ignoreversion replacesameversion
Source: ..\..\Jamtaba2.ico; DestDir: {app}; Flags: ignoreversion

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

//  VC_2005_REDIST_X86 = '{A49F249F-0C91-497F-86DF-B2585E8E76B7}';
//  VC_2005_REDIST_X64 = '{6E8E85E8-CE4B-4FF5-91F7-04999C9FAE6A}';
//  VC_2005_REDIST_IA64 = '{03ED71EA-F531-4927-AABD-1C31BCE8E187}';
//  VC_2005_SP1_REDIST_X86 = '{7299052B-02A4-4627-81F2-1818DA5D550D}';
//  VC_2005_SP1_REDIST_X64 = '{071C9B48-7C32-4621-A0AC-3F809523288F}';
//  VC_2005_SP1_REDIST_IA64 = '{0F8FB34E-675E-42ED-850B-29D98C2ECE08}';
//  VC_2005_SP1_ATL_SEC_UPD_REDIST_X86 = '{837B34E3-7C30-493C-8F6A-2B0F04E2912C}';
//  VC_2005_SP1_ATL_SEC_UPD_REDIST_X64 = '{6CE5BAE9-D3CA-4B99-891A-1DC6C118A5FC}';
//  VC_2005_SP1_ATL_SEC_UPD_REDIST_IA64 = '{85025851-A784-46D8-950D-05CB3CA43A13}';
//
//  VC_2008_REDIST_X86 = '{FF66E9F6-83E7-3A3E-AF14-8DE9A809A6A4}';
//  VC_2008_REDIST_X64 = '{350AA351-21FA-3270-8B7A-835434E766AD}';
//  VC_2008_REDIST_IA64 = '{2B547B43-DB50-3139-9EBE-37D419E0F5FA}';
//  VC_2008_SP1_REDIST_X86 = '{9A25302D-30C0-39D9-BD6F-21E6EC160475}';
//  VC_2008_SP1_REDIST_X64 = '{8220EEFE-38CD-377E-8595-13398D740ACE}';
//  VC_2008_SP1_REDIST_IA64 = '{5827ECE1-AEB0-328E-B813-6FC68622C1F9}';
//  VC_2008_SP1_ATL_SEC_UPD_REDIST_X86 = '{1F1C2DFC-2D24-3E06-BCB8-725134ADF989}';
//  VC_2008_SP1_ATL_SEC_UPD_REDIST_X64 = '{4B6C7001-C7D6-3710-913E-5BC23FCE91E6}';
//  VC_2008_SP1_ATL_SEC_UPD_REDIST_IA64 = '{977AD349-C2A8-39DD-9273-285C08987C7B}';
//  VC_2008_SP1_MFC_SEC_UPD_REDIST_X86 = '{9BE518E6-ECC6-35A9-88E4-87755C07200F}';
//  VC_2008_SP1_MFC_SEC_UPD_REDIST_X64 = '{5FCE6D76-F5DC-37AB-B2B8-22AB8CEDB1D4}';
//  VC_2008_SP1_MFC_SEC_UPD_REDIST_IA64 = '{515643D1-4E9E-342F-A75A-D1F16448DC04}';

//  VC_2010_REDIST_X86 = '{196BB40D-1578-3D01-B289-BEFC77A11A1E}';
//  VC_2010_REDIST_X64 = '{DA5E371C-6333-3D8A-93A4-6FD5B20BCC6E}';
//  VC_2010_REDIST_IA64 = '{C1A35166-4301-38E9-BA67-02823AD72A1B}';
//  VC_2010_SP1_REDIST_X86 = '{F0C3E5D1-1ADE-321E-8167-68EF0DE699A5}';
//  VC_2010_SP1_REDIST_X64 = '{1D8E6291-B0D5-35EC-8441-6616F567A0F7}';
//  VC_2010_SP1_REDIST_IA64 = '{88C73C1C-2DE5-3B01-AFB8-B46EF4AB41CD}';
//
//  // Microsoft Visual C++ 2012 x86 Minimum Runtime - 11.0.61030.0 (Update 4)
//  VC_2012_REDIST_MIN_UPD4_X86 = '{BD95A8CD-1D9F-35AD-981A-3E7925026EBB}';
//  VC_2012_REDIST_MIN_UPD4_X64 = '{CF2BEA3C-26EA-32F8-AA9B-331F7E34BA97}';
//  // Microsoft Visual C++ 2012 x86 Additional Runtime - 11.0.61030.0 (Update 4)
//  VC_2012_REDIST_ADD_UPD4_X86 = '{B175520C-86A2-35A7-8619-86DC379688B9}';
//  VC_2012_REDIST_ADD_UPD4_X64 = '{37B8F9C7-03FB-3253-8781-2517C99D7C00}';

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


[Registry]
Root: HKCU; Subkey: Software\Jamtaba; ValueType: string; ValueName: VST2InstallDir; Flags: uninsdeletekey
