~/Qt5.5.0/5.5/clang_64/bin/macdeployqt ~/Desktop/Release/Standalone/Jamtaba2.app -verbose=2

cp template.dmg Installer.dmg

hdiutil attach Installer.dmg

sleep 1

cp -r ~/Desktop/Release/Standalone/Jamtaba2.app /Volumes/Jamtaba_2_Installer/
cp -r ~/Desktop/Release/VstScanner/VstScanner /Volumes/Jamtaba_2_Installer/Jamtaba2.app/Contents/MacOS


hdiutil detach /Volumes/Jamtaba_2_Installer


hdiutil convert ./Installer.dmg -quiet -format UDZO -imagekey zlib-level=9 -o ~/Desktop/Jamtaba_2_Installer.dmg

rm Installer.dmg

