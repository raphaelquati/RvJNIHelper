! include( ../common.pri ) {
    error( Couldnt find the common.pri file! )
}

TEMPLATE = app

TARGET = ../../bin/test

QT       += core
QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle

LIBS += ../bin/RvJNIHelper.lib


SOURCES += main.cpp

OTHER_FILES += \
    Test.java


JAVAC=C:/Program Files (x86)/Java/jdk1.7.0_65/bin/javac.exe

QMAKE_POST_LINK  = $$JAVAC $$PWD/Test.java -d $$OUT_PWD/../bin

