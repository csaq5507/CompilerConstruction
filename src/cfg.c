#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <mCc/utils.h>
#include "mCc/cfg.h"


#define MALLOC(ptr, size)                                                      \
	ptr = malloc(size);                                                    \
	if ((ptr) == NULL) {                                                   \
		printf("Malloc failed for %p of"                               \
		       "size %li",                                             \
		       ptr, (size));                                           \
		return NULL;                                                   \
	}

#define REALLOC(ptr, size)                                                     \
	void *temp = realloc(ptr, size);                                       \
	if (temp == NULL) {                                                    \
		printf("Realloc failed for %p of"                              \
		       "size %li",                                             \
		       ptr, (size));                                           \
		return NULL;                                                   \
	}                                                                      \
	(ptr) = temp;

static void print_dot_begin(FILE *out);
static void print_dot_end(FILE *out);
static void print_dot_node_start(FILE *out, int node, const char *color);
static void print_dot_node_end(FILE *out);
static void print_dot_edge(FILE *out, int src_node, int dst_node,
			   const char *label);

static cfg_list *generate_unconditional_jump(cfg_list *ret, cfg_list *g_head,
                                        cfg_list *prev_head);
static cfg_list *generate_conditional_jump_left (cfg_list *ret, cfg_list *g_head,
                                                 cfg_list *prev_head, cfg_list *help,
                                                 tac_list* head);
static cfg_list *generate_conditional_jump_right (cfg_list *ret, cfg_list *g_head,
                                                 cfg_list *prev_head, cfg_list *help,
                                                 tac_list* head);
static cfg_list *generate_block(tac_list *head, cfg_list *g_head,
                                cfg_list *prev_head);

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

static void print_dot_node_start(FILE *out, int node, const char *color)
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

	fprintf(out, "\"];\n");
}

static void print_dot_edge(FILE *out, int src_node, int dst_node,
			   const char *label)
{
	assert(out);
	assert(label);

	fprintf(out, "\t\"%d\" -> \"%d\" [label=\"%s\"];\n", src_node, dst_node,
		label);
}

/* ------------------------------------------------------------- */

cfg_list *cfg_new_list()
{
	cfg_list *new = NULL;
	MALLOC(new, sizeof(cfg_list));

	new->node_num = g_node_counter++;
	new->num_next_nodes = 0;
	new->num_prev_nodes = 0;
	new->next_nodes = NULL;
	new->branch = NULL;
	new->prev_nodes = NULL;
	new->tac_start = NULL;
	new->tac_end = NULL;

	return new;
}

cfg_list *mCc_cfg_add_prev(cfg_list *head, cfg_list *new_elem)
{
    assert(head);
    assert(new_elem);

    if (head->num_prev_nodes == 0) {
        MALLOC(head->prev_nodes, sizeof(cfg_list));
    } else {
        REALLOC(head->prev_nodes,
                sizeof(cfg_list) * (head->num_prev_nodes + 1));
    }


    head->prev_nodes[head->num_prev_nodes] = *new_elem;
    head->num_prev_nodes++;
    return head;
}

cfg_list *mCc_cfg_add_node(cfg_list *head, cfg_list *new_elem)
{
    assert(head);
    assert(new_elem);

    REALLOC(head->next_nodes,
            sizeof(cfg_list) * (head->num_next_nodes + 1));

    mCc_cfg_add_prev(new_elem, head);
    memcpy(&head->next_nodes[head->num_next_nodes], new_elem,
           sizeof(cfg_list));
    head->num_next_nodes++;
    free(new_elem);
    return head;
}

void mCc_cfg_delete(cfg_list *head)
{
	assert(head);

	for (int i = 0; i < head->num_next_nodes; i++) {
		if (head->node_num < head->next_nodes[i].node_num)
			mCc_cfg_delete(&head->next_nodes[i]);
	}

	if (head->next_nodes != NULL) {
        free(head->next_nodes);
    }
    if (head->prev_nodes != NULL) {
        free(head->prev_nodes);
    }
	if (head->branch != NULL) {
        free(head->branch);
    }
	if (head->node_num == 0) {
		free(head);
    }
}

