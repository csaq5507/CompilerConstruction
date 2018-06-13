#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <mCc/utils.h>
#include "mCc/cfg.h"


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

static void print_dot_begin(FILE *out);
static void print_dot_end(FILE *out);
static void print_dot_node_start(FILE *out, int node,  const char *color);
static void print_dot_node_end(FILE *out);
static void print_dot_edge(FILE *out, int src_node, int dst_node, const char *label);


/* ------------------------------------------------------------- DOT Printer */

static void print_dot_begin(FILE *out)
{
    assert(out);

    fprintf(out, "digraph \"AST\" {\n");
    fprintf(out, "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out)
{
    assert(out);

    fprintf(out, "}\n");
}

static void print_dot_node_start(FILE *out, int node,
                           const char *color)
{
    assert(out);

    fprintf(out,
            "\t\"%d\" [shape=box, style=filled, fillcolor=\"%s\" , "
                    "label=\"",
            node, color);
}

static void print_dot_node_end(FILE *out)
{
    assert(out);

    fprintf(out,"\"];\n");
}

static void print_dot_edge(FILE *out, int src_node,
                           int dst_node, const char *label)
{
    assert(out);
    assert(label);

    fprintf(out, "\t\"%d\" -> \"%d\" [label=\"%s\"];\n", src_node, dst_node,
            label);
}

/* ------------------------------------------------------------- */

cfg_list *cfg_new_list() {
	cfg_list *new = NULL;
	MALLOC(new, sizeof(cfg_list));

	new->node_num = g_node_counter++;
	new->num_next_nodes = 0;
	new->num_prev_nodes = 0;
	new->next_nodes = NULL;
	new->prev_nodes = NULL;
	new->tac_start = NULL;
	new->tac_end = NULL;

	return new;
}

cfg_list *mCc_cfg_add_node(cfg_list *head, cfg_list *new_elem) {
	assert(head);
	assert(new_elem);

	REALLOC(head->next_nodes, sizeof(cfg_list) * (head->num_next_nodes + 1));

	new_elem->prev_nodes = head;
	memcpy(&head->next_nodes[head->num_next_nodes], new_elem, sizeof(cfg_list));
	head->num_next_nodes++;
    free(new_elem);
	return head;
}

void mCc_cfg_delete(cfg_list *head) {
	assert(head);

	for (int i = 0; i < head->num_next_nodes; i++) {
		if (head->node_num < head->next_nodes[i].node_num)
			mCc_cfg_delete(&head->next_nodes[i]);
	}

	if (head->next_nodes != NULL)
		free(head->next_nodes);
	if (head->node_num == 0)
		free(head);
}

cfg_list *generate_block(tac_list *head) {
	assert(head);

	cfg_list *ret = cfg_new_list();
	ret->tac_start = head;

	while (true) {

		if (head->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP ||
				head->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
				head->type == MCC_TAC_ELEMENT_TYPE_FUNCTION_END ||
				head->next == NULL)
			break;
		head = head->next;
	}

	if (head->type == MCC_TAC_ELEMENT_TYPE_FUNCTION_END) {
		ret->tac_end = head->prev;
	} else if (head->next == NULL) {
		ret->tac_end = head;
	} else if (head->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
		ret->tac_end = head->prev;
		cfg_list *help = cfg_new_list();
		help->tac_start = head->next;
		help->tac_end = head->jump->prev;
		if (help->tac_end->type == MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP) {
			help->tac_end = head->jump->prev->prev;
            help->num_next_nodes = 1;
            MALLOC(help->next_nodes, sizeof(cfg_list));
            help->next_nodes[0] = *ret;
		} else {
		}
		mCc_cfg_add_node(ret, help);
		mCc_cfg_add_node(ret, generate_block(head->jump->next));
	} else if (head->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
		ret->tac_end = head->prev;
		mCc_cfg_add_node(ret, generate_block(head->next));
	}

	return ret;
}

void generate_node_name(FILE *out, tac_list *start, tac_list *end) {
    assert(out);
    assert(start);
    assert(end);

    char label[2048] = {0};
    while (start != end) {
        print_tac_elem(out, start);
        fprintf(out, "\\n");
        start = start->next;
    }
    print_tac_elem(out, start);
}

static void print_cfg_function(FILE *out, cfg_list *head) {
    assert(out);
    assert(head);

    print_dot_node_start(out, head->node_num, "#FFFFFF");
    generate_node_name(out, head->tac_start, head->tac_end);
    print_dot_node_end(out);

    for (int i = 0; i < head->num_next_nodes; i++) {
        print_dot_edge(out, head->node_num, head->next_nodes[i].node_num, "");
    }

    for (int i = 0; i < head->num_next_nodes; i++) {
        if (head->node_num < head->next_nodes[i].node_num)
            print_cfg_function(out, &head->next_nodes[i]);
    }
}

void mCc_cfg_print(FILE *out, cfg_list *head) {
	assert(out);
	assert(head);

    print_dot_begin(out);
    print_dot_node_start(out, head->node_num, "#FFFFFF");
    fprintf(out, "HEAD");
    print_dot_node_end(out);
    for (int i = 0; i < head->num_next_nodes; i++) {
        print_dot_edge(out, head->node_num, head->next_nodes[i].node_num,
                       head->next_nodes[i].tac_start->prev->identifier1);
        print_cfg_function(stdout, &head->next_nodes[i]);
    }
    print_dot_end(out);

}

cfg_list *mCc_cfg_generate(tac_list *tac) {
	assert(tac);

	g_node_counter = 0;
	cfg_list *ret = cfg_new_list();

	while (tac->next != NULL) {
		tac = tac->next;
		mCc_cfg_add_node(ret, generate_block(tac));
		while (true) {
			if (tac->type == MCC_TAC_ELEMENT_TYPE_FUNCTION_END)
				break;
			tac = tac->next;
		}
		if (tac->next != NULL)
			tac = tac->next;
	}

	return ret;
}