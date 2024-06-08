cmake_minimum_required(VERSION 3.0.0)
project(MaskColorChecker VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message("debug build")
    if(MSVC)
        # Set debug build options for MSVC
        add_compile_options(/DDEBUG_BUILD /MTd)
    else()
        # Set debug build options for non-MSVC
        add_compile_options(-DDEBUG_BUILD -static)
    endif()
else()
    message("release build")
    if(MSVC)
        # Set release build options for MSVC
        add_compile_options(/DRELEASE_BUILD /MT)
        add_link_options(/subsystem:windows /ENTRY:wWinMainCRTStartup)
    else()
        # Set release build options for non-MSVC
        add_compile_options(-DRELEASE_BUILD -static)
    endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
	add_link_options(-mwindows)
endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

find_package(FLTK CONFIG REQUIRED)
include_directories(include/)
include_directories(fltk/)

add_executable(MaskColorChecker src/main.cpp fltk/MaskColorCheckerGUI.cxx "src/Canvas.cpp" "src/Program.cpp" "include/sft.rc")

set(EXE_NAME "Mask Color Checker")

set_target_properties(MaskColorChecker PROPERTIES OUTPUT_NAME ${EXE_NAME})

target_link_libraries(MaskColorChecker PRIVATE fltk fltk_gl fltk_forms fltk_images)