static cfg_list* find_elem(cfg_list *head, tac_list *first_elem) {
    if (head == NULL) {
        return NULL;
    }
    if (first_elem == NULL) {
        return NULL;
    }

    while (first_elem->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
            first_elem->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP ||
            first_elem->type == MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP) {
       if (first_elem->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
           first_elem = first_elem->next;
       } else {
           first_elem = first_elem->jump;
       }
    }

    if (first_elem == NULL) {
        return NULL;
    }

    if (head->tac_start == first_elem) {
        return head;
    }

    for (int i = 0; i < head->num_next_nodes; i++) {
        cfg_list *ret = find_elem(&head->next_nodes[i], first_elem);
        if (ret != NULL){
            return ret;
        }
    }
    return NULL;
}

static cfg_list *generate_unconditional_jump(cfg_list *ret, cfg_list *g_head,
                                        cfg_list *prev_head) {
    assert(ret);

    tac_list *elem = head->jump;

    tac_list *tmp = head->next;
    while (tmp != NULL) {
        if (tmp == elem)
            break;
        tmp = tmp->next;
    }

    if (tmp != NULL) {
        while (elem->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
               elem->type == MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP ||
               elem->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP)
            elem = elem->next;

        cfg_list *branch_left = find_elem(g_head, elem);
        if (branch_left == NULL) {
            branch_left = find_elem(prev_head, elem);
        }
        if (branch_left != NULL) {
            ret->branch = malloc(sizeof(cfg_list));
            if ((ret->branch) == NULL) {
                return NULL;
            }
            ret->branch[0] = *branch_left;
            mCc_cfg_add_prev(branch_left, ret);
        } else {
            if (elem->type != MCC_TAC_ELEMENT_TYPE_FUNCTION_END)
                ret = mCc_cfg_add_node(
                        ret, generate_block(elem, g_head, ret));
        }
    }
    return ret;
}

static cfg_list *generate_conditional_jump_left (cfg_list *ret, cfg_list *g_head,
                                            cfg_list *prev_head, cfg_list *help,
                                                 tac_list* head) {

    assert(ret);

    tac_list *left_side = head->jump->next;
    while (left_side->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
           left_side->type == MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP ||
           left_side->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP)
        left_side = left_side->next;
    cfg_list *branch_left = find_elem(g_head, left_side);
    if (branch_left == NULL) {
        branch_left = find_elem(prev_head, left_side);
    }
    if (branch_left != NULL) {
        ret->branch = malloc(sizeof(cfg_list));
        if ((ret->branch) == NULL) {
            return NULL;
        }
        ret->branch[0] = *branch_left;
        mCc_cfg_add_prev(branch_left, ret);
    } else {

        if (left_side->type != MCC_TAC_ELEMENT_TYPE_FUNCTION_END)
            ret = mCc_cfg_add_node(
                    ret, generate_block(left_side, g_head, ret));

        if (ret->tac_start->prev != NULL && ret->tac_start->prev->type == MCC_TAC_ELEMENT_TYPE_LABEL
            && strcmp(ret->tac_start->prev->identifier1, head->identifier1) == 0) {
            help->tac_end = head->jump->prev->prev;
            help->num_next_nodes = 1;
            MALLOC(help->next_nodes, sizeof(cfg_list));
            help->next_nodes[0] = *ret;
            ret = mCc_cfg_add_node(ret, help);
        } else {

        }

    }
    return ret;

}

static cfg_list *generate_conditional_jump_right(cfg_list *ret, cfg_list *g_head,
                                                 cfg_list *prev_head, cfg_list *help,
                                                 tac_list* head) {


    help->tac_end = head->jump->prev->prev;
    help->num_next_nodes = 0;
    cfg_list *help1;
    cfg_list *branch_right = find_elem(g_head, help->tac_start);
    if (branch_right == NULL) {
        branch_right = find_elem(prev_head, help->tac_start);
    }
    if (branch_right != NULL) {
        ret->branch = malloc(sizeof(cfg_list));
        if ((ret->branch) == NULL) {
            return NULL;
        }
        ret->branch[0] = *branch_right;
        mCc_cfg_add_prev(branch_right, ret);
    } else {
        help1 = generate_block(help->tac_start, g_head, ret);
        free(help);
        help = help1;
        ret = mCc_cfg_add_node(ret, help);
    }
    return ret;
}

