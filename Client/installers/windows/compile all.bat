ISCC.exe /Qp "Installer script 32 bits.iss"
ISCC.exe /Qp "Installer script 64 bits.iss"

rar -inul a "C:\Users\Elieser\Dropbox\Public\jamtaba\jamtaba2\Jamtaba 2 (32 bits) installer.rar" "Jamtaba 2 (32 bits) installer.exe"
rar -inul a "C:\Users\Elieser\Dropbox\Public\jamtaba\jamtaba2\Jamtaba 2 (64 bits) installer.rar" "Jamtaba 2 (64 bits) installer.exe" 

rm Jamtaba 2 (32 bits) installer.exe
rm Jamtaba 2 (64 bits) installer.exe