
#ifndef MINICOMPILER_CFG_H
#define MINICOMPILER_CFG_H

#include <stdio.h>

#include "mCc/ast.h"
#include "ast_visit.h"
#include "parser.h"
#include "mCc/tac.h"

#ifdef __cplusplus
extern "C" {
#endif

int g_node_counter;
char *actual_label;

typedef struct mCc_cfg_list {
	int node_num;

	int num_next_nodes;
	int num_prev_nodes;
	struct mCc_cfg_list *next_nodes;
	struct mCc_cfg_list *prev_nodes;
	struct mCc_cfg_list *branch;

	struct mCc_tac_list *tac_start;
	struct mCc_tac_list *tac_end;
} cfg_list;


cfg_list *cfg_new_list();
void mCc_cfg_delete(cfg_list *head);

void mCc_cfg_print(FILE *out, cfg_list *head);

cfg_list *mCc_cfg_generate(tac_list *tac);

#ifdef __cplusplus
}
#endif

#endif // MINICOMPILER_CFG_H
