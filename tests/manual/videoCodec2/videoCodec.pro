QT += core gui testlib

CONFIG += testcase
CONFIG += c++11
TARGET = TestVideoCodec

INCLUDEPATH += "../../../libs/includes/ffmpeg"
INCLUDEPATH += ../../../src/Common/video

VPATH += ../../../src/Common

win:LIBS += -L"$$PWD/../../../libs/static/win64-msvc/" -lavcodec -lavutil -lavformat -lswscale -lswresample -lx264
linux:LIBS += -L"$$PWD/../../../libs/static/linux64/"  -lavformat -lavcodec -lswscale -lavutil -lswresample -lx264

TEMPLATE = app

SOURCES += tst_VideoCodec.cpp

SOURCES += video/FFMpegDemuxer.cpp
SOURCES += video/FFMpegMuxer.cpp

HEADERS += video/FFMpegMuxer.h
HEADERS += video/FFMpegDemuxer.h
