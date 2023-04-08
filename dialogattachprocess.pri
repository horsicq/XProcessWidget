INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += USE_XPROCESS

!contains(XCONFIG, xprocess) {
    XCONFIG += xprocess
    include($$PWD/../XProcess/xprocess.pri)
}

FORMS += \
    $$PWD/dialogattachprocess.ui

HEADERS += \
    $$PWD/dialogattachprocess.h

SOURCES += \
    $$PWD/dialogattachprocess.cpp
