cmake_minimum_required(VERSION 3.20)
project(kepler LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE C_SOURCES src/*.c)
file(GLOB_RECURSE CPP_SOURCES src/*.cpp)

add_executable(kepler
    ${C_SOURCES}
    ${CPP_SOURCES})

target_include_directories(kepler PRIVATE
    src
    thirdparty/raylib-5.5_linux_amd64/include
    thirdparty/imgui/include
    thirdparty/rlImGui/include)

target_link_libraries(kepler PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/raylib-5.5_linux_amd64/lib/libraylib.a
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/lib/libimgui.a
    ${CMAKE_SOURCE_DIR}/thirdparty/rlImGui/lib/librlImGui.a
    m
    dl
    pthread
)

if(UNIX AND NOT APPLE)
    target_link_libraries(kepler PRIVATE GL X11)
endif()
