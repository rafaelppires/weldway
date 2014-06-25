
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
           src/granite/vsd_cmmd.cpp \
           src/keypress_manager.cpp \
           src/debug_protocol.cpp \
           src/protocol.cpp \
           src/trajectories/trajectory_executer.cpp \
           src/trajectories/trajectory.cpp \
           src/trajectories/triangular.cpp \
           src/trajectories/e_trajectory.cpp \
           src/trajectories/double_e.cpp \
           src/trajectories/double8.cpp \
           src/trajectories/bricks.cpp \
           src/trajectories/double_triang.cpp \
           src/trajectories/double_triang2nd.cpp

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
           include/protocol.h \
           include/vectorxd.h \
           include/keypress_manager.h \
           include/debug_protocol.h \
           include/Matrix.h \
           include/trajectories/trajectory.h \
           include/trajectories/trajectory_executer.h \
           include/trajectories/switch_back.h \
           include/trajectories/triangular.h \
           include/trajectories/e_trajectory.h \
           include/trajectories/double_e.h \
           include/trajectories/rhombus.h \
           include/trajectories/double8.h \
           include/trajectories/bricks.h \
           include/trajectories/double_triang.h \
           include/trajectories/double_triang2nd.h

FORMS    += ui/mainwindow.ui \
            ui/formconnection.ui

LIBS += -L$$(DEV_DEPS)/boost_1_55_0/stage/lib \
        inpout32.dll -lsetupapi \
        -lboost_system-mgw48-mt-1_55 \
        -lboost_thread-mgw48-mt-1_55 \
        -lboost_chrono-mgw48-mt-1_55

INCLUDEPATH += $$(DEV_DEPS)/boost_1_55_0 include include/trajectories ui

RESOURCES += \
    resources.qrc
