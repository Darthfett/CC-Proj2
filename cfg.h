#ifndef _CFG_H_
#define _CFG_H_

#include "shared.h"

void print_program(void);
struct three_addr_t* print_three_addr(struct three_addr_t *ta);
void print_block(struct basic_block_t *block);
void init_cfg(void);

#endif /* _CFG_H_ */
