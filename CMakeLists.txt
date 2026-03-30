cmake_minimum_required(VERSION 3.15...4.0)

project(PGCS LANGUAGES C CXX)

# cmake -B build -S . -DRAYLIB_PATH=$HOME/libs/raylib
# cmake --build build

# emcmake cmake -B build-web -S . -DRAYLIB_PATH=$HOME/libs/raylib
# cmake --build build-web

# cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# ------------------ Compile with GUI ------------------ #
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
#set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

# User provides path to raylib
if(NOT RAYLIB_PATH)
    message(FATAL_ERROR "Please set -DRAYLIB_PATH=/path/to/raylib")
endif()

add_subdirectory(${RAYLIB_PATH} raylib-build)

add_executable(PGCS)

target_include_directories(PGCS PUBLIC include library/include)

target_link_libraries(PGCS raylib)
target_compile_options(PGCS PRIVATE -g)

target_sources(PGCS PRIVATE
    source/algorithm/Processor.cpp
    source/algorithm/Generator.cpp
    source/algorithm/Cell.cpp
    source/data/Tile.cpp
    source/data/Kernel.cpp
    source/data/Ruleset.cpp
    source/data/Composite.cpp
    source/ui/Cursor.cpp
    source/scene/SceneCoordinator.cpp
    source/scene/SceneSharedData.cpp
    source/scene/SceneGeneration.cpp
    source/scene/SceneTextureSample.cpp
    source/helper/CompressColor.cpp
    source/helper/BitMath.cpp
    source/Main.cpp
)


if(EMSCRIPTEN)
    set_target_properties(PGCS PROPERTIES
        LINK_FLAGS "--preload-file ../assets"
    )

    set(CMAKE_EXECUTABLE_SUFFIX ".js")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_WEB")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_WEB")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASYNCIFY -s MODULARIZE=1 -s EXPORT_ES6=1")
endif()