static cfg_list *generate_block(tac_list *head, cfg_list *g_head,
                                cfg_list *prev_head)
{
	assert(head);

	cfg_list *ret = cfg_new_list();
	ret->tac_start = head;

    if (g_head == NULL) {
        g_head = ret;
    }

	while (true) {
		if (head->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP
            || head->type == MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP
		    || head->type == MCC_TAC_ELEMENT_TYPE_LABEL
		    || head->type == MCC_TAC_ELEMENT_TYPE_FUNCTION_END
		    || head->next == NULL) {
            if (head == ret->tac_start) {
                head = head->next;
                ret->tac_start = head;
                continue;
            } else {
                break;
            }
        }
		head = head->next;
	}

	if (head->type == MCC_TAC_ELEMENT_TYPE_FUNCTION_END) {
		ret->tac_end = head->prev;
	} else if (head->type == MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP) {
        ret->tac_end = head->prev;

        ret = generate_unconditional_jump(ret, g_head, prev_head);
    } else if (head->next == NULL) {
		ret->tac_end = head;
	} else if (head->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
		ret->tac_end = head->prev;
		cfg_list *help = cfg_new_list();
        if (head->next->type == MCC_TAC_ELEMENT_TYPE_LABEL)
		    help->tac_start = head->next->next;
        else
            help->tac_start = head->next;
        if (head->jump->prev->type == MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP)
		    help->tac_end = head->jump->prev->prev;
        else
            help->tac_end = head->jump->prev;


        ret = generate_conditional_jump_left (ret, g_head,
                                              prev_head, help, head);

        ret = generate_conditional_jump_right (ret, g_head,
                                              prev_head, help, head);

	} else if (head->type == MCC_TAC_ELEMENT_TYPE_LABEL) {
        tac_list *temp = head;
        while (temp->next->type == MCC_TAC_ELEMENT_TYPE_LABEL)
            temp = temp->next;
		ret->tac_end = head->prev;
        cfg_list *branch_next= find_elem(g_head, temp->next);
        if (branch_next == NULL) {
            branch_next = find_elem(prev_head, temp->next);
        }
        if ( branch_next != NULL) {
            ret->branch = malloc(sizeof(cfg_list));
            if ((ret->branch) == NULL) {
                return NULL;
            }
            ret->branch[0] = *branch_next;
            mCc_cfg_add_prev(branch_next, ret);
        } else if (temp->next->type != MCC_TAC_ELEMENT_TYPE_FUNCTION_END) {
            ret = mCc_cfg_add_node(ret, generate_block(temp->next, g_head, ret));
		}
	}
	return ret;
}

bool is_branch_avail(cfg_list *elem, cfg_list *branch) {
    assert(elem);
    assert(branch);

    for (int i = 0; i < elem->num_next_nodes; i++) {
        if (&elem->next_nodes[0] == branch)
            return true;
    }
    return false;

}

cfg_list *find_branch(char *jump, cfg_list *head) {
    assert(jump);
    assert(head);

    tac_list *prev = head->tac_start->prev;
    while (prev->type == MCC_TAC_ELEMENT_TYPE_LABEL ||
            prev->type == MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP ||
            prev->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP) {
		if (prev->type == MCC_TAC_ELEMENT_TYPE_LABEL &&
			strcmp(prev->identifier1, jump) == 0) {
				return head;
		}
        prev = prev->prev;
    }


    for (int i = 0; i < head->num_next_nodes; i++) {
        cfg_list *ret = find_branch(jump, &head->next_nodes[i]);
        if (ret != NULL)
            return ret;
    }
    return NULL;
}

void generate_branches_single_block(cfg_list *actual, cfg_list *head) {
    assert(actual);
    assert(head);

    cfg_list *branch = NULL;

    for (int i = 0; i < actual->num_next_nodes; i++) {
        generate_branches_single_block(&actual->next_nodes[i], head);
    }

    tac_list *next = actual->tac_end->next;
    while (next->type == MCC_TAC_ELEMENT_TYPE_LABEL)
        next = next->next;
    if (next->type == MCC_TAC_ELEMENT_TYPE_UNCONDITIONAL_JUMP ||
        next->type == MCC_TAC_ELEMENT_TYPE_CONDITIONAL_JUMP)
        branch = find_branch(next->jump->identifier1, head);
    if (next->type != MCC_TAC_ELEMENT_TYPE_LABEL &&
        next->prev->type == MCC_TAC_ELEMENT_TYPE_LABEL &&
            branch == NULL)
        branch = find_branch(next->prev->identifier1, head);
    if (branch != NULL && !is_branch_avail(actual, branch) &&
            actual->node_num > branch->node_num &&
            actual->branch == NULL)  {
        actual->branch = malloc(sizeof(cfg_list));
        if ((actual->branch) == NULL) {
            return;
        }
        actual->branch[0] = *branch;
		mCc_cfg_add_prev(branch, actual);
    }
}

