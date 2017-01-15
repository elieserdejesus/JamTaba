set -e

echo "Compiling AU plugin ..."
#compiling the AU plugin (this is a Xcode project, not a Qmake project)
#xcodebuild -project ../../PROJECTS/AUPlugin/JamTaba.xcodeproj -scheme JamTaba clean build

echo "AU plugin compiled!"

#IMPORTANT: the AUPlugin project is running a post build script and copying JamTaba.component to /Library/Audio/Plug-Ins/Components


echo "Compiling Jamtaba Standalone, VstScanner and AUScanner..."

mkdir -p ~/Desktop/Release

cd ~/Desktop/Release

echo “Compiling …”

~/Desktop/Qt-5.6/bin/qmake -config release ~/Desktop/JamTaba/PROJECTS/Jamtaba.pro -r -spec macx-clang CONFIG+=x86_64

#compile the translations file
~/Desktop/Qt-5.6/bin/lrelease ~/Desktop/JamTaba/PROJECTS/Jamtaba.pro


/usr/bin/make -s -j 4



# node-appdmg search the files considering .json relative paths. Putting the .app file in the node-appdmg .json folder

cp -R ~/Desktop/Release/Standalone/Jamtaba2.app ~/Desktop/JamTaba/installers/mac/
cp -R /Library/Audio/Plug-Ins/Components/JamTaba.component ~/Desktop/JamTaba/installers/mac

appdmg /Users/elieser/Desktop/JamTaba/installers/mac/node-appdmg.json ~/Desktop/Jamtaba_2_Installer.dmg


#removing the .app and AU files
rm -rf ~/Desktop/JamTaba/installers/mac/Jamtaba2.app
rm -rf ~/Desktop/JamTaba/installers/mac/JamTaba.component