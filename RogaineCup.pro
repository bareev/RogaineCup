#-------------------------------------------------
#
# Project created by QtCreator 2017-05-10T15:05:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RogaineCup
TEMPLATE = app


SOURCES += main.cpp\
        rogainecup.cpp \
    settings.cpp \
    addnewfile.cpp \
    structs.cpp \
    textandmath.cpp \
    editparticipiant.cpp

HEADERS  += rogainecup.h \
    settings.h \
    structs.h \
    addnewfile.h \
    textandmath.h \
    editparticipiant.h

FORMS    += rogainecup.ui \
    settings.ui \
    addnewfile.ui \
    editparticipiant.ui

RESOURCES += \
    resourses.qrc
