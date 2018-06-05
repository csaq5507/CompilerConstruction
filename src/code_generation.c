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



struct mCc_assembly *mCc_generate_assembly(struct mCc_tac_list *tac)
{
    struct mCc_assembly * assembly;
    string_label_idx = 0;
    label_idx = 0;
    stack_pointer = 0;
    MALLOC(label, sizeof(struct labels))
    label->counter=0;
    label->l=NULL;
    MALLOC(assembly, sizeof(struct mCc_assembly));
    assembly->counter=0;
    assembly->head = malloc(sizeof(struct mCc_assembly_line));
    assembly->head->type = MCC_ASSEMBLY_FILE;
    assembly->head->prev=NULL;
    assembly->head->next=NULL;
    assembly->head->instruction = new_string("\t.file\t\"%s\"","test.mC");
    struct mCc_assembly_line * current = assembly->head;
    do{
        struct mCc_assembly_line * temp = NULL;
        switch (tac->type)
        {
            case MCC_TAC_ELEMENT_TYPE_COPY_LITERAL:
                temp = mCc_assembly_copy_literal(tac, current);
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
                break;
            case MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP:
                temp = mCc_assembly_jump(tac);
                break;
            case MCC_TAC_ELEMENT_TYPE_LABEL:
                break;
            case MCC_TAC_ELEMENT_TYPE_PARAMETER_SETUP:
                MALLOC(temp, sizeof(struct mCc_assembly_line));
                temp->instruction = new_string("\tsubl\t$%d, %s",get_literal_size(tac->decl_lit_type),"%esp");
                break;
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
                break;
            case MCC_TAC_ELEMENT_TYPE_RETURN:
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

struct mCc_assembly_line *mCc_procedure_call(struct mCc_tac_list *tac) {

    struct mCc_assembly_line * temp;
    MALLOC(temp, sizeof(struct mCc_assembly_line))
    temp->instruction = new_string("\tcall\t%s",tac->identifier1);
    temp->next = NULL;
    return temp;
    /*
    if(tac->num_function_param == 0)
        return temp;
    else if(tac->num_function_param == 1) {
        struct mCc_assembly_line * params;
        MALLOC(params,(sizeof(struct mCc_assembly_line)))
        params->next = temp;
        temp->prev = params;
        params->instruction = new_string("\tpushl\t%s",get_label(tac->prev->identifier1));
        return params;
    } else {
        struct mCc_assembly_line * params;
        MALLOC(params,(sizeof(struct mCc_assembly_line) * tac->num_function_param))
        params[tac->num_function_param - 1].next = temp;
        temp->prev = &(params[tac->num_function_param -1]);
        params[tac->num_function_param - 1].prev = &(params[tac->num_function_param - 2]);
        for (int i = tac->num_function_param - 2; i > 0; --i) {
            params[i].next = &(params[i + 1]);
            params[i].prev = &(params[i - 1]);
        }
        params[0].next = &(params[1]);
        struct mCc_tac_list * current = tac;
        for (int j = 0; j < tac->num_function_param; ++j) {
            current=current->prev;
        }
        for (int j = 0; j < tac->num_function_param; ++j) {
            params[j].instruction = new_string("\tpushl\t%s",get_label(current->identifier1));
            current=current->next;
        }
        temp->next = NULL;
        return &(params[0]);
    }*/
}

struct mCc_assembly_line *mCc_assembly_function_end(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * temp;
    struct mCc_assembly_line * temp1;
    struct mCc_assembly_line * temp2;
    struct mCc_assembly_line * temp3;
    MALLOC(temp, sizeof(struct mCc_assembly_line))
    MALLOC(temp1, sizeof(struct mCc_assembly_line))
    MALLOC(temp2, sizeof(struct mCc_assembly_line))
    MALLOC(temp3, sizeof(struct mCc_assembly_line))
    temp->next = temp1;
    temp1->prev = temp;
    temp1->next = temp2;
    temp2->prev = temp1;
    temp2->next = temp3;
    temp3->prev = temp2;
    temp3->next = NULL;


    temp->instruction = new_string(".RET%s:",tac->identifier1);
    temp1->instruction = new_string("\tleave");
    temp2->instruction = new_string("\tret");
    temp3->instruction = new_string("\t.size\t%s, .-%s", tac->identifier1, tac->identifier1);

    if(strcmp(tac->identifier1,"main") == 0)
    {
        struct mCc_assembly_line * temp4;
        MALLOC(temp4, sizeof(struct mCc_assembly_line))
        temp4->next = temp;
        temp->prev = temp4;
        temp4->instruction = new_string("\tmovl\t$0, %s","%eax");
        return temp4;
    }
    return temp;
}


struct mCc_assembly_line *mCc_assembly_function_start(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * temp;
    struct mCc_assembly_line * temp1;
    struct mCc_assembly_line * temp2;
    struct mCc_assembly_line * temp3;
    struct mCc_assembly_line * temp4;
    MALLOC(temp, sizeof(struct mCc_assembly_line))
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
    temp->instruction = new_string("\t.globl\t%s",tac->identifier1);
    temp1->instruction = new_string("\t.type\t%s, @function",tac->identifier1);
    temp2->instruction = new_string("%s:",tac->identifier1);
    temp3->instruction = new_string("\tpushl\t%s","%ebp");
    temp4->instruction = new_string("\tmovl\t%s, %s","%esp", "%ebp");
    return temp;
}


struct mCc_assembly_line *mCc_assembly_copy_identifier(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * temp;
    MALLOC(temp, sizeof(struct mCc_assembly_line))
    stack_pointer+=4;
    temp->instruction = new_string("\tmovl\t%s,-%d(%s)",get_label(tac->copy_identifier),stack_pointer,"%edp");

}

struct mCc_assembly_line *mCc_assembly_copy_literal(struct mCc_tac_list *tac, struct mCc_assembly_line *current) {
    struct mCc_assembly_line * retval;
    bool is_pushl_copy = check_pushl_copy(tac);
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    switch (tac->literal_type) {
        case (MCC_TAC_LITERAL_TYPE_INT):
            if (is_pushl_copy)
                retval->instruction = new_string("\tpushl $%d", tac->i_literal);
            else {
                retval->instruction = new_string("\tmovl\t$%d %s", tac->i_literal, tac->identifier1);
                //TODO EAX
            }
            break;
        case (MCC_TAC_LITERAL_TYPE_FLOAT):
            if (is_pushl_copy)
                retval->instruction = new_string("\tpushl $%d", floor(tac->f_literal));
            else {
                retval->instruction = new_string("\tmovl\t$%d %s", floor(tac->f_literal), tac->identifier1);
                //TODO EAX
            }
            break;
        case (MCC_TAC_LITERAL_TYPE_BOOL):
            if (is_pushl_copy)
                retval->instruction = new_string("\tpushl $%d", tac->b_literal ? 1 : 0);
            else {
                retval->instruction = new_string("\tmovl\t$%d %s", tac->b_literal ? 1 : 0, tac->identifier1);
                //TODO EAX
            }
            break;
        case (MCC_TAC_LITERAL_TYPE_STRING):
            create_string_label(tac,current);
            if (is_pushl_copy)
                retval->instruction = new_string("\tpushl\t$%s", get_label(tac->identifier1));
            else {
                retval->instruction = new_string("\tmovl\t$%s %s", get_label(tac->identifier1), tac->identifier1);
                //TODO EAX
            }
            break;
    }
    retval->next = NULL;
    return retval;
}

struct mCc_assembly_line * create_string_label(struct mCc_tac_list *tac, struct mCc_assembly_line *current) {
    while (current->prev->type != MCC_ASSEMBLY_FILE)
        current=current->prev;
    struct mCc_assembly_line * temp;
    struct mCc_assembly_line * temp1;
    struct mCc_assembly_line * temp2;
    MALLOC(temp1, sizeof(struct mCc_assembly_line))
    MALLOC(temp2, sizeof(struct mCc_assembly_line))
    MALLOC(temp, sizeof(struct mCc_assembly_line))
    current->prev->next = temp;
    temp->prev = current->prev;
    temp->next = temp1;
    temp1->prev = temp;
    temp1->next = temp2;
    temp2->prev = temp1;
    temp2->next = current;
    current->prev = temp2;
    temp->instruction = new_string(".LC%d", string_label_idx++);
    set_label(tac->identifier1, temp->instruction);
    temp1->instruction = new_string("\t.string \"%s\"", tac->s_literal);
    temp2->instruction = new_string("\t.text");
    return temp;
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
        case MCC_TAC_OPERATION_TYPE_NE:
        case MCC_TAC_OPERATION_TYPE_LT:
        case MCC_TAC_OPERATION_TYPE_GT:
        case MCC_TAC_OPERATION_TYPE_LE:
        case MCC_TAC_OPERATION_TYPE_GE:
            retval->instruction = new_string("cmpl\t%s %s",tac->lhs,tac->rhs);
            jump_cond = op;
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
        default:
            printf("error");
            break;
    }
    return retval;
}


struct mCc_assembly_line *mCc_assembly_jump(struct mCc_tac_list *tac) {
    struct mCc_assembly_line * retval;
    MALLOC(retval, sizeof(struct mCc_assembly_line))
    switch(jump_cond){
        case MCC_TAC_OPERATION_TYPE_EQ:
            retval->instruction = new_string("jne\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_NE:
            retval->instruction = new_string("je\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_LT:
            retval->instruction = new_string("jge\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_GT:
            retval->instruction = new_string("jle\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_LE:
            retval->instruction = new_string("jg\t%s",tac->jump->identifier1);
            break;
        case MCC_TAC_OPERATION_TYPE_GE:
            retval->instruction = new_string("jl\t%s",tac->jump->identifier1);
            break;
        default:
            printf("error");
            break;
    }
    jump_cond = -1;
}

void mCc_print_assembly(FILE * out, struct mCc_assembly *ass)
{

    struct mCc_assembly_line *current = ass->head;
    while (current != NULL) {
        fprintf(out,"%s\n",current->instruction);
        current = current->next;
    }
}

void set_label(char *key, char *value)
{
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

char * get_label(char * key){
    for (int i = 0; i < label->counter; ++i) {
        if(strcmp(label->l[i].key,key)==0)
            return label->l[i].value;
    }
}