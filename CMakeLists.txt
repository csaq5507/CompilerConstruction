# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.0)

# Set the project name
project (MiniCompiler)

find_package(BISON)
find_package(FLEX)

set(INCLUDE include/mCc)

set(HEADER_FILES ${INCLUDE}/ast.h ${INCLUDE}/ast_print.h
        ${INCLUDE}/ast_visit.h ${INCLUDE}/parser.h)


aux_source_directory(src/ SRC_FILES)

# Add an executable
add_executable(${PROJECT_NAME} ${SRC_FILES} src/bin/mCc.c ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Add an executable
add_executable(mC_test ${SRC_FILES} src/bin/mC_to_dot.c ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include)