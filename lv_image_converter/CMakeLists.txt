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
    img_helper.cpp img_helper.h
    png_helper.cpp png_helper.h
    jpeg_helper.cpp jpeg_helper.h
    heif_helper.cpp heif_helper.h
    )
target_include_directories(${libproject} PRIVATE ${CMAKE_SOURCE_DIR} 
    /usr/include/
    /usr/local/src/ 
    /usr/local/src/libheif/libheif/api/
    /usr/local/src/libheif/build/
    ${CMAKE_SOURCE_DIR}/
    )
target_link_directories(${libproject} PRIVATE /usr/local/lib)
target_link_libraries(${libproject} PRIVATE ssl crypto png jpeg heif)

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
