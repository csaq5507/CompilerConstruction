#mC Compiler

This repository holds a *getting started* code-base for the [compiler construction course](https://github.com/W4RH4WK/UIBK-703602-Compiler-Construction).

A more detailed overview can be found in [`doc/overview.md`](doc/overview.md).

## Input Language

- see [mC Specification](https://github.com/W4RH4WK/UIBK-703602-Compiler-Construction/blob/master/mC_specification.md)
- see [examples](doc/examples)

## Prerequisites

- [Meson](http://mesonbuild.com/) in a recent version (`0.44.0`)
  (you may want to install it via `pip3 install --user meson`)
- [Ninja](https://ninja-build.org/)
- `time`, typically located at `/usr/bin/time`, do not confuse with the Bash built-in
- `flex` for generating the lexer
- `bison` for generating the parser
- a compiler supporting C11 (and C++14 for unit tests) -- typically GCC or Clang

## Building and Testing

First, get the source code.

    $ git clone https://github.com/W4RH4WK/mCc.git
    $ cd mCc

Next, generate the build directory.
[Google Test](https://github.com/google/googletest) is used for unit testing.
Meson downloads and builds it automatically if it is not found on your system.

    $ meson builddir
    $ cd builddir

Meson creates Ninja build files by default.
Let's build.

    $ ninja

Unit tests can be run directly with Ninja (or Meson).

    $ ninja test

For integration testing we pass all example inputs to the compiler and observe its exit code.
This is automated by a Bash script.

    $ ../test/integration

Furthermore, a few micro-benchmarks are provided.

    $ ninja benchmark

## Known Issues

- expressions are evaluated the wrong direction(right to left) so we need parenthesis on some examples to execute them correctly



- executing Parser-unit tests (./ut_parser) causes a valgrind error on the "check_syntax_error_msg" test. More information about it in the parser.cpp
- int[4] = int[5] error is not recognized as wrong assignment
- executing empty program results in a segmentation fault
- some invalid program input leads to memory leaks e.g. running 1 + 2; 
- i think function calls like ackermann(n+1,m) (computations or array access inside funktion call) is the only thin that doesn't work in the code generation, temp = n+1; ackermann(temp,m) should work
- task 4 is not done (no time left)
- task 6 is not done (no time left)
- no unit tests for this assignment (no time left)

## Group Work
Please take into account that our third team member (which left our group) was not able to help us in any part of the project. She was not able to write a single line of code. For this reason for every assignment we got into time trouble since we had to write her tasks shortly before the deadlines and had then no time to refactor our code, to write tests and to make everything working like it should.
