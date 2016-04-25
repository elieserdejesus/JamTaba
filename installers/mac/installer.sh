set -e

mkdir -p ~/Desktop/Release

cd ~/Desktop/Release

echo “Compiling …”

"/Users/elieser/Qt5.5.0/5.5/clang_64/bin/qmake" ~/Desktop/JamTaba/PROJECTS/Jamtaba.pro -r -spec macx-clang CONFIG+=x86_64

#compile the translations file
~/Qt5.5.0/5.5/clang_64/bin/lrelease ~/Desktop/JamTaba/PROJECTS/Jamtaba.pro


/usr/bin/make -s -j 4


~/Qt5.5.0/5.5/clang_64/bin/macdeployqt ~/Desktop/Release/Standalone/Jamtaba2.app -verbose=2

#copy the VstScanner inside Jamtaba2.app
cp ~/Desktop/Release/VstScanner/VstScanner ~/Desktop/Release/Standalone/Jamtaba2.app/Contents/MacOS/	

# node-appdmg search the files considering .json relative paths. Putting the .app file in the node-appdmg .json folder

cp -R ~/Desktop/Release/Standalone/Jamtaba2.app ~/Desktop/JamTaba/installers/mac/

appdmg /Users/elieser/Desktop/JamTaba/installers/mac/node-appdmg.json ~/Desktop/Jamtaba_2_Installer.dmg


#removing the .app
rm -rf ~/Desktop/JamTaba/installers/mac/Jamtaba2.app