void generate_branches(cfg_list *head) {
    assert(head);

    for (int i = 0; i < head->num_next_nodes; i++) {
        cfg_list *first_elem = &head->next_nodes[i];
        for (int j = 0; j < first_elem->num_next_nodes; j++) {
            generate_branches_single_block(&first_elem->next_nodes[j], &head->next_nodes[i]);
        }
    }

}

void generate_node_name(FILE *out, tac_list *start, tac_list *end)
{
	assert(out);
	assert(start);
	assert(end);

	while (start != end && start != NULL) {
		print_tac_elem(out, start);
		fprintf(out, "\\n");
		start = start->next;
	}
	if (start != NULL)
		print_tac_elem(out, start);
}

static void print_cfg_function(FILE *out, cfg_list *head)
{
	assert(out);
	assert(head);

	print_dot_node_start(out, head->node_num, "#FFFFFF");
	generate_node_name(out, head->tac_start, head->tac_end);
	print_dot_node_end(out);

	for (int i = 0; i < head->num_next_nodes; i++) {
		print_dot_edge(out, head->node_num,
			       head->next_nodes[i].node_num, "");
	}
	if (head->branch != NULL) {
		print_dot_edge(out, head->node_num, head->branch->node_num, "");
	}

	for (int i = 0; i < head->num_next_nodes; i++) {
		if (head->node_num < head->next_nodes[i].node_num)
			print_cfg_function(out, &head->next_nodes[i]);
	}
}

void mCc_cfg_print_complete(FILE *out, cfg_list *head)
{
	assert(out);
	assert(head);

	print_dot_begin(out);
	print_dot_node_start(out, head->node_num, "#FFFFFF");
	fprintf(out, "HEAD");
	print_dot_node_end(out);
	for (int i = 0; i < head->num_next_nodes; i++) {
		print_dot_edge(
			out, head->node_num, head->next_nodes[i].node_num,
			head->next_nodes[i].tac_start->prev->identifier1);
		print_cfg_function(out, &head->next_nodes[i]);
	}
	print_dot_end(out);
}

void mCc_cfg_print_single_function(FILE *out, cfg_list *head) {
    assert(out);
    assert(head);

    print_dot_begin(out);

    print_dot_node_start(out, head->node_num, "#FFFFFF");
    generate_node_name(out, head->tac_start, head->tac_end);
    print_dot_node_end(out);

    for (int i = 0; i < head->num_next_nodes; i++) {
        print_dot_edge(out, head->node_num,
                       head->next_nodes[i].node_num, "");
    }
    if (head->branch != NULL) {
        print_dot_edge(out, head->node_num, head->branch->node_num, "");
    }

    for (int i = 0; i < head->num_next_nodes; i++) {
        if (head->node_num < head->next_nodes[i].node_num)
            print_cfg_function(out, &head->next_nodes[i]);
    }

    print_dot_end(out);
}

cfg_list *mCc_cfg_generate(tac_list *tac)
{
	assert(tac);

	g_node_counter = 0;
	cfg_list *ret = cfg_new_list();

	while (tac->next != NULL) {
        if (tac->type == MCC_TAC_ELEMENT_TYPE_FUNCTION_START &&
                tac->next->type == MCC_TAC_ELEMENT_TYPE_FUNCTION_END) {
            tac = tac->next;
            if (tac->next != NULL)
                tac = tac->next;
        } else {
            tac = tac->next;
            mCc_cfg_add_node(ret, generate_block(tac, NULL, NULL));
            while (true) {
                if (tac->type == MCC_TAC_ELEMENT_TYPE_FUNCTION_END)
                    break;
                tac = tac->next;
            }
            if (tac->next != NULL)
                tac = tac->next;
        }
	}

    generate_branches(ret);

	return ret;
}