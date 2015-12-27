#include "WindowsExecutableVersionExtractor.h"

#include <QFile>
#include <Windows.h>
#include <QDebug>
#include <QFileInfo>
#include <stdexcept>

WindowsExecutableVersionExtractor::WindowsExecutableVersionExtractor(){

}

QString WindowsExecutableVersionExtractor::getVersionString(QString executablePath){
    QFileInfo file(executablePath);
    if(!file.exists()){
        qDebug() << "The current folder is" << QFileInfo(".").absoluteFilePath();
        throw std::runtime_error( QString("Can't find the executable in " + file.absoluteFilePath()).toStdString());
    }
    return extract(file.absoluteFilePath());
}

//this code is from http://stackoverflow.com/questions/940707/how-do-i-programatically-get-the-version-of-a-dll-or-exe-file
QString WindowsExecutableVersionExtractor::extract(QString executablePath){

    LPCWSTR szVersionFile = reinterpret_cast<LPCWSTR>(executablePath.utf16());
    qDebug() << "trying extract version info from " << QString::fromStdWString( szVersionFile);
    DWORD  verHandle = 0;
    UINT   size      = 0;
    LPBYTE lpBuffer  = NULL;
    DWORD  verSize   = GetFileVersionInfoSize( szVersionFile, &verHandle);
    //VS_FIXEDFILEINFO    *pFileInfo          = NULL;

    if (verSize)
    {
        LPSTR verData = new char[verSize];

        if (GetFileVersionInfo( szVersionFile, verHandle, verSize, verData))
        {
            if (VerQueryValue(verData,(LPCWSTR)"\\",(VOID FAR* FAR*)&lpBuffer,&size))
            {
                if (size)
                {
                    VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
                    if (verInfo->dwSignature == 0xfeef04bd)
                    {

                        // Doesn't matter if you are on 32 bit or 64 bit,
                        // DWORD is always 32 bits, so first two revision numbers
                        // come from dwFileVersionMS, last two come from dwFileVersionLS
                        int a  = (verInfo->dwFileVersionMS >> 16 ) & 0xffff;
                        int b = (verInfo->dwFileVersionMS >>  0 ) & 0xffff;
                        int c  = (verInfo->dwFileVersionLS >> 16 ) & 0xffff;
                        //int d = (verInfo->dwFileVersionLS >>  0 ) & 0xffff;
                        return QString::number(a) + "." + QString::number(b) + "." + QString::number(c);
                    }
                }
            }
        }
        delete[] verData;
    }
    else{
        qDebug() << QString::fromStdString(GetLastErrorAsString()) << "PATH: " << QString::fromStdWString(szVersionFile);
        throw std::runtime_error(GetLastErrorAsString());
    }
    return "";
}


//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string WindowsExecutableVersionExtractor::GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = NULL;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}
