INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/xprocessdialoghex.ui \
    $$PWD/xprocessdialogsystemstructs.ui \
    $$PWD/xprocesswidget.ui

HEADERS += \
    $$PWD/xprocessdialoghex.h \
    $$PWD/xprocessdialogsystemstructs.h \
    $$PWD/xprocesswidget.h

SOURCES += \
    $$PWD/xprocessdialoghex.cpp \
    $$PWD/xprocessdialogsystemstructs.cpp \
    $$PWD/xprocesswidget.cpp

!contains(XCONFIG, xprocess) {
    XCONFIG += xprocess
    include(../XProcess/xprocess.pri)
}

!contains(XCONFIG, xprocess) {
    XCONFIG += xprocess
    include(../XProcess/xprocess.pri)
}

!contains(XCONFIG, qhexview) {
    XCONFIG += xprocess
    include(../QHexView/qhexview.pri)
}

!contains(XCONFIG, xpe) {
    XCONFIG += xpe
    include(../Formats/xpe.pri)
}

!contains(XCONFIG, xwinsystemwidget) {
    XCONFIG += xwinsystemwidget
    include(../XWinSystemWidget/xwinsystemwidget.pri)
}

