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
                break;
            case MCC_TAC_ELEMENT_TYPE_COPY_IDENTIFIER:
                break;
            case MCC_TAC_ELEMENT_TYPE_UNARY:
                break;
            case MCC_TAC_ELEMENT_TYPE_BINARY:
                temp = mCc_binary_assembly(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP:
                break;
            case MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP:
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
                break;
            case MCC_TAC_ELEMENT_TYPE_FUNCTION_END:
                break;
            case MCC_TAC_ELEMENT_TYPE_RETURN:
                break;
        }
        current->next = temp;
        while(current->next != NULL)
            current=current->next;
        tac=tac->next;
    } while(tac!=NULL);
}

struct mCc_assembly_line * mCc_binary_assembly(struct mCc_tac_list * tac)
{
    struct mCc_assembly_line * retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    switch(tac->binary_op_type){
        case MCC_TAC_OPERATION_TYPE_ASSIGNMENT:
            break;
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
        case MCC_TAC_OPERATION_TYPE_FAC:
            break;
        case MCC_TAC_OPERATION_TYPE_EQ:
            break;
        case MCC_TAC_OPERATION_TYPE_NE:
            break;
        case MCC_TAC_OPERATION_TYPE_LT:
            break;
        case MCC_TAC_OPERATION_TYPE_GT:
            break;
        case MCC_TAC_OPERATION_TYPE_LE:
            break;
        case MCC_TAC_OPERATION_TYPE_GE:
            break;
        case MCC_TAC_OPERATION_TYPE_AND:
            break;
        case MCC_TAC_OPERATION_TYPE_OR:
            break;
    }
}
