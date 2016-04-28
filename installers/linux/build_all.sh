#!/bin/bash

#stop the script execution in errors
set -e

MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
    sh ./build_installer.sh /usr/local/Qt-5.5.1/bin #compile 64 bits
else
    sh ./build_installer.sh /home/elieser/Qt/5.5/gcc/bin
fi
