
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WeldWay
TEMPLATE = app

SOURCES += src\\main.cpp \
        ui\\mainwindow.cpp #\
#        $$(DEV_DEPS)\\boost_1_53_0\\libs\\chrono\\src\\chrono.cpp \
#        $$(DEV_DEPS)\\boost_1_53_0\\libs\\system\\src\\error_code.cpp

HEADERS  += ui\mainwindow.h

FORMS    += ui\mainwindow.ui

#LIBS += SimpleMotion.dll

INCLUDEPATH += $$(DEV_DEPS)\\boost_1_53_0 include ui C:\\Qt\\Qt5.0.2\\5.0.2\\mingw47_32\\include\\QtWidgets