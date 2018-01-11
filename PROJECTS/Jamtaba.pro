TEMPLATE = subdirs

include(../translations/translations.pri)

      SUBDIRS += Common # compile the commonlib used by Standalone and VST plugin
      SUBDIRS += VstScanner
      SUBDIRS += Standalone
  mac:SUBDIRS += AUScanner
win32:SUBDIRS += VstPlugin  #VstPlugin need Qt static build

win32:VstPlugin.depends = Common
Standalone.depends = Common

#SUBDIRS += ThemeEditor
