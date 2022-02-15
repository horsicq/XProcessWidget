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
