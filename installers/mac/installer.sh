set -e

mkdir -p ~/Desktop/Release

cd ~/Desktop/Release

echo “Compiling …”

~/Qt/5.7/clang_64/bin/qmake ~/Desktop/JamTaba/PROJECTS/Jamtaba.pro -r -spec macx-clang CONFIG+=x86_64

#compile the translations file
~/Qt/5.7/clang_64/bin/lrelease ~/Desktop/JamTaba/PROJECTS/Jamtaba.pro


/usr/bin/make -s -j 4


~/Qt/5.7/clang_64/bin/macdeployqt ~/Desktop/Release/Standalone/Jamtaba2.app -verbose=2

#copy the VstScanner inside Jamtaba2.app
cp ~/Desktop/Release/VstScanner/VstScanner ~/Desktop/Release/Standalone/Jamtaba2.app/Contents/MacOS/	


#compiling the AU plugin (this is a Xcode project, not a Qmake project)
xcodebuild -parallelizeTargets -jobs 4 -target JamTaba -project ~/Desktop/JamTaba/PROJECTS/AUPlugin/JamTaba.xcodeproj build

#IMPORTANT: the AUPlugin project is running a post build script and copying JamTaba.component to /Library/Audio/Plug-Ins/Components


# node-appdmg search the files considering .json relative paths. Putting the .app file in the node-appdmg .json folder

cp -R ~/Desktop/Release/Standalone/Jamtaba2.app ~/Desktop/JamTaba/installers/mac/
cp -R /Library/Audio/Plug-Ins/Components/JamTaba.component ~/Desktop/JamTaba/installers/mac

appdmg /Users/elieser/Desktop/JamTaba/installers/mac/node-appdmg.json ~/Desktop/Jamtaba_2_Installer.dmg


#removing the .app and AU files
rm -rf ~/Desktop/JamTaba/installers/mac/Jamtaba2.app
rm -rf ~/Desktop/JamTaba/installers/mac/JamTaba.component