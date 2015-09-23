@echo OFF
REM deploy 32 and 64 bits, zip files,  put the files in drop box, start droppox application

echo "calling win32 deploy.bat..."
call "win32 deploy.bat"

REM zip win32 files using 7zip
echo "compacting 32 bit files..."
7z a -tzip ..\Jamtaba2-x86.zip ..\deploy-win32\*

echo "calling win64 deploy.bat..."
call "win64 deploy.bat"

REM zip win64 files using 7zip
echo "compacting 64 bit files..."
7z a -tzip ..\Jamtaba2-x64.zip ..\deploy-win64\*


SET ROBOCOPY_OPTIONS=/NFL /NDL /NJH /NJS /nc /ns /np

SET DROPBOX_LOCAL_FOLDER=C:\Users\elieser\Dropbox\Public\jamtaba\jamtaba2

echo "copying files to drop box folder..."
robocopy .\.. %DROPBOX_LOCAL_FOLDER% %ROBOCOPY_OPTIONS% Jamtaba2-x86.zip
robocopy .\.. %DROPBOX_LOCAL_FOLDER% %ROBOCOPY_OPTIONS% Jamtaba2-x64.zip


REM start dropbox
echo "starting dropbox..."
"C:\Program Files (x86)\Dropbox\Client\DropBox.exe"
