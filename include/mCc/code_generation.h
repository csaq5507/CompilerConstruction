//
// Created by ivan on 23.05.18.
//

#ifndef MINICOMPILER_CODE_GENERATION_H
#define MINICOMPILER_CODE_GENERATION_H

#include <stdio.h>

#include "mCc/ast.h"
#include "ast_visit.h"
#include "tac.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mCc_assembly *mCc_generate_assembly(struct mCc_tac_list *tac, const char * output);

enum instruction{
    MCC_ASSEMBLY_LABEL,
    MCC_ASSEMBLY_DIRECTIVE,
    MCC_ASSEMBLY_MOV,
    MCC_ASSEMBLY_ADD,
    MCC_ASSEMBLY_SUB,
    MCC_ASSEMBLY_PUSH,
    MCC_ASSEMBLY_POP,
    MCC_ASSEMBLY_CALL,
    MCC_ASSEMBLY_LEAVE,
    MCC_ASSEMBLY_RET,
    MCC_ASSEMBLY_CMP,
    MCC_ASSEMBLY_JMP

};

struct mCc_assembly_line{
    struct mCc_assembly_line * prev;
    struct mCc_assembly_line * next;
    enum instruction type;
    char * instruction;
};

struct mCc_assembly{
    int counter;
    struct mCc_assembly_line * head;
};

struct mCc_assembly_line * mCc_binary_assembly(struct mCc_tac_list * tac);



#ifdef __cplusplus
}
#endif
#endif //MINICOMPILER_CODE_GENERATION_H
