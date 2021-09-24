INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/xprocesswidget.ui

HEADERS += \
    $$PWD/xprocesswidget.h

SOURCES += \
    $$PWD/xprocesswidget.cpp

!contains(XCONFIG, xprocess) {
    XCONFIG += xprocess
    include(../Xprocess/xprocess.pri)
}

!contains(XCONFIG, xpe) {
    XCONFIG += xpe
    include(../Formats/xpe.pri)
}
