#!/bin/bash

#stop the script execution in errors
set -e

if [ $# -lt 1 ] ; then
	echo "The first parameter (the qmake path) is empty"	
	exit
fi

scriptDir=$(pwd)
#check if the script is running from the correct dir. Just checking if a expected file is in the folder.
if [ ! -f "$scriptDir/installer_script.sh" ]; then
	echo "Please run this script from the script root folder."
	exit
fi

qmakeDir=$1 #the first parameter is the qmake path

#check if qmake exists and abort if not
if [ ! -d "$qmakeDir" ]; then
	echo "qmake not founded in " $qmakeDir
	exit
fi

#ask about arch (32 or 64 bits)
#echo "What is the target arch bits (32 or 64)?"
#read arch

#Checking for valid archs
#if (($arch != 32 && $arch != 64)) ; then 
#	echo "The arch " $arch " is invalid!"
#	exit
#fi

#echo "selected arch: " $arch

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
$qmakeDir/qmake -config release $projectsDir/Jamtaba.pro 

echo "generating translation (.qm) files..."
lrelease $projectsDir/Jamtaba.pro

echo "Compiling..."
make -s -j 4

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
