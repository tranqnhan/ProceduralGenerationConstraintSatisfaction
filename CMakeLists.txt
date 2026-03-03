cmake_minimum_required(VERSION 3.15...4.0)

project(ProceduralGenerationConstraintSatisfaction LANGUAGES C CXX)

# cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# ------------------ Compile with GUI ------------------ #
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib)
add_executable(ProceduralGenerationConstraintSatisfaction)

target_include_directories(ProceduralGenerationConstraintSatisfaction PUBLIC include library/include)

target_link_libraries(ProceduralGenerationConstraintSatisfaction raylib)
target_compile_options(ProceduralGenerationConstraintSatisfaction PRIVATE -O2)

target_sources(ProceduralGenerationConstraintSatisfaction PRIVATE
    source/algorithm/Ruleset.cpp
    source/algorithm/Analyzer.cpp
    source/algorithm/Generator.cpp
    source/Main.cpp
)