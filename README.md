#mC Compiler

This repository holds a compiler for a Mini C language. The code was created by Kopp Daniel and Waldboth Ivan.

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

    $ git clone https://github.com/csaq5507/CompilerConstruction.git
    $ cd CompilerConstruction

Next, generate the build directory.
[Google Test](https://github.com/google/googletest) is used for unit testing.
Meson downloads and builds it automatically if it is not found on your system.

    $ meson build
    $ cd build

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

With the build two executables are generated. 
The first executable is the mCc. It is the main compiler. The following flags are available:

    ./mCc <FILE> [flags]
    <FILE>                  Input filepath or '-' for stdin
    
    [flags]:
    -t || --tac              Create TAC file and CFG Dot File
    -g || --graph            Create graph file in DOT format
    -o || --output <FILE>    Output filepath or '-' for 

The second executable is the mC_cfg_to_dot program. 
It prints for each function of an input file the corresponding CFG in DOT format
The executable can be executed with the following command:

    ./mC_cfg_to_dot <FILE>
    <FILE>                  Input filepath
    

## Known Issues

###Issue #1

For some unknown reason the build gives some strange compiler warning:
    
    mCc@sha/scanner.c: In function ‘yy_get_next_buffer’:
    mCc@sha/scanner.c:689:18: warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
       for ( n = 0; n < max_size && \
                      ^
    mCc@sha/scanner.c:1350:3: note: in expansion of macro ‘YY_INPUT’
       YY_INPUT( (&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move]),
       ^~~~~~~~

The warning are generated inside the scanner.c file which is build by bison and flex.

### Issue #2

By parsing the file in some cases a malloc is lost. 
The following code example produces such a memleak:

    void main(){
        int a;
        int b
        a = b;
    }
    
Where in this example no memleak occurs:

    void main(){
        int a
        int b;
        a = b;
    }
    
The reason for that is that in the lexer a malloc is done which is never forwarded to the parser.y file.

### Issue #3

For some reason by running 'valgrind' over ninja test some unknown errors are printed. 
This errors are always there even if the tests are empty. This seams to be a problem of the google test.
By running the test with the mCc binary no other memory leaks are shown if they are not listed inside this Known Issues section.

### Issue #4

Our parser does not support certain expressions e.g.:
    
    a < b || b == 10
    
For such an expression our parser will not take into account that a < b has a stronger bounding then the ||    

This will throw an semantic error since there is an or between a bool and an integer (b || (b==10))
To get a correct result the expression must be changed:

    (a < b) || (b == 10)

### Issue #5

For some reason the statement:

    pi = pi + sign * kth(i);
    
in leibniz.mC is not working. For this we changed the code to:

    float ret;
    ret = kth(i);
    pi = pi + sign * ret;


