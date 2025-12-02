cmake_minimum_required(VERSION 3.10)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) #-fPIC

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
    add_custom_target(python DEPENDS ${PYTHON_LIB_OUTPUT})
    set(${OUT_VAR_PYTHON_LIB_OUTPUT} "${PYTHON_LIB_OUTPUT}" PARENT_SCOPE)
endfunction()

function(buildLvImageConverter OUT_VAR_LV_IMG_DSC_SRC)
    message(STATUS "buildLvImageConverter()")

    set(LV_IMG_DSC_SRC 
        "${CMAKE_BINARY_DIR}/res/img_dsc.cpp" 
        # "${CMAKE_BINARY_DIR}/res/img_dsc.h"
        # "${CMAKE_BINARY_DIR}/lv_image_converter/libimage_converter.so"
    )
    add_subdirectory(lv_image_converter)
    add_custom_command(
        OUTPUT ${LV_IMG_DSC_SRC}
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_BINARY_DIR}/res"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/res"
        # COMMAND patchelf --set-rpath '$ORIGIN/../lib:/usr/local/lib' ${CMAKE_BINARY_DIR}/src/lv_image_converter/libimage_converter.so
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/src/lv_image_converter/libimage_converter.so /usr/local/lib/
        # COMMAND patchelf --set-rpath '$ORIGIN/../lib:/usr/local/lib:${CMAKE_BINARY_DIR}/src/lv_image_converter' ${CMAKE_BINARY_DIR}/bin/lv_image_converter
        #/repos/lv_port_linux/x86-build/bin/lv_image_converter /repos/lv_port_linux/src/res/ /repos/lv_port_linux/x86-build/res/
        COMMAND LD_LIBRARY_PATH=/usr/local/lib ${CMAKE_BINARY_DIR}/bin/lv_image_converter "${CMAKE_SOURCE_DIR}/src/res" "${CMAKE_BINARY_DIR}/res" 
        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/res/.success"
        # osm todo copy ${CMAKE_SOURCE_DIR}/src/res/* to ${CMAKE_BINARY_DIR}/res/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/res ${CMAKE_BINARY_DIR}/res
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        # WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/src/lv_image_converter/"
        # -E env LD_LIBRARY_PATH=/usr/local/lib 
        DEPENDS "${CMAKE_SOURCE_DIR}/src/res" lv_image_converter
        COMMENT "Converting image files to lv format... ${CMAKE_BINARY_DIR}/bin/lv_image_converter ${CMAKE_SOURCE_DIR}/src/res ${CMAKE_BINARY_DIR}/res"
        VERBATIM
    )
    add_custom_target(lv_image_convert DEPENDS ${LV_IMG_DSC_SRC})
    set(${OUT_VAR_LV_IMG_DSC_SRC} "${LV_IMG_DSC_SRC}" PARENT_SCOPE)
    # message(FATAL_ERROR "@@@ BUILT {LV_IMG_DSC_SRC} ${LV_IMG_DSC_SRC}")
endfunction()

