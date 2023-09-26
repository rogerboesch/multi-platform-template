
cmake_minimum_required(VERSION 3.22.1)

project(MyMLApp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNIX_HOST -DBUILTIN_MINI_STDLIB")

file(TO_CMAKE_PATH "$ENV{MLSDK}" MLSDK)
list(APPEND CMAKE_MODULE_PATH "${MLSDK}/cmake" "${ANDROID_NDK}/../../mlsdk/cmake")

find_package(MagicLeap REQUIRED)
find_package(MagicLeapAppFramework REQUIRED)

include_directories(base imgui myapp)

set(BASE_SOURCES
    base/rb_base.h
    base/rb_log.c
    base/rb_log.h
    base/rb_keys.h
    base/rb_vtext.c
    base/rb_vtext.h
    imgui/text_editor.cpp
    imgui/text_editor.h
)

set(APP_SOURCES
    myapp/myapp.cpp
    myapp/ml_myapp.cpp
    myapp/ml_myapp.hpp
    myapp/rb_file.cpp
    myapp/rb_file.hpp
)

add_library(MyMLApp SHARED
    main.cpp
    ${BASE_SOURCES}    
    ${APP_SOURCES}    
)

target_link_libraries(MyMLApp
    ML::app_framework
    ML::native_surface
)

if (COMMAND copy_artifacts)
    copy_artifacts(MyMLApp)
endif()
