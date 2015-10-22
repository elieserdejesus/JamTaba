#define InstallerFileName = "Jamtaba 2 (64 bits) installer"
#define QtDir "C:\Qt\Qt5.5.0-64\5.5\msvc2013_64\bin"
#define SslLibsDir "C:\OpenSSL-Win64"
#define BuildDir ".\..\..\..\..\build-64bit\release"

#define MsvcRedistributable "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\1033\vcredist_x64.exe"

#include "Installer script common.iss"

[Setup]
ArchitecturesInstallIn64BitMode=x64
