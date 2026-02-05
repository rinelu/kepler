cmake_minimum_required(VERSION 3.20)
project(kepler LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS src/*.c)
file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(kepler
    ${C_SOURCES}
    ${CPP_SOURCES}
)

add_library(raylib STATIC IMPORTED)

set_target_properties(raylib PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_SOURCE_DIR}/thirdparty/raylib/include
)

if(EMSCRIPTEN)
    set(RAYLIB_LIB
        ${CMAKE_SOURCE_DIR}/thirdparty/raylib/lib/wasm/libraylib.a
    )

elseif(WIN32 AND MSVC)
    set(RAYLIB_LIB
        ${CMAKE_SOURCE_DIR}/thirdparty/raylib/lib/win64-msvc/raylib.lib
    )

elseif(WIN32) # MinGW
    set(RAYLIB_LIB
        ${CMAKE_SOURCE_DIR}/thirdparty/raylib/lib/win64-mingw/libraylib.a
    )

elseif(APPLE)
    set(RAYLIB_LIB
        ${CMAKE_SOURCE_DIR}/thirdparty/raylib/lib/macos/libraylib.a
    )

else() # Linux
    set(RAYLIB_LIB
        ${CMAKE_SOURCE_DIR}/thirdparty/raylib/lib/linux/libraylib.a
    )
endif()

set_target_properties(raylib PROPERTIES
    IMPORTED_LOCATION ${RAYLIB_LIB}
)

target_include_directories(kepler PRIVATE
    src
    thirdparty/imgui/include
    thirdparty/raylib-nuklear/include
    thirdparty/rlImGui/include
)

target_link_libraries(kepler PRIVATE
    raylib
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/lib/libimgui.a
    ${CMAKE_SOURCE_DIR}/thirdparty/rlImGui/lib/librlImGui.a
)

if(UNIX AND NOT APPLE AND NOT ANDROID)
    target_link_libraries(kepler PRIVATE
        m dl pthread GL X11
    )
endif()

if(APPLE)
    target_link_libraries(kepler PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework OpenGL"
        "-framework CoreVideo"
    )
endif()

if(WIN32)
    target_link_libraries(kepler PRIVATE
        winmm gdi32 opengl32
    )
endif()

if(EMSCRIPTEN)
    set_target_properties(kepler PROPERTIES
        SUFFIX ".html"
    )
endif()

target_compile_options(kepler PRIVATE
    $<$<CONFIG:Debug>:-ggdb>
)
