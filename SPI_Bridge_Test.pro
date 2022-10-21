QT -= gui
QT += core network

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
        coding_channel.cpp \
#       devices/temperature/ds18b20.cpp \
    devices/adc/ads130e08.cpp \
        gpiosysfs.cpp \
        jsondatahandle.cpp \
        main.cpp\
        controller.cpp \
        afe_control.cpp \
#       gpiocontrol.cpp \
        common.cpp \
        devices/spi_bridge/pl23d3.cpp \
        devices/dac/dac8562.cpp \
        devices/adc/ads8866.cpp \
        devices/adc/ads1120.cpp \
        tcpsocketrw.cpp

HEADERS += \
        buildinfo.h \
        coding_channel.h \
        controller.h \
        afe_control.h \
#       devices/temperature/ds18b20.h \
    devices/adc/ads130e08.h \
        gpiosysfs.h \
        jsondatahandle.h \
#       gpiocontrol.h \
        common.h \
        devices/spi_bridge/pl23d3.h \
        devices/dac/dac8562.h \
        devices/adc/ads8866.h \
        devices/adc/ads1120.h \
        tcpsocketrw.h

INCLUDEPATH +=/opt/debian_imx8mm-var-dart/qt5/sysroot/usr/local/include \
            +=/opt/debian_imx8mm-var-dart/qt5/sysroot/usr/include

#LIBS += -L/opt/debian_imx8mm-var-dart/qt5/sysroot/usr/lib/aarch64-linux-gnu -lhidapi-hidraw

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /home/user/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
