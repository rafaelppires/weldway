
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WeldWay
TEMPLATE = app

SOURCES += src/main.cpp \
           src/simple_motion_interface.cpp \
           ui/mainwindow.cpp \
           ui/slider_spin.cpp \
           src/units.cpp \
           ui/formconnection.cpp \
           src/master_communicator.cpp \
           src/parallel_port.cpp \
           src/parallel_protocol.cpp \
           src/granite/sm_consts.cpp \
           src/granite_spi_interface.cpp \
           src/switch_back.cpp \
           src/triangular.cpp \
           src/matrix_trajectory.cpp \
           src/double_e.cpp \
           src/e_trajectory.cpp \
           src/rhombus.cpp \
           src/granite/vsd_cmmd.cpp \
           src/keypress_manager.cpp

HEADERS += ui/mainwindow.h \
           include/simplemotion.h \
           include/simple_motion_interface.h \
           ui/slider_spin.h \
           include/units.h \
           ui/formconnection.h \
           include/master_communicator.h \
           include/parallel_port.h \
           include/parallel_protocol.h \
           include/granite/vsd_cmd.h \
           include/granite/simplemotion_private.h \
           include/ftdi/ftd2xx.h \
           include/granite_spi_interface.h \
           include/trajectory.h \
           include/switch_back.h \
           include/protocol.h \
           include/triangular.h \
           include/matrix_trajectory.h \
           include/e_trajectory.h \
           include/double_e.h \
           include/vector2d.h \
           include/rhombus.h \
           include/keypress_manager.h

FORMS    += ui/mainwindow.ui \
            ui/formconnection.ui

LIBS += -L$$(DEV_DEPS)/boost_1_54_0/stage/lib \
        SimpleMotion.dll inpout32.dll -lsetupapi \
        -lboost_system-mgw48-mt-1_54 \
        -lboost_thread-mgw48-mt-1_54 \
        -lboost_chrono-mgw48-mt-1_54

INCLUDEPATH += $$(DEV_DEPS)/boost_1_54_0 include ui

RESOURCES += \
    resources.qrc
