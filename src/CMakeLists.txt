cmake_minimum_required(VERSION 3.10)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-stdlib=libstdc++ -std=stdc++23")
# clang can use libc++ or libstdc++ (that is what gcc uses)

set(CXX_MODULE_STD ON)
set(LELE_VERSION_MAJOR 1)
set(LELE_VERSION_MINOR 0)

add_compile_definitions(LELE_VERSION_MAJOR=${LELE_VERSION_MAJOR})
add_compile_definitions(LELE_VERSION_MINOR=${LELE_VERSION_MINOR})

set(LV_CONF_INCLUDE_SIMPLE 1)
set(LV_USE_WAYLAND 1)

# set(PYTHON_LIB_NAME "micropython") #https://github.com/micropython/micropython
# set(PYTHON_LIB_MAJOR_MINOR "1.27")
# set(PYTHON_LIB_VERSION "${PYTHON_LIB_MAJOR_MINOR}.0")
# set(PYTHON_LIB_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_LIB_NAME}-${PYTHON_LIB_VERSION}.tar.gz")
# set(PYTHON_LIB_URL "https://github.com/micropython/micropython/archive/refs/tags/v${PYTHON_LIB_VERSION}.tar.gz")

set(PYTHON_LIB_NAME "Python")
set(PYTHON_LIB_MAJOR_MINOR "3.13")
set(PYTHON_LIB_VERSION "${PYTHON_LIB_MAJOR_MINOR}.3")
set(PYTHON_LIB_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_LIB_NAME}-${PYTHON_LIB_VERSION}.tgz")
set(PYTHON_LIB_URL "https://www.python.org/ftp/python/${PYTHON_LIB_VERSION}/${PYTHON_LIB_NAME}-${PYTHON_LIB_VERSION}.tgz")

function(buildPython OUT_VAR_PYTHON_LIB_OUTPUT)
    message(STATUS "buildPython()")

    set(PYTHON_LIB_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_LIB_NAME}-${PYTHON_LIB_VERSION}")
    set(PYTHON_LIB_OUTPUT "${PYTHON_LIB_SOURCE_DIR}/libpython${PYTHON_LIB_MAJOR_MINOR}.so")

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

