# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.0)

# Set the project name
project (MiniCompiler)

find_package(BISON)
find_package(FLEX)

set(INCLUDE include/mCc)
set(BUILD build/mCc@sha)
set(CMAKE_BUILD_TYPE Debug)

set(HEADER_FILES ${INCLUDE}/ast.h ${INCLUDE}/ast_print.h
        ${INCLUDE}/ast_visit.h ${INCLUDE}/parser.h)

set(PARSER_FILES ${BUILD}/parser.tab.c ${BUILD}/parser.tab.h ${BUILD}/scanner.c ${BUILD}/scanner.h)
aux_source_directory(src/ SRC_FILES)

# Add an executable
add_executable(${PROJECT_NAME} ${SRC_FILES} src/bin/mCc.c ${HEADER_FILES} ${PARSER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
