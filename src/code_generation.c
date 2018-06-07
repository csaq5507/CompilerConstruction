//
// Created by ivan on 23.05.18.
//

#include <mCc/tac.h>
#include <memory.h>
#include <stdlib.h>
#include <mCc/code_generation.h>
#include <mCc/utils.h>
#include <math.h>
#include <assert.h>

#define MALLOC(ptr,size) 				\
ptr = malloc(size);                		\
if((ptr) == NULL)        				\
{                                   	\
	printf("Malloc failed for %p of"	\
		   "size %li",ptr,(size));  	\
	return NULL;                    	\
}

#define REALLOC(ptr,size) 				\
void * temp = realloc(ptr,size);   		\
if(temp == NULL)     					\
{                                       \
	printf("Realloc failed for %p of"   \
		   "size %li",ptr,(size));      \
	return NULL;                        \
}                                       \
(ptr) = temp;

#define MALLOC_(ptr,size) 				\
ptr = malloc(size);                		\
if((ptr) == NULL)        				\
{                                   	\
	printf("Malloc failed for %p of"	\
		   "size %li",ptr,(size));  	\
	return ;                    	    \
}

#define REALLOC_(ptr,size) 				\
void * temp = realloc(ptr,size);   		\
if(temp == NULL)     					\
{                                       \
	printf("Realloc failed for %p of"   \
		   "size %li",ptr,(size));      \
	return ;                            \
}                                       \
(ptr) = temp;

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))