function(buildFonts OUT_VAR_FONTS_OUTPUT)
    message(STATUS "buildFonts()")

    set(FONTS_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/res/fonts")
    add_custom_command(
        OUTPUT ${FONTS_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -B ${CMAKE_BINARY_DIR}/fonts -S ${CMAKE_CURRENT_SOURCE_DIR}/font -DCMAKE_BUILD_TYPE=Release
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/fonts
        #COMMAND ${CMAKE_BINARY_DIR}/fonts/ttf_to_lvf -i ${CMAKE_CURRENT_SOURCE_DIR}/res/fonts/.ttf/noto -o ${CMAKE_CURRENT_SOURCE_DIR}/res/fonts/.lvf/noto
        COMMENT "Building ${FONTS_OUTPUT}"
        VERBATIM
    )
    add_custom_target(ttf_to_lvf DEPENDS ${FONTS_OUTPUT})
    set(${OUT_VAR_FONTS_OUTPUT} "${FONTS_OUTPUT}" PARENT_SCOPE)
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

function(buildLvglSim 
    PYTHON_LIB
    OUT_VAR_SOURCES
    OUT_VAR_PROPS
    OUT_VAR_LINK_DIRS
    OUT_VAR_LINK_LIBS
    OUT_VAR_INC_DIRS    
    )
    message(STATUS "buildLvglSim() PYTHON_LIB:${PYTHON_LIB}")

    set(LELE_WIDGETS
        lelewidgets/lelebutton.cpp
        lelewidgets/lelebutton.py.cpp
        lelewidgets/leleevent.cpp
        lelewidgets/leleevent.py.cpp
        lelewidgets/lelelabel.cpp
        lelewidgets/lelelabel.py.cpp
        lelewidgets/lelecolorwheel.cpp
        lelewidgets/lelecolorwheel.py.cpp
        lelewidgets/lelemessagebox.cpp
        lelewidgets/lelemessagebox.py.cpp
        lelewidgets/lelenullwidget.cpp
        lelewidgets/leleimage.cpp
        lelewidgets/leleimage.py.cpp
        lelewidgets/leleobject.cpp
        lelewidgets/leleobject.py.cpp
        lelewidgets/lelestackview.cpp
        lelewidgets/lelestyle.cpp
        lelewidgets/lelestyle.py.cpp        
        lelewidgets/leletabview.cpp
        lelewidgets/leletextbox.cpp
        lelewidgets/leleview.cpp
        lelewidgets/lelewidgetfactory.cpp
    )
    list(TRANSFORM LV_LINUX_BACKEND_SRC PREPEND "${CMAKE_SOURCE_DIR}/")
    set(sources
        graphics_backend.cpp
        python/python_binding.cpp
        image_builder/image_builder.cpp
        font/lelefont.cpp
        tr/tr.cpp
        ${LELE_WIDGETS}
        ${LV_LINUX_SRC} 
        ${LV_LINUX_BACKEND_SRC} 
        ${LV_IMG_DSC_SRC}
        # ${PYTHON_LIB_OUTPUT}
    )
    set(link_dirs
        ${CMAKE_BINARY_DIR}/src/Python-${PYTHON_LIB_VERSION}
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
        ${CMAKE_BINARY_DIR}/src/Python-${PYTHON_LIB_VERSION} #pyconfig.h
        ${CMAKE_BINARY_DIR}/src/Python-${PYTHON_LIB_VERSION}/Include
    )
    set(props
        CMAKE_CXX_STANDARD 23
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
    set(${OUT_VAR_SOURCES} "${sources}" PARENT_SCOPE)
    set(${OUT_VAR_PROPS} "${props}" PARENT_SCOPE)
    set(${OUT_VAR_LINK_DIRS} "${link_dirs}" PARENT_SCOPE)
    set(${OUT_VAR_LINK_LIBS} "${link_libs}" PARENT_SCOPE)
    set(${OUT_VAR_INC_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${inc_dirs} PARENT_SCOPE)
    # message(FATAL_ERROR "==================================")

    add_library(lele_ui SHARED ${sources})
    set_target_properties(lele_ui PROPERTIES ${props})
    target_link_directories(lele_ui PRIVATE ${link_dirs})
    target_link_libraries(lele_ui PRIVATE ${link_libs})
    target_include_directories(lele_ui PRIVATE ${inc_dirs})
    add_dependencies(lele_ui lv_image_convert python ttf_to_lvf)

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

function(buildCommaSeparatedString MY_LIST APPEND_PATH OUT_STR)
    set(STR "")
    foreach(item IN LISTS MY_LIST)
        if(APPEND_PATH)
            string(SUBSTRING "${item}" 0 1 first_char)
            if(NOT "${first_char}" STREQUAL "/")
                string(PREPEND item "${CMAKE_CURRENT_SOURCE_DIR}/")
            endif()
            string(LENGTH ${item} item_len)
            if(item_len GREATER 2)
                math(EXPR extension_pos "${item_len} - 2")
                string(SUBSTRING "${item}" ${extension_pos} 2 last_chars)
                if("${last_chars}" STREQUAL ".c")
                    string(APPEND item "pp")
                endif()
            endif()
        endif()
        if(NOT item MATCHES "::")
            set(STR "${STR}
                    '${item}',")
        endif()
    endforeach()
    set(${OUT_STR} "${STR}" PARENT_SCOPE)
endfunction()

function(generateSetupPyFile 
    LVGL_SOURCES_LIST 
    LVGL_PROPS_LIST 
    LVGL_LINK_DIRS_LIST 
    LVGL_LINK_LIBS_LIST 
    LVGL_INC_DIRS_LIST
    )
    message(STATUS "generateSetupPyFile()")

    set(LVGL_MACROS "
                ('LV_CONF_INCLUDE_SIMPLE', '${LV_CONF_INCLUDE_SIMPLE}'),
                ('LV_USE_WAYLAND', '${LV_USE_WAYLAND}'),
                ('LELE_VERSION_MAJOR', '${LELE_VERSION_MAJOR}'),
                ('LELE_VERSION_MINOR', '${LELE_VERSION_MINOR}'),
        ")

    buildCommaSeparatedString("${LVGL_SOURCES_LIST}" TRUE LVGL_SOURCES)
    buildCommaSeparatedString("${LVGL_PROPS_LIST}" FALSE LVGL_PROPS)
    buildCommaSeparatedString("${LVGL_LINK_DIRS_LIST}" TRUE LVGL_LINK_DIRS)
    buildCommaSeparatedString("${LVGL_LINK_LIBS_LIST}" FALSE LVGL_LINK_LIBS)
    buildCommaSeparatedString("${LVGL_INC_DIRS_LIST}" TRUE LVGL_INC_DIRS)

    string(PREPEND LVGL_LINK_LIBS "
                'lele_ui',")
    string(PREPEND LVGL_LINK_DIRS "
                '${CMAKE_CURRENT_SOURCE_DIR}',")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in"
        "${CMAKE_BINARY_DIR}/setup.py"
        @ONLY
    )
endfunction()

function(buildLeleCpython)
    set(PYTHON_LIB_OUTPUT "lele.cpython-314-x86_64-linux-gnu.so")
    cmake_host_system_information(RESULT ncores QUERY NUMBER_OF_LOGICAL_CORES)
    add_custom_command(
        TARGET lele_ui POST_BUILD #will run this command after the specified target is built
        COMMAND CXX=clang++ LD_LIBRARY_PATH=/usr/local/lib python3 ${CMAKE_BINARY_DIR}/setup.py build_ext --verbose --build-lib=/usr/local/lib/ --build-temp=${CMAKE_SOURCE_DIR}/py-build -j ${ncores}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Finished building ${PYTHON_LIB_OUTPUT}"
        DEPENDS ${CMAKE_BINARY_DIR}/src/liblele_ui.so
        VERBATIM
        ALL
    )
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

buildFonts(FONTS_OUTPUT)
buildPython(PYTHON_LIB_OUTPUT)
buildLvImageConverter(LV_IMG_DSC_SRC)
buildLvglSim(
    ${PYTHON_LIB_OUTPUT}
    LVGL_SOURCES
    LVGL_PROPS
    LVGL_LINK_DIRS
    LVGL_LINK_LIBS
    LVGL_INC_DIRS
)
generateSetupPyFile(
    "${LVGL_SOURCES}" 
    "${LVGL_PROPS}"
    "${LVGL_LINK_DIRS}"
    "${LVGL_LINK_LIBS}"
    "${LVGL_INC_DIRS}"
)

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# target_link_options(lele_ui PRIVATE -no-pie) #no-pie is needed to print line number from addr2line
# target_link_options(lvglsim PRIVATE -no-pie) #no-pie is needed to print line number from addr2line
# endif()

# buildLeleCpython() #commented out becasue it is slow. This lets us run py file using python
buildTests()
runTests()
install(TARGETS lele_ui LIBRARY DESTINATION /usr/local/lib)

