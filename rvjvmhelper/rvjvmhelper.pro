! include( ../common.pri ) {
    error( Couldnt find the common.pri file! )
}

TEMPLATE = lib

DEFINES += QJVMHELPER_LIBRARY

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
    INCLUDEPATH += "C:/Program Files/Java/jdk-18.0.2/include"
    INCLUDEPATH += "C:/Program Files/Java/jdk-18.0.2/include/win32"


    LIBS += -ladvapi32
}


