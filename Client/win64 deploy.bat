@echo OFF

SET DEPLOY_DIR=..\deploy-win64

mkdir %DEPLOY_DIR%

SET QT_DIR=C:\Qt\Qt5.5.0-64\5.5\msvc2013_64\bin

REM run qmake, clean and compile the release version

REM %QT_DIR%\qmake.exe ./Jamtaba2.pro -r -spec win32-msvc2013
REM %QT_DIR%\..\..\..\Tools\QtCreator\bin\jom.exe clean
REM %QT_DIR%\..\..\..\Tools\QtCreator\bin\jom.exe -j 4
REM        %QT_DIR%\..\..\..\Tools\QtCreator\bin\jom.exe -f Makefile.Release

SET SSL_LIBS_DIR=C:\OpenSSL-Win64

SET BUILD_DIR=E:\Jamtaba2\build-64bit\release

SET ROBOCOPY_OPTIONS=/NFL /NDL /NJH /NJS /nc /ns /np

robocopy %BUILD_DIR% %DEPLOY_DIR% %ROBOCOPY_OPTIONS% Jamtaba2.exe
robocopy %QT_DIR% %DEPLOY_DIR% %ROBOCOPY_OPTIONS% Qt5Core.dll
robocopy %QT_DIR% %DEPLOY_DIR% %ROBOCOPY_OPTIONS% Qt5Gui.dll
robocopy %QT_DIR% %DEPLOY_DIR% %ROBOCOPY_OPTIONS% Qt5Network.dll
robocopy %QT_DIR% %DEPLOY_DIR% %ROBOCOPY_OPTIONS% Qt5Widgets.dll

robocopy %SSL_LIBS_DIR% %DEPLOY_DIR% %ROBOCOPY_OPTIONS% ssleay32.dll
robocopy %SSL_LIBS_DIR% %DEPLOY_DIR% %ROBOCOPY_OPTIONS% libeay32.dll

robocopy %QT_DIR%\..\plugins\platforms %DEPLOY_DIR%\platforms %ROBOCOPY_OPTIONS% qwindows.dll
robocopy %QT_DIR%\..\plugins\bearer %DEPLOY_DIR%\bearer %ROBOCOPY_OPTIONS% qgenericbearer.dll
robocopy %QT_DIR%\..\plugins\bearer %DEPLOY_DIR%\bearer %ROBOCOPY_OPTIONS% qnativewifibearer.dll

robocopy %QT_DIR%\..\plugins\imageformats %DEPLOY_DIR%\imageformats %ROBOCOPY_OPTIONS% qgif.dll

SET MSVC_REDIST_DIR="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x64\Microsoft.VC120.CRT"

robocopy %MSVC_REDIST_DIR% %DEPLOY_DIR% %ROBOCOPY_OPTIONS% msvcp120.dll
robocopy %MSVC_REDIST_DIR% %DEPLOY_DIR% %ROBOCOPY_OPTIONS% msvcr120.dll



