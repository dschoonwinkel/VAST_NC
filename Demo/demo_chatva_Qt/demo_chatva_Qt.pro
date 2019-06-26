#-------------------------------------------------
#
# Project created by QtCreator 2018-05-23T14:49:16
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = demo_chatva_Qt
TEMPLATE = app

INCLUDEPATH += . \
                ../../common \
                ../../VASTsim \
		~/Qt/5.10.1/Src/qtbase/include/QtWidgets/ \
		../../../kodo-rlnc/resolve_symlinks/kodo-core/src \
		../../../kodo-rlnc/resolve_symlinks/endian/src \
		../../../kodo-rlnc/kodo_build/include

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
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

LIBS += -L$$PWD/../../lib/ -lvastsim -lvast -lvastnet -lvastcommon -lACE -lboost_system -lboost_thread -Wl,-Bstatic -L$(HOME)/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic

INCLUDEPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../
DESTDIR = ../../bin
