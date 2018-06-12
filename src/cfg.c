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

void mCc_cfg_print(FILE *out, cfg_list *head) {
	assert(out);
	assert(head);

	fprintf(out, "------------%d------------\n", head->node_num);
	print_tac_elem(out, head->tac_start);
	print_tac_elem(out, head->tac_end);
	fprintf(out, "next nodes: ");
	for (int i = 0; i < head->num_next_nodes; i++) {
		fprintf(out, "%d ", head->next_nodes[i].node_num);
	}
	fprintf(out, "\n");

	for (int i = 0; i < head->num_next_nodes; i++) {
		if (head->node_num < head->next_nodes[i].node_num)
			mCc_cfg_print(out, &head->next_nodes[i]);
	}
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