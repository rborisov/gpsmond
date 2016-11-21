cmake_minimum_required(VERSION 2.6)

project (gpsmon)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "2")

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(GPSD REQUIRED)

include_directories(${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}
    )

link_directories(
        )

include(CheckCSourceCompiles)

set(CMAKE_C_FLAGS "-std=gnu99 -Wall") 
set(CMAKE_C_FLAGS_DEBUG "-ggdb -pedantic")

get_target_property(MKDATA_EXE mkdata LOCATION)

set(SOURCES
    main.c
)

add_executable(gpsmon ${SOURCES})
target_link_libraries(gpsmon ${CMAKE_THREAD_LIBS_INIT}
    ${GPSD_LIBRARY})

install(TARGETS gpsmon DESTINATION bin)