struct mCc_assembly *mCc_generate_assembly(struct mCc_tac_list *tac) {
    //init vars
    struct mCc_assembly * assembly;
    string_label_idx = 0;
    float_label_idx = 0;
    label_idx = 0;
    MALLOC(label, sizeof(struct labels))
    label->counter=0;
    label->l=NULL;
    MALLOC(registers, sizeof(struct regs))
    registers->eax = NULL;
    registers->ebx = NULL;
    registers->ecx = NULL;
    registers->edx = NULL;
    MALLOC(assembly, sizeof(struct mCc_assembly));
    assembly->counter=0;
    //generate first line
    assembly->head = malloc(sizeof(struct mCc_assembly_line));
    assembly->head->type = MCC_ASSEMBLY_FILE;
    assembly->head->prev=NULL;
    assembly->head->next=NULL;
    assembly->head->instruction = new_string("\t.file\t\"%s\"","test.mC");
    struct mCc_assembly_line * current = assembly->head;
    struct mCc_tac_list * temp_tac = tac;
    builtin = 0;
    //generate labels/builtin functions
    do{
        if(temp_tac->type == MCC_TAC_ELEMENT_TYPE_COPY_LITERAL)
        {
            if(temp_tac->literal_type == MCC_TAC_LITERAL_TYPE_STRING){
                current->next = create_string_label(temp_tac);
                current->next->prev = current;
            }
            if(temp_tac->literal_type == MCC_TAC_LITERAL_TYPE_FLOAT){
                current->next = create_float_label(temp_tac);
                current->next->prev = current;
            }
        }else if(temp_tac->type == MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL)
        {
            if(is_builtin(temp_tac->identifier1))
            {
                current->next = create_builtin_func(temp_tac);
            }
        } else if(temp_tac->type == MCC_TAC_ELEMENT_TYPE_UNARY && temp_tac->unary_op_type == MCC_TAC_OPERATION_TYPE_FAC){
            create_builtin_func(temp_tac);
        }
        temp_tac=temp_tac->next;
        while(current->next!=NULL)
            current=current->next;
    }while (temp_tac!= NULL);
    //generate code
    do{
        struct mCc_assembly_line * temp = NULL;
        switch (tac->type)
        {
            case MCC_TAC_ELEMENT_TYPE_COPY_LITERAL:
                temp = mCc_assembly_copy_literal(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER:
                temp = mCc_assembly_copy_identifier(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_UNARY:
            case MCC_TAC_ELEMENT_TYPE_BINARY:
                temp = mCc_assembly_operation(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP:
                temp = mCc_assembly_jump(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP:
                temp = mCc_assembly_conditional_jump(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_LABEL:
                MALLOC(temp,sizeof(struct mCc_assembly_line))
                temp->type = MCC_ASSEMBLY_LABEL;
                temp->instruction = new_string(".L%d:",label_idx);
                temp->next=NULL;
                set_label(tac->identifier1,new_string(".L%d",label_idx++));
                break;
            case MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP:
                tac=tac->next;
                continue;
                //temp = mCc_assembly_param(tac);
                //break;
            case MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL:
                temp = mCc_procedure_call(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_LOAD:
                break;
            case MCC_TAC_ELEMENT_TYPE_STORE:
                break;
            case MCC_TAC_ELEMENT_TYPE_ADDRESS_ASSIGNMENT:
                break;
            case MCC_TAC_ELEMENT_TYPE_POINTER_ASSIGNMENT:
                break;
            case MCC_TAC_ELEMENT_TYPE_FUNCTION_START:
                temp = mCc_assembly_function_start(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_FUNCTION_END:
                temp = mCc_assembly_function_end(tac);
                delete_stack();
                break;
            case MCC_TAC_ELEMENT_TYPE_RETURN:
                temp = mCc_assembly_function_return(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_UNKNOWN:
                tac=tac->next;
                continue;
        }
        assert(temp);
        temp->prev=current;
        current->next = temp;
        while(current->next != NULL)
            current=current->next;
        tac=tac->next;
    } while(tac!=NULL);
    while(current->prev!= NULL)
        current= current->prev;
    assembly->head = current;
    return assembly;
}

struct mCc_assembly_line *mCc_assembly_param(struct mCc_tac_list *tac) {
    struct mCc_assembly_line *retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line));
    retval->next = NULL;
    retval->type = MCC_ASSEMBLY_SUB;
    retval->instruction = new_string("\tsubl\t$%d, %s",get_literal_size(tac->decl_lit_type),"%esp");

    return retval;
}

struct mCc_assembly_line *mCc_assembly_function_return(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    struct mCc_assembly_line * temp1;

    MALLOC(retval, sizeof(struct mCc_assembly_line))
    MALLOC(temp1, sizeof(struct mCc_assembly_line))
    struct mCc_tac_list * func_end = tac;
    while(func_end->type != MCC_TAC_ELEMENT_TYPE_FUNCTION_END)
        func_end=func_end->next;
    retval->type = MCC_ASSEMBLY_MOV;

    if(tac->identifier1 == NULL) {
        retval->instruction = new_string("\tnop\t");
        if(strcmp(func_end->identifier1,"main") == 0)
        {
            retval->instruction = new_string("\tmovl\t$0, %s","%eax");
        }
    }else
        retval->instruction = new_string("\tmovl\t$%s, %s",tac->identifier1,"%eax");

    retval->next = temp1;
    temp1->prev = retval;
    temp1->next = NULL;
    temp1->type = MCC_ASSEMBLY_JMP;
    temp1->instruction = new_string("\tjmp\t.RET%s",func_end->identifier1);
    return retval;
}

struct mCc_assembly_line *mCc_procedure_call(struct mCc_tac_list *tac) {

    struct mCc_assembly_line * retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    retval->type = MCC_ASSEMBLY_CALL;
    retval->instruction = new_string("\tcall\t%s",tac->identifier1);
    retval->next = NULL;
    return retval;
}

struct mCc_assembly_line *create_builtin_func(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    if(strcmp(tac->identifier1,"print") == 0 && !CHECK_BIT(builtin,0) )
    {
        builtin+=1;
        retval->instruction = "\t.section\t.rodata\n"
                ".LCPRINT:\n"
                "\t.string\t\"%s\"\n"
                "\t.text\n"
                "\t.globl\tprint\n"
                "\t.type\tprint, @function\n"
                "print:\n"
                "\tpushl\t%ebp\n"
                "\tmovl\t%esp, %ebp\n"
                "\tsubl\t$8, %esp\n"
                "\tsubl\t$8, %esp\n"
                "\tpushl\t8(%ebp)\n"
                "\tpushl\t$.LCPRINT\n"
                "\tcall\tprintf\n"
                "\taddl\t$16, %esp\n"
                "\tnop\n"
                "\tleave\n"
                "\tret\n"
                "\t.size\tprint, .-print";
    } else if (strcmp(tac->identifier1,"print_nl") == 0 && !CHECK_BIT(builtin,1)){
        builtin+=2;
        retval->instruction = "\t.globl\tprint_nl\n"
                "\t.type\tprint_nl, @function\n"
                "print_nl:\n"
                "\tpushl\t%ebp\n"
                "\tmovl\t%esp, %ebp\n"
                "\tsubl\t$8, %esp\n"
                "\tsubl\t$12, %esp\n"
                "\tpushl\t$10\n"
                "\tcall\tputchar\n"
                "\taddl\t$16, %esp\n"
                "\tnop\n"
                "\tleave\n"
                "\tret\n"
                "\t.size\tprint_nl, .-print_nl";
    } else if (strcmp(tac->identifier1,"print_int") == 0 && !CHECK_BIT(builtin,2)){
        builtin+=4;
        retval->instruction = "\t.section\t.rodata\n"
                ".LCPRINTINT:\n"
                "\t.string\t\"%ld\"\n"
                "\t.text\n"
                "\t.globl\tprint_int\n"
                "\t.type\tprint_int, @function\n"
                "print_int:\n"
                "\tpushl\t%ebp\n"
                "\tmovl\t%esp, %ebp\n"
                "\tsubl\t$8, %esp\n"
                "\tsubl\t$8, %esp\n"
                "\tpushl\t8(%ebp)\n"
                "\tpushl\t$.LCPRINTINT\n"
                "\tcall\tprintf\n"
                "\taddl\t$16, %esp\n"
                "\tnop\n"
                "\tleave\n"
                "\tret\n"
                "\t.size\tprint_int, .-print_int";
    } else if (strcmp(tac->identifier1,"print_float") == 0 && !CHECK_BIT(builtin,3)){
        builtin+=8;
        retval->instruction = "\t.section\t.rodata\n"
                ".LCPRINTFLOAT:\n"
                "\t.string\t\"%f\"\n"
                "\t.text\n"
                "\t.globl\tprint_float\n"
                "\t.type\tprint_float, @function\n"
                "print_float:\n"
                "\tpushl\t%ebp\n"
                "\tmovl\t%esp, %ebp\n"
                "\tsubl\t$8, %esp\n"
                "\tflds\t8(%ebp)\n"
                "\tsubl\t$4, %esp\n"
                "\tleal\t-8(%esp), %esp\n"
                "\tfstpl\t(%esp)\n"
                "\tpushl\t$.LCPRINTFLOAT\n"
                "\tcall\tprintf\n"
                "\taddl\t$16, %esp\n"
                "\tnop\n"
                "\tleave\n"
                "\tret\n"
                "\t.size\tprint_float, .-print_float";
    } else if (strcmp(tac->identifier1,"read_int") == 0 && !CHECK_BIT(builtin,4)){
        builtin+=16;
        retval->instruction = "\t.globl\tread_int\n"
                "\t.type\tread_int, @function\n"
                "read_int:\n"
                "\tpushl\t%ebp\n"
                "\tmovl\t%esp, %ebp\n"
                "\tsubl\t$24, %esp\n"
                "\tmovl\t$0, -12(%ebp)\n"
                "\tsubl\t$8, %esp\n"
                "\tleal\t-12(%ebp), %eax\n"
                "\tpushl\t%eax\n"
                "\tpushl\t$.LC1\n"
                "\tcall\t__isoc99_scanf\n"
                "\taddl\t$16, %esp\n"
                "\tmovl\t-12(%ebp), %eax\n"
                "\tleave\n"
                "\tret\n"
                "\t.size\tread_int, .-read_int";
    } else if (strcmp(tac->identifier1,"read_float") == 0 && !CHECK_BIT(builtin,5)){
        builtin+=32;
        retval->instruction = "\t.globl\tread_float\n"
                "\t.type\tread_float, @function\n"
                "read_float:\n"
                "\tpushl\t%ebp\n"
                "\tmovl\t%esp, %ebp\n"
                "\tsubl\t$24, %esp\n"
                "\tfldz\n"
                "\tfstps\t-12(%ebp)\n"
                "\tsubl\t$8, %esp\n"
                "\tleal\t-12(%ebp), %eax\n"
                "\tpushl\t%eax\n"
                "\tpushl\t$.LC2\n"
                "\tcall\t__isoc99_scanf\n"
                "\taddl\t$16, %esp\n"
                "\tflds\t-12(%ebp)\n"
                "\tleave\n"
                "\tret\n"
                "\t.size\tread_float, .-read_float";
    } else if(tac->type==MCC_TAC_ELEMENT_TYPE_UNARY && tac->unary_op_type == MCC_TAC_OPERATION_TYPE_FAC && !CHECK_BIT(builtin,6)){
        builtin+=64;
        retval->instruction = "\t.globl\tfactorial_builtin\n"
                "\t.type\tfactorial_builtin, @function\n"
                "factorial_builtin:\n"
                "\tpushl\t%ebp\n"
                "\tmovl\t%esp, %ebp\n"
                "\tsubl\t$8, %esp\n"
                "\tcmpl\t$1, 8(%ebp)\n"
                "\tjle\t.L2\n"
                "\tmovl\t8(%ebp), %eax\n"
                "\tsubl\t$1, %eax\n"
                "\tsubl\t$12, %esp\n"
                "\tpushl\t%eax\n"
                "\tcall\tfactorial_builtin\n"
                "\taddl\t$16, %esp\n"
                "\timull\t8(%ebp), %eax\n"
                "\tjmp\t.L3\n"
                ".L2:\n"
                "\tmovl\t8(%ebp), %eax\n"
                ".L3:\n"
                "\tleave\n"
                "\tret\n"
                "\t.size\tfactorial_builtin, .-factorial_builtin";
    }
    retval->next = NULL;
    retval->type = MCC_ASSEMBLY_BUILTIN;
    return retval;
}

struct mCc_assembly_line *create_string_label(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    struct mCc_assembly_line * temp1;
    struct mCc_assembly_line * temp2;
    MALLOC(temp1, sizeof(struct mCc_assembly_line))
    MALLOC(temp2, sizeof(struct mCc_assembly_line))
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    retval->next = temp1;
    temp1->prev = retval;
    temp1->next = temp2;
    temp2->prev = temp1;
    temp2->next = NULL;
    retval->type = MCC_ASSEMBLY_LABEL;
    retval->instruction = new_string(".LC%d:", string_label_idx);
    set_label(tac->identifier1, new_string(".LC%d",string_label_idx++));
    temp1->type = MCC_ASSEMBLY_CONSTANT;
    temp1->instruction = new_string("\t.string \"%s\"", tac->s_literal);
    temp2->type = MCC_ASSEMBLY_DIRECTIVE;
    temp2->instruction = new_string("\t.text");
    return retval;
}

struct mCc_assembly_line *create_float_label(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    struct mCc_assembly_line * temp1;
    MALLOC(temp1, sizeof(struct mCc_assembly_line))
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    retval->next = temp1;
    temp1->prev = retval;
    temp1->next = NULL;
    retval->type = MCC_ASSEMBLY_LABEL;
    retval->instruction = new_string(".LF%d:", float_label_idx);
    set_label(tac->identifier1, new_string(".LF%d", float_label_idx++));
    temp1->type = MCC_ASSEMBLY_CONSTANT;
    temp1->instruction = new_string("\t.float \"%f\"", tac->f_literal);
    return retval;
}

struct mCc_assembly_line *mCc_assembly_function_end(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    struct mCc_assembly_line * temp1;
    struct mCc_assembly_line * temp2;
    struct mCc_assembly_line * temp3;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    MALLOC(temp1, sizeof(struct mCc_assembly_line))
    MALLOC(temp2, sizeof(struct mCc_assembly_line))
    MALLOC(temp3, sizeof(struct mCc_assembly_line))
    retval->next = temp1;
    temp1->prev = retval;
    temp1->next = temp2;
    temp2->prev = temp1;
    temp2->next = temp3;
    temp3->prev = temp2;
    temp3->next = NULL;
    retval->type = MCC_ASSEMBLY_LABEL;
    retval->instruction = new_string(".RET%s:",tac->identifier1);
    temp1->type = MCC_ASSEMBLY_LEAVE;
    temp1->instruction = new_string("\tleave");
    temp2->type = MCC_ASSEMBLY_RET;
    temp2->instruction = new_string("\tret");
    temp3->type = MCC_ASSEMBLY_DIRECTIVE;
    temp3->instruction = new_string("\t.size\t%s, .-%s", tac->identifier1, tac->identifier1);

    if(strcmp(tac->identifier1,"main") == 0)
    {
        struct mCc_assembly_line * temp4;
        MALLOC(temp4, sizeof(struct mCc_assembly_line))
        temp4->next = retval;
        retval->prev = temp4;
        temp4->type = MCC_ASSEMBLY_MOV;
        temp4->instruction = new_string("\tmovl\t$0, %s","%eax");
        return temp4;
    }
    return retval;
}

struct mCc_assembly_line *mCc_assembly_function_start(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    struct mCc_assembly_line * temp1;
    struct mCc_assembly_line * temp2;
    struct mCc_assembly_line * temp3;
    struct mCc_assembly_line * temp4;
    struct mCc_assembly_line * temp5;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    MALLOC(temp1, sizeof(struct mCc_assembly_line))
    MALLOC(temp2, sizeof(struct mCc_assembly_line))
    MALLOC(temp3, sizeof(struct mCc_assembly_line))
    MALLOC(temp4, sizeof(struct mCc_assembly_line))
    MALLOC(temp5, sizeof(struct mCc_assembly_line))
    retval->next = temp1;
    temp1->prev = retval;
    temp1->next = temp2;
    temp2->prev = temp1;
    temp2->next = temp3;
    temp3->prev = temp2;
    temp3->next = temp4;
    temp4->prev = temp3;
    temp4->next = temp5;
    temp5->prev = temp4;
    temp5->next = NULL;
    retval->type = MCC_ASSEMBLY_DIRECTIVE;
    retval->instruction = new_string("\t.globl\t%s",tac->identifier1);
    temp1->type = MCC_ASSEMBLY_DIRECTIVE;
    temp1->instruction = new_string("\t.type\t%s, @function",tac->identifier1);
    temp2->type = MCC_ASSEMBLY_LABEL;
    temp2->instruction = new_string("%s:",tac->identifier1);
    set_label(tac->identifier1,new_string(tac->identifier1));
    temp3->type = MCC_ASSEMBLY_PUSH;
    temp3->instruction = new_string("\tpushl\t%s","%ebp");
    temp4->type = MCC_ASSEMBLY_MOV;
    temp4->instruction = new_string("\tmovl\t%s, %s","%esp", "%ebp");
    struct mCc_tac_list* temp_tac = tac;
    new_stack();
    while (temp_tac->type!=MCC_TAC_ELEMENT_TYPE_FUNCTION_END)
    {
        if(temp_tac->type == MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP) {
            set_var(get_literal_size(temp_tac->decl_lit_type),temp_tac->identifier1);
        }
        temp_tac=temp_tac->next;
    }
    temp5->type = MCC_ASSEMBLY_SUB;
    temp5->instruction = new_string("\tsubl\t$%d, %s",get_stack_size(),"%esp");
    return retval;
}

struct mCc_assembly_line *mCc_assembly_copy_identifier(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    retval->type = MCC_ASSEMBLY_MOV;
    if(tac->identifier1[0] == 't') {
        retval->instruction = new_string("\tmovl\t-%d(%s), %s",get_var(tac->copy_identifier),"%edp",get_register(tac->identifier1));
    } else {
        retval->instruction = new_string("\tmovl\t%s,-%d(%s)",get_register(tac->copy_identifier),get_var(tac->identifier1),"%edp");
        free_register(tac->copy_identifier);
    }
    retval->next = NULL;
    return retval;
}

struct mCc_assembly_line *mCc_assembly_copy_literal(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    bool is_pushl_copy = check_pushl_copy(tac);
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    switch (tac->literal_type) {
        case (MCC_TAC_LITERAL_TYPE_INT):
            if (is_pushl_copy)
                retval->instruction = new_string("\tpushl $%d", tac->i_literal);
            else {
                retval->instruction = new_string("\tmovl\t$%d %s", tac->i_literal, get_register(tac->identifier1));
            }
            break;
        case (MCC_TAC_LITERAL_TYPE_FLOAT):
            if (is_pushl_copy)
                retval->instruction = new_string("\tpushl $%s", get_label(tac->identifier1));
            else {
                retval->instruction = new_string("\tmovl\t$%s %s", get_label(tac->identifier1), get_register(tac->identifier1));
            }
            break;
        case (MCC_TAC_LITERAL_TYPE_BOOL):
            if (is_pushl_copy)
                retval->instruction = new_string("\tpushl $%d", tac->b_literal ? 1 : 0);
            else {
                retval->instruction = new_string("\tmovl\t$%d %s", tac->b_literal ? 1 : 0,  get_register(tac->identifier1));
            }
            break;
        case (MCC_TAC_LITERAL_TYPE_STRING):
            if (is_pushl_copy)
                retval->instruction = new_string("\tpushl\t$%s", get_label(tac->identifier1));
            else {
                retval->instruction = new_string("\tmovl\t$%s %s", get_label(tac->identifier1),  get_register(tac->identifier1));
            }
            break;
    }
    retval->next = NULL;
    return retval;
}

struct mCc_assembly_line * mCc_assembly_operation(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    int op = (tac->type == MCC_TAC_ELEMENT_TYPE_UNARY) ? tac->unary_op_type : tac->binary_op_type;
    switch(op){
        case MCC_TAC_OPERATION_TYPE_PLUS:
            retval->type= MCC_ASSEMBLY_ADD;
            retval->instruction = new_string("\taddl\t%s %s", get_register(tac->lhs),get_register(tac->rhs));
            retval->next=NULL;
            break;
        case MCC_TAC_OPERATION_TYPE_MINUS:
            retval->type= MCC_ASSEMBLY_SUB;
            retval->instruction = new_string("\tsubl\t%s %s", get_register(tac->lhs),get_register(tac->rhs));
            retval->next=NULL;
            break;
        case MCC_TAC_OPERATION_TYPE_MULTIPLY:
            retval->type= MCC_ASSEMBLY_MUL;
            retval->instruction = new_string("\timull\t%s %s", get_register(tac->lhs),get_register(tac->rhs));
            retval->next=NULL;
            break;
        case MCC_TAC_OPERATION_TYPE_DIVISION:
            retval->type= MCC_ASSEMBLY_DIV;
            retval->instruction = new_string("\tidivl\t%s %s", get_register(tac->lhs),get_register(tac->rhs));
            retval->next=NULL;
            break;
        case MCC_TAC_OPERATION_TYPE_EQ:
        case MCC_TAC_OPERATION_TYPE_NE:
        case MCC_TAC_OPERATION_TYPE_LT:
        case MCC_TAC_OPERATION_TYPE_GT:
        case MCC_TAC_OPERATION_TYPE_LE:
        case MCC_TAC_OPERATION_TYPE_GE:
        case MCC_TAC_OPERATION_TYPE_AND:
        case MCC_TAC_OPERATION_TYPE_OR:
            free(retval);
            return mCc_assembly_condition(tac);
        case MCC_TAC_OPERATION_TYPE_ASSIGNMENT:
            printf("gibs des ibohaup?");
            break;
        case MCC_TAC_OPERATION_TYPE_FAC:
            MALLOC(retval->next, sizeof(struct mCc_assembly_line))
            MALLOC(retval->next->next, sizeof(struct mCc_assembly_line))
            retval->type = MCC_ASSEMBLY_PUSH;
            retval->instruction = new_string("\tpushl\t%s",get_register(tac->identifier1));
            retval->next->type = MCC_ASSEMBLY_CALL;
            retval->next->instruction = new_string("\tcall\t%s","factorial_builtin");
            retval->next->next->type = MCC_ASSEMBLY_MOV;
            retval->next->next->instruction = new_string("\tmovl\t%s, %s","%eax",get_register(tac->identifier1));
            retval->next->prev = retval;
            retval->next->next->prev = retval->next;
            retval->next->next->next = NULL;
            break;
        default:
            printf("error");
            break;
    }
    free_register(tac->lhs);
    update_register(tac->rhs,tac->identifier1);
    return retval;
}

struct mCc_assembly_line *mCc_assembly_condition(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    retval->type = MCC_ASSEMBLY_CMP;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    if (tac->next->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
        jump_cond = tac->binary_op_type;
        retval->instruction = new_string("\tcmpl\t%s, %s",get_register(tac->lhs),get_register(tac->rhs));
    } else {
        struct mCc_tac_list *current = tac;
        bool and=false;
        while(current->type != MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP)
        {
            if(current->type == MCC_TAC_ELEMENT_TYPE_BINARY && current->binary_op_type == MCC_TAC_OPERATION_TYPE_AND)
                and=true;
            current=current->next;
        }
        //TODO NESTED AND/OR Conditions
    }
    retval->next = NULL;
    return retval;
}

struct mCc_assembly_line *mCc_assembly_conditional_jump(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    retval->type=MCC_ASSEMBLY_JMP;
    switch(jump_cond){
        case MCC_TAC_OPERATION_TYPE_EQ:
            retval->instruction = new_string("\tjne\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_NE:
            retval->instruction = new_string("\tje\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_LT:
            retval->instruction = new_string("\tjge\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_GT:
            retval->instruction = new_string("\tjle\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_LE:
            retval->instruction = new_string("\tjg\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_GE:
            retval->instruction = new_string("\tjl\t%s",tac->jump->identifier1);
            break;
        default:
            printf("error");
            break;
    }
    jump_cond = -1;
    retval->next=NULL;
    return retval;
}

struct mCc_assembly_line *mCc_assembly_jump(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    retval->type = MCC_ASSEMBLY_JMP;
    retval->instruction = new_string("\tjmp\t%s",get_label(tac->identifier1));
    retval->next=NULL;
    return retval;
}

void mCc_print_assembly(FILE * out, struct mCc_assembly *ass) {

    struct mCc_assembly_line *current = ass->head;
    while (current != NULL) {
        fprintf(out,"%s\n",current->instruction);
        current = current->next;
    }
}

int get_literal_size(enum mCc_ast_literal_type type) {
    switch (type)
    {
        case MCC_AST_LITERAL_TYPE_STRING:
            return 4;
        case MCC_AST_LITERAL_TYPE_INT:
            return 4;
        case MCC_AST_LITERAL_TYPE_FLOAT:
            return 4;
        case MCC_AST_LITERAL_TYPE_BOOL:
            return 4;
    }
}

bool check_pushl_copy(struct mCc_tac_list *tac) {
    struct mCc_tac_list * current = tac;
    int counter = 0;
    while (current != NULL && current->type != MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL) {
        counter++;
        current = current->next;
    }
    if(current == NULL)
        return false;
    if(current->num_function_param >= counter)
        return true;
    return false;
}

bool is_builtin(char *function) {
    if(strcmp(function,"print") == 0 ||
       strcmp(function,"print_nl") == 0 ||
       strcmp(function,"print_int") == 0 ||
       strcmp(function,"print_float") == 0 ||
       strcmp(function,"read_int") == 0 ||
       strcmp(function,"read_float") == 0)
        return true;
    return false;
}

void set_label(char *key, char *value) {
    for (int i = 0; i < label->counter; ++i) {
        if(strcmp(label->l[i].key,key)==0) {
            label->l[i].value = value;
            return;
        }
    }
    if(label->counter==0) {
        MALLOC_((label->l), (sizeof(struct label_identification)))
    }else
    {
        REALLOC_((label->l),((label->counter+1) * sizeof(struct label_identification)))
    }
    label->l[label->counter].key = key;
    label->l[label->counter].value = value;
    label->counter++;
}

char * get_label(char * key) {
    for (int i = 0; i < label->counter; ++i) {
        if(strcmp(label->l[i].key,key)==0)
            return label->l[i].value;
    }
}

void set_var(int size, char *identifier) {
    for (int i = 0; i < stack->counter; ++i) {
        if(stack->variables[i].identifier == NULL)
            continue;
        if(strcmp(stack->variables[i].identifier,identifier)==0)
            return;
    }
    if(stack->counter==0) {
        MALLOC_((stack->variables), (sizeof(struct variable)))
    }else
    {
        REALLOC_((stack->variables),((stack->counter+1) * sizeof(struct variable)))
    }
    stack->variables[stack->counter].stack_diff = stack->variables[stack->counter - 1].stack_diff + size;
    stack->variables[stack->counter].identifier = identifier;
    stack->counter++;
}

int get_var(char * identifier) {
    for (int i = 0; i < stack->counter; ++i) {
        if(strcmp(stack->variables[i].identifier,identifier)==0)
            return stack->variables[i].stack_diff;
    }
}

void new_stack() {
    MALLOC_(stack, sizeof(struct stack_helper))
    stack->counter = 0;
    stack->variables = NULL;
}

void delete_stack() {
    free(stack->variables);
    free(stack);
}

int get_stack_size() {
    return stack->variables[stack->counter-1].stack_diff;
}

void free_register(char* identifier) {
    if(registers->eax != NULL && strcmp(registers->eax,identifier)==0) {
        registers->eax = NULL;
        return;
    }
    if(registers->ebx != NULL && strcmp(registers->ebx,identifier)==0) {
        registers->ebx = NULL;
        return;
    }
    if(registers->ecx != NULL && strcmp(registers->ecx,identifier)==0) {
        registers->ecx = NULL;
        return;
    }
    if(registers->edx != NULL && strcmp(registers->edx,identifier)==0) {
        registers->edx = NULL;
        return;
    }
}

char* get_register(char* identifier) {
    if(registers->eax != NULL && strcmp(registers->eax,identifier)==0)
        return "%eax";
    if(registers->ebx != NULL && strcmp(registers->ebx,identifier)==0)
        return "%ebx";
    if(registers->ecx != NULL && strcmp(registers->ecx,identifier)==0)
        return "%ecx";
    if(registers->edx != NULL && strcmp(registers->edx,identifier)==0)
        return "%edx";

    if(registers->eax == NULL)
    {
        registers->eax = identifier;
        return "%eax";
    }
    if(registers->ebx == NULL)
    {
        registers->ebx = identifier;
        return "%ebx";
    }
    if(registers->ecx == NULL)
    {
        registers->ecx = identifier;
        return "%ecx";
    }
    if(registers->edx == NULL)
    {
        registers->edx = identifier;
        return "%edx";
    }

    return NULL;
}

void update_register(char *old_identifier, char *new_identifier) {
    if(registers->eax != NULL && strcmp(registers->eax,old_identifier)==0)
        registers->eax = new_identifier;
    if(registers->ebx != NULL && strcmp(registers->ebx,old_identifier)==0)
        registers->ebx = new_identifier;
    if(registers->ecx != NULL && strcmp(registers->ecx,old_identifier)==0)
        registers->ecx = new_identifier;
    if(registers->edx != NULL && strcmp(registers->edx,old_identifier)==0)
        registers->edx = new_identifier;
}