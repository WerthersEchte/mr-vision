cmake_minimum_required (VERSION 2.8)

project ( mrVision )

add_subdirectory( "Core" )

add_executable( mrVision ${SOURCE} ${HEADERS} )