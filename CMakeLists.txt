# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.0)



# Options. Turn on with 'cmake -Dmyvarname=ON'.
option(test "Build all tests." OFF) # Makes boolean 'test' available.


# Set the project name
project (MiniCompiler)

find_package(BISON)
find_package(FLEX)
find_package(GTest)

set(INCLUDE include/mCc)
set(BUILD build/mCc@sha)
set(CMAKE_BUILD_TYPE Debug)

set(HEADER_FILES ${INCLUDE}/ast.h ${INCLUDE}/ast_print.h
        ${INCLUDE}/ast_visit.h ${INCLUDE}/parser.h
        ${INCLUDE}/ast_symbol_table.h ${INCLUDE}/ast_semantic_checks.h
        ${INCLUDE}/tac.h ${INCLUDE}/error.h ${INCLUDE}/utils.h ${INCLUDE}/code_generation.h)

set(PARSER_FILES ${BUILD}/parser.tab.c ${BUILD}/parser.tab.h ${BUILD}/scanner.c ${BUILD}/scanner.h)
aux_source_directory(src/ SRC_FILES)

# Add an executable
add_executable(${PROJECT_NAME} ${SRC_FILES} src/bin/mCc.c ${HEADER_FILES} ${PARSER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

if (test)

    # This adds another subdirectory, which has 'project(gtest)'.
    add_subdirectory(subprojects/googletest-release-1.8.0)

    enable_testing()

    # Include the gtest library. gtest_SOURCE_DIR is available due to
    # 'project(gtest)' above.
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    ##############
    # Unit Tests
    ##############
    add_executable(runUnitTests  ${SRC_FILES} test/tac.cpp  ${HEADER_FILES} ${PARSER_FILES})

    # Standard linking to gtest stuff.
    target_link_libraries(runUnitTests gtest gtest_main )
    target_include_directories(runUnitTests PUBLIC include)

    # Extra linking for the project.

    # This is so you can do 'make test' to see all your tests run, instead of
    # manually running the executable runUnitTests to see those specific tests.
    add_test(NAME that-test-I-made COMMAND runUnitTests)

    # You can also omit NAME and COMMAND. The second argument could be some other
    # test executable.
    add_test(that-other-test-I-made runUnitTests)
endif()
