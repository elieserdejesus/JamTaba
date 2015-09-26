~/Qt5.5.0/5.5/clang_64/bin/macdeployqt ~/Desktop/jamtaba2/build-Jamtaba2-Desktop_Qt_5_5_0_clang_64bit-Release/Jamtaba2.app -verbose=2

cp template.dmg Installer.dmg

hdiutil attach Installer.dmg

sleep 1

cp -r ~/Desktop/jamtaba2/build-Jamtaba2-Desktop_Qt_5_5_0_clang_64bit-Release/Jamtaba2.app /Volumes/Jamtaba_2_Installer/


hdiutil detach /Volumes/Jamtaba_2_Installer


hdiutil convert ./Installer.dmg -quiet -format UDZO -imagekey zlib-level=9 -o ~/Desktop/Jamtaba_2_Installer.dmg

rm Installer.dmg