function(buildLvglSim PYTHON_LIB)
    message(STATUS "buildLvglSim() PYTHON_LIB:${PYTHON_LIB}")

    set(LELE_WIDGETS
        lelewidgets/leleobject.cpp
        lelewidgets/lelebutton.cpp
        lelewidgets/leleevent.cpp
        lelewidgets/leleimage.cpp
        lelewidgets/lelelabel.cpp
        lelewidgets/lelenullwidget.cpp
        lelewidgets/lelestackview.cpp
        lelewidgets/lelestyle.cpp
        lelewidgets/leletabview.cpp
        lelewidgets/leletextbox.cpp
        lelewidgets/leleview.cpp
        lelewidgets/lelemessagebox.cpp
        lelewidgets/lelewidgetfactory.cpp
    )
    list(TRANSFORM LV_LINUX_BACKEND_SRC PREPEND "${CMAKE_SOURCE_DIR}/")
    set(sources
        python/python_binding.cpp
        python/leleevent.cpp
        python/leleobject.cpp
        python/lelelabel.cpp
        graphics_backend.cpp
        # mainwindow.cpp 
        # chart.cpp 
        ${LELE_WIDGETS}
        ${LV_LINUX_SRC} 
        ${LV_LINUX_BACKEND_SRC} 
        ${LV_IMG_DSC_SRC}
        # ${PYTHON_LIB_OUTPUT}
    )
    set(link_dirs
        ${CMAKE_BINARY_DIR}/src/Python-3.13.3
    )
    set(link_libs
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
        ${PYTHON_LIB_OUTPUT} crypt pthread dl util m
    )
    set(inc_dirs
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src/Python-3.13.3 #pyconfig.h
        ${CMAKE_BINARY_DIR}/src/Python-3.13.3/Include
    )
    set(props
        CMAKE_CXX_STANDARD 17
        CMAKE_CXX_STANDARD_REQUIRED ON
        CMAKE_CXX_EXTENSIONS OFF # Optional: ensures strict C++17 compliance, no GNU extensions
    )

    # message(FATAL_ERROR "@@@ {sources} ${sources}")
    message(STATUS "==================================")
    message(STATUS "@@@ {sources} ${sources}")
    message(STATUS "==================================")
    message(STATUS "@@@ {props} ${props}")
    message(STATUS "==================================")
    message(STATUS "@@@ {link_dirs} ${link_dirs}")
    message(STATUS "==================================")
    message(STATUS "@@@ {link_libs} ${link_libs}")
    message(STATUS "==================================")
    message(STATUS "@@@ {inc_dirs} ${inc_dirs}")
    message(STATUS "==================================")
    # message(FATAL_ERROR "==================================")

    add_library(lele_ui SHARED ${sources})
    set_target_properties(lele_ui PROPERTIES ${props})
    target_link_directories(lele_ui PRIVATE ${link_dirs})
    target_link_libraries(lele_ui PRIVATE ${link_libs})
    target_include_directories(lele_ui PRIVATE ${inc_dirs})
    add_dependencies(lele_ui lv_image_convert python)

    add_executable(lvglsim main.cpp python/python_wrapper.cpp)
    set_target_properties(lvglsim PROPERTIES ${props})
    target_link_directories(lvglsim PRIVATE ${link_dirs})
    target_link_libraries(lvglsim PRIVATE ${link_libs} lele_ui)
    target_include_directories(lvglsim PRIVATE ${inc_dirs})
    add_dependencies(lvglsim lele_ui)

    # file(COPY "config.json" DESTINATION "${CMAKE_BINARY_DIR}/bin" FOLLOW_SYMLINK_CHAIN)
    get_filename_component(PYTHON_DIR "${PYTHON_LIB}" DIRECTORY)
    message(STATUS "PYTHON_DIR: ${PYTHON_DIR}")
    add_custom_target(create_my_symlink ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${PYTHON_DIR}
        ${CMAKE_BINARY_DIR}/bin/Python
        COMMENT "Creating symbolic link: ${CMAKE_BINARY_DIR}/bin/Python -> ${PYTHON_DIR}"
    )
    file(COPY ${CMAKE_SOURCE_DIR}/src/res DESTINATION ${CMAKE_BINARY_DIR}/bin/)
    add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvglsim DEPENDS lvglsim)

endfunction()

function(buildTests)
    add_subdirectory(googletest)
    include_directories(googletest/include googletest)
    add_subdirectory(lelewidgets/tests)
endfunction()

function(runTests)
  add_custom_target(run_tests ALL
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/src/lelewidgets/tests"
        DEPENDS style_tests # Ensure test executable is built before running tests
    )
endfunction()

if(NOT TARGET utils::utils)
    set(utils_DIR "${CMAKE_PREFIX_PATH}")
    find_package(utils REQUIRED CONFIG)
endif()

buildPythonLib(PYTHON_LIB_OUTPUT)
buildLvImageConverter(LV_IMG_DSC_SRC)
buildLvglSim(${PYTHON_LIB_OUTPUT})
buildTests()
runTests()
install(TARGETS lele_ui LIBRARY DESTINATION /usr/local/lib)

# libutils.so => not found
# libdebug_logger.so => not found
# libjson_utils.so => not found
# libimage_converter.so
