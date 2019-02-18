cmake_minimum_required (VERSION 2.8.11)
cmake_policy(SET CMP0043 NEW)

set(MAIN_NAME visiongui)

project ( ${MAIN_NAME} CXX )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(TENSORFLOW "extern/tensorflow") 

find_package( Qt5Widgets REQUIRED )
find_package( Qt5Core REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5Concurrent REQUIRED )

# find_package( aruco REQUIRED )
set(OpenCV_DIR "D:/tools/opencv/build/x64/vc14/lib")
find_package( OpenCV REQUIRED ) # in vs maybe use nuget
# SET( BOOST_ROOT $ENV(BOOST_ROOT) )
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE( Boost COMPONENTS system regex REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} "C:/Users/roboticlab/Documents/IC Imaging Control 3.4/classlib/include" ${TENSORFLOW})
LINK_DIRECTORIES("C:/Users/roboticlab/Documents/IC Imaging Control 3.4/classlib/x64" ${Boost_LIBRARY_DIRS} ${TENSORFLOW})

set( SOURCE
   ${SOURCE}
   src/main.cpp
   src/gui/visiongui.cpp
   src/gui/cameragui.cpp
   src/gui/networkgui.cpp
   src/gui/markergui.cpp
   src/gui/flowlayout.cpp
   src/core/bot.cpp
   src/core/camera.cpp
   src/core/marker.cpp
   src/core/markerlist.cpp
   src/core/udp_server.cpp
   src/core/detectorsimple.cpp
   src/core/detectorneural.cpp
   src/core/neural.cpp
   src/core/detectedmarker.cpp
   src/core/ill/MarkerDetector.cpp
)
set( HEADERS
   ${HEADERS}
   src/gui/visiongui.h
   src/gui/cameragui.h
   src/gui/networkgui.h
   src/gui/markergui.h
   src/gui/flowlayout.h
   src/core/bot.h
   src/core/camera.h
   src/core/marker.h
   src/core/markerlist.h
   src/core/udp_server.h
   src/core/detector.h
   src/core/detectorsimple.h
   src/core/detectorneural.h
   src/core/neural.h
   src/core/detectedmarker.h
   src/core/ill/MarkerDetector.h
)

set( UI_FILES
   ${UI_FILES}
   src/gui/ui/vision.ui
   src/gui/ui/camera.ui
   src/gui/ui/network.ui
   src/gui/ui/marker.ui
)

QT5_WRAP_UI( UI_HEADERS
   ${UI_FILES}
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable( ${MAIN_NAME} WIN32 ${SOURCE} ${HEADERS} ${UI_HEADERS} ${UI_FILES} )

qt5_use_modules( ${MAIN_NAME} Widgets Core Gui )
target_link_libraries( ${MAIN_NAME} ${OpenCV_LIBS} ${aruco_LIBS} optimized TIS_UDSHL11_x64 debug TIS_UDSHL11d_x64 ${Boost_LIBRARIES} tensorflow)
