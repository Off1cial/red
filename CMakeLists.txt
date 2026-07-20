cmake_minimum_required(VERSION 3.10)

project(red VERSION 1.0 LANGUAGES C CXX)

add_compile_definitions(_POSIX_C_SOURCE=200112L)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(SDL3IMAGE REQUIRED sdl3-image)
pkg_check_modules(SDL3TTF REQUIRED sdl3-ttf)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------------
# Shared source files
# -------------------------

file(GLOB_RECURSE SOURCE_SHARED
    src/corebase/*.c
    src/corebase/*.cpp
)

# -------------------------
# Client sources
# -------------------------

file(GLOB_RECURSE SOURCE_CLIENT
    src/client/*.c
    src/client/*.cpp
    src/engine/*.c
    src/engine/*.cpp
)

file(GLOB_RECURSE SOURCE_CLIENT_PLATFORM
    src/platform/*.c
    src/platform/*.cpp
)

# -------------------------
# Server sources
# -------------------------

file(GLOB_RECURSE SOURCE_SERVER
    src/server/*.c
    src/server/*.cpp
    src/platform/network/*.c
    src/platform/network/*.cpp
    
)

# -------------------------
# ImGui sources
# -------------------------

set(IMGUI_SRC
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_impl_sdl3.cpp
    ${CMAKE_SOURCE_DIR}/include/imgui/imgui_impl_opengl3.cpp
)

# -------------------------
# Client executable
# -------------------------

add_executable(client
    ${IMGUI_SRC}
    ${SOURCE_SHARED}
    ${SOURCE_CLIENT_PLATFORM}
    ${SOURCE_CLIENT}
)

target_compile_definitions(client PRIVATE
    CLIENT_BUILD
)

# -------------------------
# Server executable
# -------------------------

add_executable(server
    ${SOURCE_SHARED}
    ${SOURCE_SERVER}
)

target_compile_definitions(server PRIVATE
    SERVER_BUILD
)

# -------------------------
# Common include paths
# -------------------------

target_include_directories(client PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# -------------------------
# Client libraries
# -------------------------

target_include_directories(client PRIVATE
    ${SDL3_INCLUDE_DIRS}
    ${SDL3IMAGE_INCLUDE_DIRS}
    ${SDL3TTF_INCLUDE_DIRS}
)

target_link_directories(client PRIVATE
    ${SDL3_LIBRARY_DIRS}
    ${SDL3IMAGE_LIBRARY_DIRS}
    ${SDL3TTF_LIBRARY_DIRS}
)

target_link_libraries(client PRIVATE
    ${SDL3_LIBRARIES}
    ${SDL3IMAGE_LIBRARIES}
    ${SDL3TTF_LIBRARIES}
)

if(WIN32)

    target_link_libraries(client PRIVATE
        opengl32
    )

elseif(UNIX)

    target_link_libraries(client PRIVATE
        GL
        m
        stdc++
    )

endif()

# -------------------------
# Server libraries
# -------------------------

target_include_directories(server PRIVATE
    ${SDL3_INCLUDE_DIRS}
)

target_link_directories(server PRIVATE
    ${SDL3_LIBRARY_DIRS}
)

target_link_libraries(server PRIVATE
    ${SDL3_LIBRARIES}
    m
    stdc++
)

# -------------------------
# Compiler settings
# -------------------------

set_target_properties(client server PROPERTIES
    LINKER_LANGUAGE CXX
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
)

# -------------------------
# Address sanitizer
# -------------------------

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)

    foreach(target client server)

        target_compile_options(${target} PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
            -g
        )

        target_link_options(${target} PRIVATE
            -fsanitize=address
        )

    endforeach()

endif()

# -------------------------
# Debug output
# -------------------------

message(STATUS "SDL3 Include Dir: ${SDL3_INCLUDE_DIRS}")
message(STATUS "SDL3 Libraries: ${SDL3_LIBRARIES}")

message(STATUS "SDL3_image Include Dir: ${SDL3IMAGE_INCLUDE_DIRS}")
message(STATUS "SDL3_image Libraries: ${SDL3IMAGE_LIBRARIES}")

message(STATUS "SDL3_ttf Include Dir: ${SDL3TTF_INCLUDE_DIRS}")
message(STATUS "SDL3_ttf Libraries: ${SDL3TTF_LIBRARIES}")
