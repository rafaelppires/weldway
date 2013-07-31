
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WeldWay
TEMPLATE = app

SOURCES += src/main.cpp \
           src/simple_motion_interface.cpp \
           ui/mainwindow.cpp \
           ui/slider_spin.cpp \
           src/units.cpp \
           src/parallel.cpp \
           ui/formconnection.cpp
#          $$(DEV_DEPS)/boost_1_53_0/libs/chrono/src/chrono.cpp \
#          $$(DEV_DEPS)/boost_1_53_0/libs/system/src/error_code.cpp

HEADERS  += ui/mainwindow.h \
            include/simplemotion.h \
            include/simple_motion_interface.h \
            ui/slider_spin.h \
            include/units.h \
            include/parallel.h \
            ui/formconnection.h

FORMS    += ui/mainwindow.ui \
            ui/formconnection.ui

LIBS += SimpleMotion.dll -lsetupapi

INCLUDEPATH += C:\Users\Rafael\Documents\Development\boost_1_53_0 include ui #$$(DEV_DEPS)/boost_1_53_0
