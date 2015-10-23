VERSION = 1.0.0

# Define the preprocessor macro to get the application version in theme editor application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       +=  gui widgets

CONFIG += c++11

TARGET = Jamtaba2-ThemeEditor
TEMPLATE = app

SOURCES += ../src/ThemeEditorMain.cpp

