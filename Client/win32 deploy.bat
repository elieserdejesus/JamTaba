@ECHO=OFF

SET DEPLOY_DIR=..\deploy-win32

mkdir %DEPLOY_DIR%

SET QT_DIR=C:\Qt\Qt5.5.0\5.5\mingw492_32\bin

REM libraries in this mingw dir not depends from msvcrt library, so windows users don't need install visual studio redistributable
SET SSL_LIBS_DIR=C:\Qt\Qt5.5.0\Tools\mingw492_32\opt\bin

SET BUILD_DIR=..\..\build\release


robocopy %BUILD_DIR% %DEPLOY_DIR% Jamtaba2.exe
robocopy %QT_DIR% %DEPLOY_DIR% Qt5Core.dll
robocopy %QT_DIR% %DEPLOY_DIR% Qt5Gui.dll
robocopy %QT_DIR% %DEPLOY_DIR% Qt5Network.dll
robocopy %QT_DIR% %DEPLOY_DIR% Qt5Widgets.dll
robocopy %QT_DIR% %DEPLOY_DIR% libgcc_s_dw2-1.dll
robocopy %QT_DIR% %DEPLOY_DIR% libstdc++-6.dll
robocopy %QT_DIR% %DEPLOY_DIR% libwinpthread-1.dll

robocopy %SSL_LIBS_DIR% %DEPLOY_DIR% ssleay32.dll
robocopy %SSL_LIBS_DIR% %DEPLOY_DIR% libeay32.dll

robocopy .\..    %DEPLOY_DIR% GeoLite2-Country.mmdb

robocopy %QT_DIR%\..\plugins\platforms %DEPLOY_DIR%\platforms qwindows.dll
robocopy %QT_DIR%\..\plugins\bearer %DEPLOY_DIR%\bearer qgenericbearer.dll
robocopy %QT_DIR%\..\plugins\bearer %DEPLOY_DIR%\bearer qnativewifibearer.dll

robocopy %QT_DIR%\..\plugins\imageformats %DEPLOY_DIR%\imageformats qgif.dll




