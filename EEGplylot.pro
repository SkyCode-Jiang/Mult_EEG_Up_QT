QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

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
#    MyComboBox.cpp \
    datastruct/eegdata.cpp \
    device/devicemicroeeg.cpp \
#    edflib.c \
#    eegstart.cpp \
#    eegdata.cpp \
    extern/edflib.c \
    extern/qcustomplot.cpp \
#    fir_filter.cpp \
    main.cpp \
    mainwindow.cpp \
#    mymessagebox.cpp \
#    qcustomplot.cpp \
#    qttcustomplot.cpp \
#    switchcontrol.cpp \
    qcptooltip.cpp \
    tool/bdfdata.cpp \
    tool/filtertool.cpp \
    tool/fir_filter.cpp \
    ui/ComboBoxWidget/MyComboBox.cpp \
    ui/EEGDataPlotWidget/qttcustomplot.cpp \
    ui/FilterConfigWidget/wavemainwindow.cpp \
    ui/MessboxWidget/mymessagebox.cpp \
    ui/UartConfigWidget/eegstart.cpp \
    ui/UartConfigWidget/switchcontrol.cpp \
#    wavemainwindow.cpp

HEADERS += \
    datastruct/eegdata.h \
    device/devicemicroeeg.h \
#    edflib.h \
#    eegstart.h \
    extern/edflib.h \
    extern/qcustomplot.h \
#    fir_filter.h \
    mainwindow.h \
#    mymessagebox.h \
#    qcustomplot.h \
#    qttcustomplot.h \
 #    switchcontrol.h \
    qcptooltip.h \
    tool/bdfdata.h \
    tool/filtertool.h \
    tool/fir_filter.h \
    tool/include.h \
    ui/ComboBoxWidget/MyComboBox.h \
    ui/EEGDataPlotWidget/qttcustomplot.h \
    ui/FilterConfigWidget/wavemainwindow.h \
    ui/MessboxWidget/mymessagebox.h \
    ui/UartConfigWidget/eegstart.h \
    ui/UartConfigWidget/switchcontrol.h \
#    wavemainwindow.h



FORMS += \
    mainwindow.ui \
#    mymessagebox.ui \
    ui/FilterConfigWidget/wavemainwindow.ui \
    ui/MessboxWidget/mymessagebox.ui \
    ui/UartConfigWidget/eegstart.ui \
#    wavemainwindow.ui


#LIBS += C:\Users\32802\Desktop\EEGplylot\build-EEGplylot-Desktop_Qt_5_14_2_MinGW_64_bit-Release\libfftw3-3.dll

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc
