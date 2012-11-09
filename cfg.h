#ifndef _CFG_H_
#define _CFG_H_

#include "shared.h"

int seen_var(int var);
void mark_var_seen(int var);
int seen_block(struct basic_block_t *block);
void mark_block_seen(struct basic_block_t *block);
void clear_blocks_seen(void);
void traverse_three_addr(struct three_addr_t *ta);
void traverse_block(struct basic_block_t *block);

int is_int(char *name);
int is_dummy_block(struct basic_block_t *block);
struct basic_block_t* get_child_nondummy_block(struct basic_block_t *block);
struct parent_node_t* get_nondummy_parents(struct basic_block_t *block);

int is_commutative(int op);
int is_const(int hashval);
int is_unary_op(int op);
int perform_operation(int op, int op1, int op2, int is_branch);
int is_temp_var(int hashval);
void merge_dummy_3_addr(struct basic_block_t *block);
void merge_dummy_parents(struct basic_block_t *block);
void merge_dummy_children(struct basic_block_t *block);
void merge_dummy_3_addrs(void);
void merge_dummy_blocks(void);

void print_hashval(int hashval);
void print_vars_in_three_addr(struct three_addr_t *ta);
void print_vars_seen_in_block(struct basic_block_t *block);
void print_vars_seen(void);
void print_three_addr(struct three_addr_t *ta);
void print_transition_next_block(struct three_addr_t *next);
void print_block(struct basic_block_t *block);
void print_blocks(void);
void print_block_parents(struct basic_block_t *block);
void print_block_children(struct basic_block_t *block);
void print_children_blocks(void);
void print_parent_blocks(void);
void print_program(void);

void init_cfg(void);

#endif /* _CFG_H_ */
