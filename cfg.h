#ifndef _CFG_H_
#define _CFG_H_

#include "shared.h"

int is_dummy_block(struct basic_block_t *block);
struct basic_block_t* get_child_nondummy_block(struct basic_block_t *block);
void remove_dummy_nodes(struct basic_block_t *block);
int seen_block(struct basic_block_t *block);
void mark_block_seen(struct basic_block_t *block);
void traverse_three_addr(struct three_addr_t *ta);
void traverse_block(struct basic_block_t *block);
void print_three_addr(struct three_addr_t *ta);
void print_block(struct basic_block_t *block);
void print_blocks(void);
void print_program(void);
void init_cfg(void);

#endif /* _CFG_H_ */
