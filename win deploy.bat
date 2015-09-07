@echo OFF
REM deploy 32 and 64 bits, zip files,  put the files in drop box, start droppox application

call "win32 deploy.bat"

REM zip win32 files using 7zip
7z a -tzip ..\Jamtaba2-x86.zip ..\deploy-win32\*



call "win64 deploy.bat"

REM zip win64 files using 7zip
7z a -tzip ..\Jamtaba2-x64.zip ..\deploy-win64\*


SET ROBOCOPY_OPTIONS=/NFL /NDL /NJH /NJS /nc /ns /np

SET DROPBOX_LOCAL_FOLDER=C:\Users\elieser\Dropbox\Public\jamtaba\jamtaba2

robocopy .\.. %DROPBOX_LOCAL_FOLDER% %ROBOCOPY_OPTIONS% Jamtaba2-x86.zip
robocopy .\.. %DROPBOX_LOCAL_FOLDER% %ROBOCOPY_OPTIONS% Jamtaba2-x64.zip

REM start dropbox
C:\Users\elieser\AppData\Roaming\Dropbox\bin\Dropbox.exe
