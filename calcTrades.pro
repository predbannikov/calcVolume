#-------------------------------------------------
#
# Project created by QtCreator 2018-03-30T16:52:53
#
#-------------------------------------------------

QT       += core gui network widgets

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = calcTrades
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    binacpp.cpp

HEADERS += \
        mainwindow.h \
    global.h \
    binacpp.h

FORMS += \
        mainwindow.ui

#LIBS += -lta_lib
#LIBS += -L/usr/lib -lssl -lcrypto

#QMAKE_LFLAGS_DEBUG += -s

QMAKE_CXXFLAGS_DEBUG += -O0

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_cd
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_cdd

#INCLUDEPATH += $$PWD/include
#DEPENDPATH += $$PWD/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_cdr
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_cdr

#INCLUDEPATH += $$PWD/include
#DEPENDPATH += $$PWD/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_cm
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_cmd

#INCLUDEPATH += $$PWD/include
#DEPENDPATH += $$PWD/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_cs
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_csd

#INCLUDEPATH += $$PWD/include
#DEPENDPATH += $$PWD/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_csr
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_csr

#INCLUDEPATH += $$PWD/include
#DEPENDPATH += $$PWD/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_cmr
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lta_libc_cmr

#INCLUDEPATH += $$PWD/include
#DEPENDPATH += $$PWD/include
