#!/bin/bash

#stop the script execution in errors
set -e

if [ $# -lt 1 ] ; then
	echo "The first parameter (the Qt base dir) is empty"	
	exit
fi

scriptDir=$(pwd)
#check if the script is running from the correct dir. Just checking if a expected file is in the folder.
if [ ! -f "$scriptDir/installer_script.sh" ]; then
	echo "Please run this script from the script root folder."
	exit
fi

qtDir=$1 #the first parameter is the Qt base path

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
make -s -j 4
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

echo "Copying Qt distribution files from $qtDir to 'packageFiles'" 
cp $qtDir/lib/libQt5Widgets.so.* packageFiles/
cp $qtDir/lib/libQt5Gui.so.* packageFiles/ 
cp $qtDir/lib/libQt5Core.so.* packageFiles/
cp $qtDir/lib/libQt5Network.so.* packageFiles/
cp $qtDir/lib/libQt5DBus.so.* packageFiles/
cp $qtDir/lib/libQt5XcbQpa.so.* packageFiles/
cp $qtDir/lib/libicui18n.so.* packageFiles/
cp $qtDir/lib/libicuuc.so.* packageFiles/
cp $qtDir/lib/libicudata.so.* packageFiles/
cp $qtDir/plugins/platforms/libqxcb.so packageFiles/platforms


chmod +x packageFiles/installer_script.sh 


#the installer file name will change in differen arch (32 or 64 bits)
installerPrefix="Jamtaba2_Installer"
arch=$(getconf LONG_BIT)
installerSuffix=".run"

installerName=$installerPrefix"_"$arch"bits"$installerSuffix

makeself.sh packageFiles ~/Desktop/$installerName "Jamtaba 2 Installer" ./installer_script.sh
chmod +x ~/Desktop/$installerName
