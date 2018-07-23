#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include "mCc/ast.h"
#include "mCc/parser.h"
#include "mCc/ast_symbol_table.h"
#include "mCc/ast_semantic_checks.h"
#include "mCc/tac.h"
#include "mCc/code_generation.h"

TEST(code_generation, is_register)
{
    char reg[] = "reg_43";
    char not_reg[] = "a";
    ASSERT_TRUE(is_register(reg));
    ASSERT_FALSE(is_register(not_reg));
}

TEST(code_generation, get_literal_size) {

    ASSERT_EQ(get_literal_size(MCC_AST_LITERAL_TYPE_STRING),
              4);

    ASSERT_EQ(get_literal_size(MCC_AST_LITERAL_TYPE_INT),
              4);

    ASSERT_EQ(get_literal_size(MCC_AST_LITERAL_TYPE_FLOAT),
              8);

    ASSERT_EQ(get_literal_size(MCC_AST_LITERAL_TYPE_BOOL),
              4);

}

TEST(code_generation, is_builtin) {

    char print[] = "print";
    char print_nl[] = "print_nl";
    char print_int[] = "print_int";
    char print_float[] = "print_float";
    char read_int[] = "read_int";
    char read_float[] = "read_float";
    char other[] = "main";

    ASSERT_TRUE(is_builtin(print));
    ASSERT_TRUE(is_builtin(print_nl));
    ASSERT_TRUE(is_builtin(print_int));
    ASSERT_TRUE(is_builtin(print_float));
    ASSERT_TRUE(is_builtin(read_int));
    ASSERT_TRUE(is_builtin(read_float));
    ASSERT_FALSE(is_builtin(other));
}

TEST(code_generation, label) {
    init_globals();
    new_stack();

    char key[] = "key";
    char value[] = "value";
    set_label(key, value);
    ASSERT_STREQ(get_label(key), value);
    delete_stack();
    free_all_registers();
}

TEST(code_generation, var) {
    init_globals();
    new_stack();

    char value[] = "value";
    set_var(4, value);
    ASSERT_EQ(get_var(value)->size, 4);
    delete_stack();
    free_all_registers();
}

TEST(code_generation, param_var) {
    init_globals();
    new_stack();

    char value[] = "value";
    set_param_var(4, value);
    ASSERT_EQ(get_var(value)->size, 4);
    delete_stack();
    free_all_registers();
}

TEST(code_generation, register_) {
    init_globals();

    char value_1[] = "value_1";
    char value_2[] = "value_2";
    //char value_3[] = "value_3";
    char value_4[] = "value_4";
    char value_5[] = "value_5";
    char value_6[] = "value_6";
    char eax[] = "%eax";
    char ebx[] = "%ebx";
    char st0[] = "%st(0)";
    char st1[] = "%st(1)";
    ASSERT_STREQ(get_register(value_1), eax);
    ASSERT_STREQ(get_register(value_2), ebx);
    ASSERT_STREQ(get_register(value_2), ebx);
/*
    ASSERT_TRUE(has_register(value_1));
    ASSERT_FALSE(has_register(value_3));
*/
    push_float_register(value_4);
    ASSERT_STREQ(get_register(value_4), st0);
    push_float_register(value_6);
    ASSERT_STREQ(get_register(value_6), st0);
    ASSERT_STREQ(get_register(value_4), st1);
    ASSERT_STREQ(pop_float_register(), value_6);
    push_float_register(value_6);
    fxch(value_4);
    ASSERT_STREQ(get_register(value_4), st0);
    ASSERT_STREQ(get_register(value_6), st1);

    ASSERT_STREQ(get_register(value_2), ebx);


    /*swap_register(value_6);
    ASSERT_STREQ(get_register(value_6), st0);
    ASSERT_STREQ(get_register(value_4), st1);
*/
    update_register(value_1, value_5);
    ASSERT_STREQ(get_register(value_5), eax);

    ASSERT_TRUE(is_float(value_4));
    ASSERT_FALSE(is_float(value_5));

    free_all_registers();

}
/**/