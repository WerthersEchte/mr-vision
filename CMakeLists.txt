cmake_minimum_required (VERSION 2.8.9)

set(CMAKE_PREFIX_PATH "/home/marvin/.Qt5.2.1/5.2.1/gcc/")

project ( mrVision CXX )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package( Qt5Widgets REQUIRED )
find_package( Qt5Core REQUIRED )
find_package( Qt5Gui REQUIRED )

find_package(aruco REQUIRED )

find_package( OpenCV REQUIRED )
FIND_PACKAGE( Boost COMPONENTS system regex REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} )

set( SOURCE
   ${SOURCE}
   core/bot.cpp
   core/core.cpp
   core/udp_server.cpp
   core/marker.cpp
   core/markerlist.cpp
   core/gui/mrvisiongui.cpp
)
set( HEADERS
   ${HEADERS}
   core/bot.h
   core/marker.h
   core/udp_server.h
   core/markerlist.h
   core/gui/mrvisiongui.h
)

set( UI_FILES
   ${UI_FILES}
   core/gui/mrvisiongui.ui
)

QT5_WRAP_UI( UI_HEADERS
   ${UI_FILES}
)

set( CXX_COMPILER_FLAGS
"-std=c++0x"
)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_COMPILER_FLAGS}" )

set( SEQAN_C++11_STANDARD ON)

add_executable( mrVision ${SOURCE} ${HEADERS} ${UI_HEADERS} ${UI_FILES})
qt5_use_modules( mrVision Widgets Core Gui )
target_link_libraries( mrVision ${OpenCV_LIBS} ${aruco_LIBS} /usr/lib/i386-linux-gnu/libdc1394.so ${Boost_LIBRARIES} )

