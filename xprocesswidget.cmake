include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../XProcess/xprocess.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../FormatWidgets/allformatwidgets.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../XProcessMemoryMapWidget/xprocessmemorymapwidget.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../XProcessModulesWidget/xprocessmoduleswidget.cmake)

set(XPROCESSWIDGET_SOURCES
    ${XPROCESS_SOURCES}
    ${ALLFORMATWIDGETS_SOURCES}
    ${XPROCESSMEMORYMAPWIDGET_SOURCES}
    ${XPROCESSMODULESWIDGET_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xprocesswidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xprocesswidget.ui
)
