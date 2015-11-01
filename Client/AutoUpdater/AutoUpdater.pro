#I'm stuck: how separated all assets in GitHub Releases?
    #If the AutoUpdater will download the assets is necessary download the correct ones.
    #For example, a win 32 user need only the win 32 assets. So we need release
    #all assets for all Jamtaba variants (Standalone.exe [x86 and x64], VST Plugin DLL [x86, x64] and the Mac .dmg)
    #This is a lot of stuff to upload, but is possible.
    #How we can identify these things using the GitHub REST API?
        #Change the file names to include arch (x86, x64) is a possibility, but
        #this will mess up the installers?
    #Maybe create separated releases? A x86 release, a x64 and a Mac release?
        #This option is insteresting because if we fix some in Mac we can just release a new mac version.

#Another possibility is just release the installers and try run the installer in /SILENT.
    #Possible problems:
        #installer will be marked as malware (because is using /SILENT). Maybe  use a custom parameter to InnoSetup
            #to hide the installer windows?
        #Installer can't install the AutoUpdater.exe, because this .exe is opened and running the installer.


#TODO get the assets of the latest release using GitHub REST API

#TODO write a script to use the GitHub API and post the assets

QT       += core gui widgets network

TARGET = AutoUpdater
TEMPLATE = app


SOURCES += src\autoUpdaterMain.cpp \
    src\AutoUpdaterDialog.cpp \
    src\AutoUpdater.cpp \
    src\WindowsExecutableVersionExtractor.cpp \
    src\Downloader.cpp \
    ..\src\log\logging.cpp \


HEADERS  += \
    src\AutoUpdaterDialog.h \
    src\AutoUpdater.h \
    src\WindowsExecutableVersionExtractor.h \
    src\Downloader.h \
    ..\src\log\logging.h \


FORMS    += \
    src\AutoUpdaterDialog.ui

CONFIG += openssl-linked

win32{

    win32-msvc*{#all msvc compilers{
        #windows xp compatibility
        QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
    }

    win32-g++{#MinGW compiler
        #supressing some MinGW annoying warnings
        QMAKE_CXXFLAGS_WARN_ON += -Wunused-variable
        QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
    }

    LIBS += -lVersion  #this is the windows lib to get file version infos
}

RESOURCES += \
    src/resources.qrc
