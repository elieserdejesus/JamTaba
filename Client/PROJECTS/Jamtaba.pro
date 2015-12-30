TEMPLATE = subdirs

SUBDIRS += VstScanner

SUBDIRS += Standalone

win32{
    SUBDIRS += VstPlugin  #VstPlugin need Qt static build
}

#SUBDIRS += ThemeEditor

RESOURCES += \
    ../src/Common/resources/jamtaba.qrc
