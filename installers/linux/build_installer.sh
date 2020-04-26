#!/bin/bash

#stop the script execution in errors
set -e

if [ $# -lt 1 ] ; then
	echo "The first parameter (the Qt base dir) is empty"	
	exit
fi

if [ $# -lt 2 ] ; then
	echo "The second parameter (the Qt LIBs dir) is empty"	
	exit
fi

scriptDir=$(pwd)
#check if the script is running from the correct dir. Just checking if a expected file is in the folder.
if [ ! -f "$scriptDir/installer_script.sh" ]; then
	echo "Please run this script from the script root folder."
	exit
fi

qtDir=$1 #the first parameter is the Qt base path
qtLibDir=$2 #the second parameter is Qt LIBs path

#check if qt dir exists and abort if not
if [ ! -d "$qtDir" ]; then
	echo "qt not founded in " $qtDir
	exit
fi

projectsDir=$(pwd)/../../PROJECTS

destDir=~/Desktop/release

#make dir if not exists
if [ ! -d "$destDir" ]; then
	echo "Creating the dir " $destDir
	mkdir $destDir
fi

chmod 777 $destDir

cd $destDir

echo "Creating makefile ..."
#-spec linux-g++-32
$qtDir/bin/qmake -config release $projectsDir/Jamtaba.pro 

echo "generating translation (.qm) files..."
$qtDir/bin/lrelease $projectsDir/Jamtaba.pro

echo "Compiling..."
make -s -Wno-reorder -j 4
echo "Compilation finished!"

if [ ! -d "packageFiles" ]; then
	echo "Creating the dirs packageFiles/platforms"
	mkdir -p 'packageFiles/platforms'
fi

echo "Copying jamtaba files to 'packageFiles' dir"
cp Standalone/Jamtaba2 packageFiles/
cp Standalone/VstScanner packageFiles/
cp $scriptDir/Jamtaba2.desktop packageFiles/
cp $scriptDir/installer_script.sh packageFiles/
cp $scriptDir/Jamtaba2.sh packageFiles/
cp $scriptDir/uninstaller.sh packageFiles/
cp $scriptDir/Jamtaba2.png packageFiles/Jamtaba2.png
echo "Done!"

echo "Copying Qt distribution files from $qtLibDir to 'packageFiles'" 
cp $qtLibDir/libQt5Widgets.so.* packageFiles/
cp $qtLibDir/libQt5Gui.so.* packageFiles/ 
cp $qtLibDir/libQt5Core.so.* packageFiles/
cp $qtLibDir/libQt5Network.so.* packageFiles/
cp $qtLibDir/libQt5Multimedia.so.* packageFiles/
cp $qtLibDir/libQt5WebSockets.so.* packageFiles/
cp $qtLibDir/libQt5DBus.so.* packageFiles/
cp $qtLibDir/libQt5XcbQpa.so.* packageFiles/
cp $qtLibDir/libicui18n.so.* packageFiles/
cp $qtLibDir/libicuuc.so.* packageFiles/
cp $qtLibDir/libicudata.so.* packageFiles/
cp $qtLibDir/qt5/plugins/platforms/libqxcb.so packageFiles/platforms


chmod +x packageFiles/installer_script.sh 


#the installer file name will change in differen arch (32 or 64 bits)
installerPrefix="Jamtaba2_Installer"
arch=$(getconf LONG_BIT)
installerSuffix=".run"

installerName=$installerPrefix"_"$arch"bits"$installerSuffix

makeself.sh packageFiles ~/Desktop/$installerName "Jamtaba 2 Installer" ./installer_script.sh
sudo chmod +x ~/Desktop/$installerName
