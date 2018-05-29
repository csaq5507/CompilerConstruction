//
// Created by ivan on 23.05.18.
//

#include <mCc/tac.h>
#include <memory.h>
#include <mCc/error.h>
#include <stdlib.h>
#include <mCc/code_generation.h>
#include <mCc/utils.h>

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

struct mCc_assembly *mCc_generate_assembly(struct mCc_tac_list *tac, const char * output)
{
    struct mCc_assembly * assembly;
    MALLOC(assembly, sizeof(struct mCc_assembly));
    assembly->counter=0;
    assembly->head = malloc(sizeof(struct mCc_assembly_line));
    assembly->head->type = MCC_ASSEMBLY_DIRECTIVE;
    assembly->head->prev=NULL;
    assembly->head->next=NULL;
    assembly->head->instruction = new_string("\t.file\t\"%s\"",output);
    struct mCc_assembly_line * current = assembly->head;
    do{
        struct mCc_assembly_line * temp;
        switch (tac->type)
        {
            case MCC_TAC_ELEMENT_TYPE_COPY_LITERAL:
                temp = mCc_assembly_copy_literal(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER:
                MALLOC(temp, sizeof(struct mCc_assembly_line));
                temp->instruction = new_string("movl\t%s %s",tac->identifier1,tac->copy_identifier);
                break;
            case MCC_TAC_ELEMENT_TYPE_UNARY:
            case MCC_TAC_ELEMENT_TYPE_BINARY:
                temp = mCc_assembly_operation(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP:
                MALLOC(temp, sizeof(struct mCc_assembly_line))
                temp->instruction = new_string("call\t%s",tac->jump->identifier1);
                break;
            case MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP:
                MALLOC(temp, sizeof(struct mCc_assembly_line))
                temp->instruction = new_string("call\t%s",tac->jump->identifier1);
                break;
            case MCC_TAC_ELEMENT_TYPE_LABEL:
                break;
            case MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP:
                break;
            case MCC_TAC_ELEMENT_TYPE_PROCEDURE_CALL:
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

            MALLOC(temp, sizeof(struct mCc_assembly_line))
                temp->instruction = new_string(".size\t%s, .-%s",tac->identifier1,tac->identifier1);
                break;
            case MCC_TAC_ELEMENT_TYPE_RETURN:
                break;
        }
        temp->prev=current;
        current->next = temp;
        while(current->next != NULL)
            current=current->next;
        tac=tac->next;
    } while(tac!=NULL);
}


struct mCc_assembly_line *mCc_assembly_function_start(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * temp;
    MALLOC(temp, sizeof(struct mCc_assembly_line))
    temp->instruction = new_string(".globl\t%s",tac->identifier1);
    struct mCc_assembly_line * temp1;
    struct mCc_assembly_line * temp2;
    struct mCc_assembly_line * temp3;
    struct mCc_assembly_line * temp4;
    MALLOC(temp1, sizeof(struct mCc_assembly_line))
    MALLOC(temp2, sizeof(struct mCc_assembly_line))
    MALLOC(temp3, sizeof(struct mCc_assembly_line))
    MALLOC(temp4, sizeof(struct mCc_assembly_line))
    temp->next = temp1;
    temp1->prev = temp;
    temp1->next = temp2;
    temp2->prev = temp1;
    temp2->next = temp3;
    temp3->prev = temp2;
    temp3->next = temp4;
    temp4->prev = temp3;
    temp4->next = NULL;
    temp1->instruction = new_string(".type\t%s, @function",tac->identifier1);
    temp2->instruction = new_string("%s:",tac->identifier1);
    temp3->instruction = new_string("pushl\t%ebp");
    temp4->instruction = new_string("movl\t %esp, %ebp");
    return temp;
}


struct mCc_assembly_line *mCc_assembly_copy_literal(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    switch (tac->literal_type){
        case (MCC_TAC_LITERAL_TYPE_INT):
            retval->instruction = new_string("imovl\t%s %s",tac->identifier1,tac->i_literal);
            break;
        case (MCC_TAC_LITERAL_TYPE_FLOAT):
            retval->instruction = new_string("fmovl\t%s %s",tac->identifier1,tac->f_literal);
            break;
        case (MCC_TAC_LITERAL_TYPE_BOOL):
            retval->instruction = new_string("bmovl\t%s %s",tac->identifier1,tac->b_literal);
            break;
        case (MCC_TAC_LITERAL_TYPE_STRING):
            retval->instruction = new_string("smovl\t%s %s",tac->identifier1,tac->s_literal);
            break;
    }
    return retval;
}

struct mCc_assembly_line * mCc_assembly_operation(struct mCc_tac_list *tac)
{
    struct mCc_assembly_line * retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    int op = (tac->type == MCC_TAC_ELEMENT_TYPE_UNARY) ? tac->unary_op_type : tac->binary_op_type;
    switch(op){
        case MCC_TAC_OPERATION_TYPE_PLUS:
            retval->instruction = new_string("addl\t%s %s %s",tac->identifier1,tac->lhs,tac->rhs);
            break;
        case MCC_TAC_OPERATION_TYPE_MINUS:
            retval->instruction = new_string("subl\t%s %s %s",tac->identifier1,tac->lhs,tac->rhs);
            break;
        case MCC_TAC_OPERATION_TYPE_MULTIPLY:
            retval->instruction = new_string("imull\t%s %s %s",tac->identifier1,tac->lhs,tac->rhs);
            break;
        case MCC_TAC_OPERATION_TYPE_DIVISION:
            retval->instruction = new_string("idivl\t%s %s %s",tac->identifier1,tac->lhs,tac->rhs);
            break;
        case MCC_TAC_OPERATION_TYPE_EQ:
            retval->instruction = new_string("");
            break;
        case MCC_TAC_OPERATION_TYPE_NE:
            retval->instruction = new_string("");
            break;
        case MCC_TAC_OPERATION_TYPE_LT:
            retval->instruction = new_string("");
            break;
        case MCC_TAC_OPERATION_TYPE_GT:
            retval->instruction = new_string("");
            break;
        case MCC_TAC_OPERATION_TYPE_LE:
            retval->instruction = new_string("");
            break;
        case MCC_TAC_OPERATION_TYPE_GE:
            retval->instruction = new_string("");
            break;
        case MCC_TAC_OPERATION_TYPE_AND:
            retval->instruction = new_string("");
            break;
        case MCC_TAC_OPERATION_TYPE_OR:
            retval->instruction = new_string("");
            break;
        case MCC_TAC_OPERATION_TYPE_ASSIGNMENT:
            break;
        case MCC_TAC_OPERATION_TYPE_FAC:
            break;
    }
    return retval;
}

