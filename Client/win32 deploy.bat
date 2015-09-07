@ECHO=OFF

SET DEPLOY_DIR=..\deploy-win32

mkdir %DEPLOY_DIR%

SET QT_DIR=C:\Qt\Qt5.5.0\5.5\msvc2013\bin

SET SSL_LIBS_DIR=C:\Qt\Qt5.5.0\Tools\QtCreator\bin

SET BUILD_DIR=..\..\build-x32\release

robocopy %BUILD_DIR% %DEPLOY_DIR% Jamtaba2.exe
robocopy %QT_DIR% %DEPLOY_DIR% Qt5Core.dll
robocopy %QT_DIR% %DEPLOY_DIR% Qt5Gui.dll
robocopy %QT_DIR% %DEPLOY_DIR% Qt5Network.dll
robocopy %QT_DIR% %DEPLOY_DIR% Qt5Widgets.dll
REM robocopy %QT_DIR% %DEPLOY_DIR% libgcc_s_dw2-1.dll
REM robocopy %QT_DIR% %DEPLOY_DIR% libstdc++-6.dll
REM robocopy %QT_DIR% %DEPLOY_DIR% libwinpthread-1.dll

robocopy %SSL_LIBS_DIR% %DEPLOY_DIR% ssleay32.dll
robocopy %SSL_LIBS_DIR% %DEPLOY_DIR% libeay32.dll

robocopy %QT_DIR%\..\plugins\platforms %DEPLOY_DIR%\platforms qwindows.dll
robocopy %QT_DIR%\..\plugins\bearer %DEPLOY_DIR%\bearer qgenericbearer.dll
robocopy %QT_DIR%\..\plugins\bearer %DEPLOY_DIR%\bearer qnativewifibearer.dll

robocopy %QT_DIR%\..\plugins\imageformats %DEPLOY_DIR%\imageformats qgif.dll




