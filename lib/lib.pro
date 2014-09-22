! include( ../common.pri ) {
    error( Couldnt find the common.pri file! )
}

TEMPLATE = lib

DEFINES += QJVMHELPER_LIBRARY

TARGET = ../../bin/RvJNIHelper

QT       -= gui

SOURCES += \
    rvjvm.cpp \
    rvjvmhelper.cpp \
    rvjvmobject.cpp \
    rvjvmthreadscope.cpp

HEADERS +=\
    rvjvm.h \
    rvjvmhelper.h \
    rvjvmhelper_global.h \
    rvjvmobject.h \
    rvjvmthreadscope.h

win32 {
    INCLUDEPATH += "C:/Program Files/Java/jdk1.7.0_65/include"
    INCLUDEPATH += "C:/Program Files/Java/jdk1.7.0_65/include/win32"

    LIBS += advapi32.lib
}

