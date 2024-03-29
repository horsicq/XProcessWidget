INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += USE_XPROCESS

HEADERS += \
    $$PWD/dialogdumpprocessmemory.h \
    $$PWD/dialoghandleinfoprocess.h \
    $$PWD/xhandleinfo.h \
    $$PWD/xprocesswidgetadvanced.h

SOURCES += \
    $$PWD/dialogdumpprocessmemory.cpp \
    $$PWD/dialoghandleinfoprocess.cpp \
    $$PWD/xhandleinfo.cpp \
    $$PWD/xprocesswidgetadvanced.cpp

FORMS += \
    $$PWD/dialogdumpprocessmemory.ui \
    $$PWD/xprocesswidgetadvanced.ui

!contains(XCONFIG, xprocess) {
    XCONFIG += xprocess
    include($$PWD/../XProcess/xprocess.pri)
}

!contains(XCONFIG, allformatwidgets) {
    XCONFIG += allformatwidgets
    include($$PWD/../FormatWidgets/allformatwidgets.pri)
}

!contains(XCONFIG, xprocessmemorymapwidget) {
    XCONFIG += xprocessmemorymapwidget
    include($$PWD/../XProcessMemoryMapWidget/xprocessmemorymapwidget.pri)
}

!contains(XCONFIG, xprocessmoduleswidget) {
    XCONFIG += xprocessmoduleswidget
    include($$PWD/../XProcessModulesWidget/xprocessmoduleswidget.pri)
}
