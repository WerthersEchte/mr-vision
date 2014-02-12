cmake_minimum_required (VERSION 2.8.11)

set (CMAKE_PREFIX_PATH "C:/Qt/5.2.1/mingw48_32/")

project ( mrVision CXX )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set( SOURCE
   ${SOURCE}
   core/core.cpp
   core/gui/mainGUI.cpp
)
set( HEADERS
   ${HEADERS}
   core/gui/mainGUI.h
)

set( UI_FILES
   ${UI_FILES}
   core/gui/mainGUI.ui
)

QT5_WRAP_UI( UI_HEADERS
   ${UI_FILES}
)

add_executable( mrVision ${SOURCE} ${HEADERS} ${UI_HEADERS} ${UI_FILES})
target_link_libraries( mrVision ${OpenCV_LIBS} )

qt5_use_modules( mrVision Widgets Core Gui)