INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += USE_XPROCESS

HEADERS += \
    $$PWD/xprocesswidgetadvanced.h

SOURCES += \
    $$PWD/xprocesswidgetadvanced.cpp

FORMS += \
    $$PWD/xprocesswidgetadvanced.ui

!contains(XCONFIG, xprocess) {
    XCONFIG += xprocess
    include($$PWD/../XProcess/xprocess.pri)
}

!contains(XCONFIG, allformatwidgets) {
    XCONFIG += allformatwidgets
    include($$PWD/../FormatWidgets/allformatwidgets.pri)
}

!contains(XCONFIG, xdynstructswidget) {
    XCONFIG += xdynstructswidget
    include($$PWD/../XDynStructsWidget/xdynstructswidget.pri)
}

!contains(XCONFIG, xprocessmemorymapwidget) {
    XCONFIG += xprocessmemorymapwidget
    include($$PWD/../XProcessMemoryMapWidget/xprocessmemorymapwidget.pri)
}

!contains(XCONFIG, xprocessmoduleswidget) {
    XCONFIG += xprocessmoduleswidget
    include($$PWD/../XProcessModulesWidget/xprocessmoduleswidget.pri)
}
