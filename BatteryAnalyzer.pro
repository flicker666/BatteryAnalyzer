QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = BatteryAnalyzer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    batterydata.cpp \
    heatmapwidget.cpp \
    curvewidget.cpp \
    statisticspanel.cpp

HEADERS += \
    mainwindow.h \
    batterydata.h \
    heatmapwidget.h \
    curvewidget.h \
    statisticspanel.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
