#!/bin/bash

qmakeDir=/usr/local/Qt-5.5.1/bin

scriptDir=$(pwd)

echo "Compiling Jamtaba..."
projectsDir=$(pwd)/../../PROJECTS

destDir=~/Desktop/release

#make dir if not exists
if [ ! -d "$destDir" ]; then
	echo "Creating the dir " $destDir
	mkdir $destDir
	chmod $destDir 777
fi

cd $destDir

echo "Creating makefile ..."
$qmakeDir/qmake -config release $projectsDir/Jamtaba.pro 

echo "Compiling..."
make -j 4

if [ ! -d "packageFiles" ]; then
	echo "Creating the dir packageFiles"
	mkdir packageFiles
fi

cp Standalone/Jamtaba2 packageFiles/
cp VstScanner/VstScanner packageFiles/
cp $scriptDir/Jamtaba2.desktop packageFiles/
cp $scriptDir/installer_script.sh packageFiles/
cp $scriptDir/uninstaller.sh packageFiles/
cp $scriptDir/Jamtaba2.png packageFiles/Jamtaba2.png

chmod +x packageFiles/installer_script.sh 

makeself.sh packageFiles ~/Desktop/Jamtaba2_Installer.run "Jamtaba 2 Installer" ./installer_script.sh
chmod +x ~/Desktop/Jamtaba2_Installer.run
