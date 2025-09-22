cmake_minimum_required(VERSION 3.10)

function(buildPythonLib OUT_VAR_PYTHON_LIB_OUTPUT)
    message(STATUS "buildPython()")

    set(PYTHON_LIB_NAME "Python")
    set(PYTHON_LIB_MAJOR_MINOR "3.13")
    set(PYTHON_LIB_VERSION "${PYTHON_LIB_MAJOR_MINOR}.3")
    set(PYTHON_LIB_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_LIB_NAME}-${PYTHON_LIB_VERSION}")
    set(PYTHON_LIB_OUTPUT "${PYTHON_LIB_SOURCE_DIR}/libpython${PYTHON_LIB_MAJOR_MINOR}.so")
    set(PYTHON_LIB_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_LIB_NAME}-${PYTHON_LIB_VERSION}.tgz")
    set(PYTHON_LIB_URL "https://www.python.org/ftp/python/${PYTHON_LIB_VERSION}/${PYTHON_LIB_NAME}-${PYTHON_LIB_VERSION}.tgz")

    add_custom_command(
        OUTPUT ${PYTHON_LIB_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR} curl -L "${PYTHON_LIB_URL}" -o "${PYTHON_LIB_ARCHIVE}"
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_COMMAND} -E tar xzf ${PYTHON_LIB_ARCHIVE}
        COMMAND ${CMAKE_COMMAND} -E chdir ${PYTHON_LIB_SOURCE_DIR} ${CMAKE_COMMAND} -E env CFLAGS=-Wno-error=coverage-mismatch ./configure --enable-optimizations --enable-shared
        COMMAND ${CMAKE_COMMAND} -E chdir ${PYTHON_LIB_SOURCE_DIR} make -j
        WORKING_DIRECTORY ${PYTHON_LIB_SOURCE_DIR}
        COMMENT "Building ${PYTHON_LIB_NAME}"
        VERBATIM
    )

    set(${OUT_VAR_PYTHON_LIB_OUTPUT} "${PYTHON_LIB_OUTPUT}" PARENT_SCOPE)
endfunction()

function(buildLvImageConverter OUT_VAR_LV_IMG_DSC_SRC)
    message(STATUS "buildLvImageConverter()")

    set(LV_IMG_DSC_SRC 
        "${CMAKE_BINARY_DIR}/res/img_dsc.cpp" 
        "${CMAKE_BINARY_DIR}/res/img_dsc.h"
        # "${CMAKE_BINARY_DIR}/lv_image_converter/libimage_converter.so"
    )
    add_subdirectory(lv_image_converter)
    add_custom_command(
    OUTPUT ${LV_IMG_DSC_SRC}
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_BINARY_DIR}/res"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/res"
    #/repos/lv_port_linux/x86-build/bin/lv_image_converter /repos/lv_port_linux/res/ /repos/lv_port_linux/x86-build/res/
    COMMAND ${CMAKE_BINARY_DIR}/bin/lv_image_converter "${CMAKE_SOURCE_DIR}/res" "${CMAKE_BINARY_DIR}/res" 
    COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/res/.success"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    DEPENDS "${CMAKE_SOURCE_DIR}/res"
    COMMENT "Converting image files to lv format... ${CMAKE_BINARY_DIR}/bin/lv_image_converter ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res"
    VERBATIM
    )
    add_custom_target(lv_image_convert DEPENDS lv_image_converter ${LV_IMG_DSC_SRC})

    set(${OUT_VAR_LV_IMG_DSC_SRC} "${LV_IMG_DSC_SRC}" PARENT_SCOPE)
endfunction()

if(NOT TARGET utils::utils)
    set(utils_DIR "${CMAKE_PREFIX_PATH}")
    find_package(utils REQUIRED CONFIG)
endif()

function(buildLvglSim)
    message(STATUS "buildLvglSim()")

    set(LELE_WIDGETS
        lelewidgets/lelebase.cpp
        lelewidgets/lelenullwidget.cpp
        lelewidgets/lelelabel.cpp
        lelewidgets/lelebutton.cpp
        lelewidgets/leletextbox.cpp
        lelewidgets/lelestyle.cpp
        lelewidgets/leletabview.cpp
        lelewidgets/lelewidgetfactory.cpp
        lelewidgets/leleview.cpp
        lelewidgets/lelestackview.cpp
    )

    list(TRANSFORM LV_LINUX_BACKEND_SRC PREPEND "${CMAKE_SOURCE_DIR}/")
    add_executable(lvglsim 
        main.cpp 
        # mainwindow.cpp 
        # chart.cpp 
        ${LELE_WIDGETS}
        ${LV_LINUX_SRC} 
        ${LV_LINUX_BACKEND_SRC} 
        ${LV_IMG_DSC_SRC}
        ${PYTHON_LIB_OUTPUT}
    )

    set_target_properties(lvglsim PROPERTIES
        CMAKE_CXX_STANDARD 17
        CMAKE_CXX_STANDARD_REQUIRED ON
        CMAKE_CXX_EXTENSIONS OFF # Optional: ensures strict C++17 compliance, no GNU extensions
    )
    target_link_libraries(lvglsim 
        lvgl_linux 
        lvgl 
        lvgl::examples 
        lvgl::demos 
        lvgl::thorvg 
        m
        pthread 
        ${PKG_CONFIG_LIB}
        utils::utils
        image_converter
    )
    target_include_directories(lvglsim
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )
    # target_compile_options(lvglsim PRIVATE "-fdiagnostics-all-candidates")

    file(COPY "config.json" DESTINATION "${CMAKE_BINARY_DIR}/bin" FOLLOW_SYMLINK_CHAIN)
    add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvglsim DEPENDS lvglsim)

    add_dependencies(lvglsim lv_image_convert)
endfunction()

buildPythonLib(PYTHON_LIB_OUTPUT)
buildLvImageConverter(LV_IMG_DSC_SRC)
buildLvglSim()