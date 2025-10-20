cmake_minimum_required(VERSION 3.10)
set(project "lv_image_converter")
set(libproject "image_converter")
project(${project})


set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#add_subdirectory(libpng)

####################################
add_library(${libproject} SHARED
    lv_image_converter.h 
    lv_image_converter.cpp
    )
target_include_directories(${libproject} PRIVATE ${CMAKE_SOURCE_DIR} 
    /usr/include/
    /usr/local/src/ 
    /usr/local/src/libheif/libheif/api/
    /usr/local/src/libheif/build/
    ${CMAKE_SOURCE_DIR}/smart_pointer
    )
target_link_directories(${libproject} PRIVATE /usr/local/lib)

if(NOT TARGET utils::utils)
    set(utils_DIR "${CMAKE_PREFIX_PATH}")
    find_package(utils REQUIRED CONFIG)
endif()

target_link_libraries(${libproject} PRIVATE utils::utils ssl crypto png jpeg heif)

####################################
add_executable(${project} main.cpp)
target_include_directories(${project} PRIVATE ${CMAKE_SOURCE_DIR} 
    /usr/include/
    /usr/local/src/ 
    /usr/local/src/libheif/libheif/api/
    /usr/local/src/libheif/build/
    ${CMAKE_SOURCE_DIR}/
    )
target_link_directories(${project} PRIVATE /usr/local/lib)
target_link_libraries(${project} PRIVATE ${libproject} ssl crypto png jpeg heif)
