#define InstallerFileName = "Jamtaba 2 (64 bits) installer"
#define BuildDir ".\..\..\..\build-64bit\release"

#define Redistributable "vcredist_x64.exe"

#include "Installer script common.iss"

[Setup]
ArchitecturesInstallIn64BitMode=x64
