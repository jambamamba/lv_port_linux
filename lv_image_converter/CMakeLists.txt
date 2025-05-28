cmake_minimum_required(VERSION 3.10)
set(project "lv_image_converter")
project(${project})


set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#add_subdirectory(libpng)

add_executable(${project} main.cpp 
    img_helper.cpp img_helper.h
    png_helper.cpp png_helper.h
    jpeg_helper.cpp jpeg_helper.h
    )
target_include_directories(${project} PRIVATE ${CMAKE_SOURCE_DIR} /usr/local/src/)
target_link_directories(${project} PRIVATE /usr/local/lib)
target_link_libraries(${project} PRIVATE png jpeg)
