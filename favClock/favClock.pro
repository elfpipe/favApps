include(../rasterwindow/rasterwindow.pri)

# work-around for QTBUG-13496
CONFIG += no_batch

SOURCES += \
    main.cpp

RESOURCES = favClock.qrc

target.path = /qt6-amiga/favClock
INSTALLS += target
