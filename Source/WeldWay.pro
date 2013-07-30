
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WeldWay
TEMPLATE = app

SOURCES += src/main.cpp \
           src/simple_motion_interface.cpp \
           ui/mainwindow.cpp \
           ui/slider_spin.cpp \
           src/units.cpp \
           src/parallel.cpp
#          $$(DEV_DEPS)/boost_1_53_0/libs/chrono/src/chrono.cpp \
#          $$(DEV_DEPS)/boost_1_53_0/libs/system/src/error_code.cpp

HEADERS  += ui/mainwindow.h \
            include/simplemotion.h \
            include/simple_motion_interface.h \
            ui/slider_spin.h \
            include/units.h \
            include/parallel.h

FORMS    += ui/mainwindow.ui

LIBS += SimpleMotion.dll -lsetupapi

INCLUDEPATH += $$(DEV_DEPS)/boost_1_53_0 include ui C:\\Qt\\Qt5.0.2\\5.0.2\\mingw47_32\\include\\QtWidgets