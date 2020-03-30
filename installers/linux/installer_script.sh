#!/bin/bash

destDir=$(eval echo ~${SUDO_USER})/Jamtaba2

#create the directory and set permissions
install -d -m 0777 $destDir


echo "Installing Jamtaba 2 in " $destDir

#copy all files preserving attributes
cp -rp . $destDir


#change the _DEST_DIR placeholder text in Jamtaba2.desktop file
sed -i "s|_DEST_DIR|$destDir|" Jamtaba2.desktop

#copy the .desktop file to Applications folder
cp Jamtaba2.desktop /usr/share/applications/


#copy the icon
cp Jamtaba2.png /usr/share/icons/

#permissions
chmod 777 $destDir/Jamtaba2
chmod 777 $destDir/VstScanner
chmod 777 $destDir/uninstaller.sh
chmod 777 /usr/share/applications/Jamtaba2.desktop
chmod 777 /usr/share/icons/Jamtaba2.png
