
cmake_minimum_required(VERSION 3.10)
project(MyApp)

# ==============================================================================
# Include and library pathes
# ==============================================================================

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

# ==============================================================================
# Headers and sources
# ==============================================================================

include_directories(src/sdl ../app/src/main/cpp/base ../app/src/main/cpp/engine3d  ../app/src/main/cpp/engine3d/components ../app/src/main/cpp/lua)

set(BASE_SOURCES
    ../app/src/main/cpp/base/rb_base.h
    ../app/src/main/cpp/base/rb_platform.h
    ../app/src/main/cpp/base/rb_log.c
    ../app/src/main/cpp/base/rb_log.h
    ../app/src/main/cpp/base/rb_keys.h
    ../app/src/main/cpp/base/rb_vtext.c
    ../app/src/main/cpp/base/rb_vtext.h
)

set(PLAYGROUND_SOURCES
    src/rb_sdl.cpp
    src/platform_win.c
    src/platform_mac.c
)

set(APP_SOURCES
    ../app/src/main/cpp/myapp/myapp.cpp
)


# ==============================================================================
# Resource definition
# ==============================================================================

set(ASSET_FILES
    ../app/src/main/assets/myapp.cfg
)

source_group(
    "Assets" FILES ${ASSET_FILES}
)

# ==============================================================================
# Target properties and definitions
# ==============================================================================

set(CMAKE_CXX_STANDARD 17) 

set(TARGET MyApp)

add_executable(${TARGET} ${PLAYGROUND_SOURCES} ${BASE_SOURCES} ${APP_SOURCES} ${ASSET_FILES})

if(APPLE)
    set_target_properties(MyApp PROPERTIES MACOSX_BUNDLE TRUE MACOSX_FRAMEWORK_IDENTIFIER org.cmake.ExecutableTarget RESOURCE "${ASSET_FILES}" )
endif()

# ==============================================================================
# Compile options
# ==============================================================================

if(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /D_CRT_SECURE_NO_WARNINGS)
    target_compile_options(${TARGET} PRIVATE /fp:fast)
else()
#    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(${TARGET} PRIVATE -ffast-math)
	target_compile_features(${TARGET} PUBLIC c_std_99)
endif()

if(UNIX AND NOT APPLE)
    target_compile_options(${TARGET} PRIVATE -D_POSIX_C_SOURCE=200809L)
endif()


# ==============================================================================
# Link libraries
# ==============================================================================

if(WIN32)
    target_link_libraries(MyApp ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
elseif(APPLE)
    target_link_libraries(${TARGET} PRIVATE "-framework Cocoa")
    target_link_libraries(${TARGET} PRIVATE ${SDL2_LIBRARIES})
else()
    target_link_libraries(${TARGET} PRIVATE ${SDL2_LIBRARIES} m X11)
endif()

# ==============================================================================
# IDE support
# ==============================================================================

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${TARGET})

source_group("App" FILES ${APP_SOURCES})
source_group("Playground" FILES ${PLAYGROUND_SOURCES})
source_group("Base" FILES ${BASE_SOURCES})

# ==============================================================================
# macOS apps bundle support
# ==============================================================================

set_target_properties(${TARGET} PROPERTIES
	MACOSX_BUNDLE TRUE
)
