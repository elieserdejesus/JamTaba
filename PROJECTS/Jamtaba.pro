TEMPLATE = subdirs

SUBDIRS += VstScanner

SUBDIRS += Standalone

include(../translations/translations.pri)

win32{
    SUBDIRS += VstPlugin  #VstPlugin need Qt static build
}

mac{
    SUBDIRS += AUScanner  # audio unit plugins scanner
}

CONFIG += ordered #without 'ordered' QtCreator is freezing during compiling. Possibily something realted with multicore compiling.

#SUBDIRS += ThemeEditor
