project('mCc', 'c',
    default_options: [
        'buildtype=release',
        'c_std=c11',
        'cpp_std=c++14',
        'warning_level=2'

    ],
    meson_version: '>=0.44.0'
)

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

# ------------------------------------------------------------------ GENERATORS

flex = find_program('flex')
lgen = generator(flex,
                 output: ['@BASENAME@.c', '@BASENAME@.h'],
                 arguments: [ '--outfile=@OUTPUT0@',
                              '--header-file=@OUTPUT1@',
                              '@INPUT@' ])

bison = find_program('bison')
pgen = generator(bison,
                 output: ['@BASENAME@.tab.c', '@BASENAME@.tab.h'],
                 arguments: [ '-Wall',
                              '--output=@OUTPUT0@',
                              '--defines=@OUTPUT1@',
                              '@INPUT@' ])

# --------------------------------------------------------------------- LIBRARY

mCc_inc = include_directories('include')

mCc_src = [ 'src/ast.c',
            'src/ast_print.c',
            'src/ast_symbol_table.c',
            'src/ast_semantic_checks.c',
            'src/ast_visit.c',
            'src/parser.c',
            'src/tac.c',
            'src/error.c',
            'src/utils.c',
           'src/code_generation.c',
            'src/cfg.c',
            lgen.process('src/scanner.l'),
            pgen.process('src/parser.y')]

mCc_lib = library('mCc', mCc_src,
                  c_args: '-D_POSIX_C_SOURCE=200809L',
                  include_directories: mCc_inc,
                  dependencies: m_dep)


# ----------------------------------------------------------------- EXECUTABLES

mCc_exes = [ 'mCc', 'mC_cfg_to_dot' ]


foreach exe : mCc_exes
    executable(exe, 'src/bin/' + exe + '.c',
               include_directories: mCc_inc,
               link_with: mCc_lib,
             dependencies: m_dep)
endforeach

# ------------------------------------------------------------------ UNIT TESTS



gtest = dependency('gtest', fallback: ['gtest', 'gtest_main_dep'])

mCc_uts = [ 'parser', 'ast_symbol_table', 'ast_semantic_checks', 'tac', 'cfg', 'code_generation' ]

foreach ut : mCc_uts
    t = executable('ut_' + ut.underscorify(), 'test/' + ut + '.cpp',
                   include_directories: mCc_inc,
                   link_with: mCc_lib,
                   dependencies: [gtest, m_dep])

    test(ut, t)
endforeach