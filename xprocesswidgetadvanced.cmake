include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../XProcess/xprocess.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../FormatWidgets/allformatwidgets.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../XProcessMemoryMapWidget/xprocessmemorymapwidget.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../XProcessModulesWidget/xprocessmoduleswidget.cmake)

set(XPROCESSWIDGETADVANCED_SOURCES
    ${XPROCESS_SOURCES}
    ${ALLFORMATWIDGETS_SOURCES}
    ${XPROCESSMEMORYMAPWIDGET_SOURCES}
    ${XPROCESSMODULESWIDGET_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogdumpprocessmemory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdumpprocessmemory.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogdumpprocessmemory.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialoghandleinfoprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialoghandleinfoprocess.h
    ${CMAKE_CURRENT_LIST_DIR}/xhandleinfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xhandleinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/xprocesswidgetadvanced.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xprocesswidgetadvanced.h
    ${CMAKE_CURRENT_LIST_DIR}/xprocesswidgetadvanced.ui
)
