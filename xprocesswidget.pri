INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/xprocesswidget.ui

HEADERS += \
    $$PWD/xprocesswidget.h

SOURCES += \
    $$PWD/xprocesswidget.cpp

!contains(XCONFIG, xoptions) {
    XCONFIG += xoptions
    include($$PWD/../XOptions/xoptions.pri)
}

!contains(XCONFIG, xpe) {
    XCONFIG += xpe
    include($$PWD/../Formats/xpe.pri)
}

!contains(XCONFIG, xprocess) {
    XCONFIG += xprocess
    include($$PWD/../XProcess/xprocess.pri)
}
