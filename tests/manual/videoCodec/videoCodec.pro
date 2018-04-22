QT += core gui widgets multimedia multimediawidgets

CONFIG += c++11

TARGET = TestVideoCodec
CONFIG -= app_bundle

INCLUDEPATH += "../../../libs/includes/ffmpeg"
INCLUDEPATH += ../../../src/Common/video

VPATH += ../../../src/Common

LIBS += -L"$$PWD/../../../libs/static/win64-msvc/" -lavcodec -lavutil -lavformat -lswscale -lswresample -lx264

TEMPLATE = app

SOURCES += main.cpp
SOURCES += video/FFMpegDemuxer.cpp
SOURCES += video/FFMpegMuxer.cpp
SOURCES += MainWindow.cpp

HEADERS += video/FFMpegMuxer.h
HEADERS += video/FFMpegCommon.h
HEADERS += video/FFMpegDemuxer.h
HEADERS += MainWindow.h
