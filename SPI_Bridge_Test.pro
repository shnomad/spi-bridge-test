QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \   
        main.cpp\
        gpiocontrol.cpp \
        common.cpp \
        devices/spi_bridge/ftdi/ft4222.cpp \
        devices/spi_bridge/prolific/pl23d3.cpp \
#       devices/display/sh1106/sh1106.cpp \
#       devices/dac/dac80004/dac80004.cpp \
        devices/dac/dac8562/dac8562.cpp


HEADERS += \
        common.h \
        gpiocontrol.h \
        devices/spi_bridge/ftdi/ft4222.h \
    devices/spi_bridge/prolific/pl23d3.h \
#       devices/display/sh1106/sh1106.h \
#       devices/dac/dac80004/dac80004.h \
        devices/dac/dac8562/dac8562.h

INCLUDEPATH +=/opt/qt5rpi3/sysroot/usr/include \
            +=/opt/qt5rpi3/sysroot/usr/local/include
LIBS += -L/opt/qt5rpi3/sysroot/usr/local/lib -lft4222 \
        -L/opt/qt5rpi3/sysroot/usr/local/lib -lpl23d3

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /home/user/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
