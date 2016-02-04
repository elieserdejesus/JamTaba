#!/bin/bash

#stop the script execution in errors
set -e

#call installer builder script to compile 64 bits
sh ./build_installer.sh /usr/local/Qt-5.5.1/bin
