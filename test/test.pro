! include( ../common.pri ) {
    error( Couldnt find the common.pri file! )
}

TEMPLATE = app

TARGET = test

QT       += core
QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle

SOURCES += main.cpp

OTHER_FILES += \
    Test.java

JAVAC=C:/Program Files/Java/jdk-18.0.2/bin/javac.exe

win32:CONFIG(release, debug|release): QMAKE_POST_LINK  = $$JAVAC $$PWD/Test.java -d $$OUT_PWD/../test/release/
else:win32:CONFIG(debug, debug|release): QMAKE_POST_LINK  = $$JAVAC $$PWD/Test.java -d $$OUT_PWD/../test/debug/

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rvjvmhelper/release/ -lrvjvmhelper
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rvjvmhelper/debug/ -lrvjvmhelper
else:unix: LIBS += -L$$OUT_PWD/../rvjvmhelper/ -lrvjvmhelper

INCLUDEPATH += $$PWD/../rvjvmhelper
DEPENDPATH += $$PWD/../rvjvmhelper
