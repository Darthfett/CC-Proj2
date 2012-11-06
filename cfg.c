#include "cfg.h"
#include "symtab.h"

extern struct program_t *program;
extern int error_flag;

int out_line_no = 1;
struct basic_block_t **seen_blocks;
int seen_blocks_count = 0;
int seen_blocks_size = 20;


void print_three_addr(struct three_addr_t *ta)
{
    char *lhs = get_hashval_name(ta->LHS);
    char *op1 = get_hashval_name(ta->op1);
    char *op2 = get_hashval_name(ta->op2);
    switch (ta->type) {
    case THREE_ADDR_T_ASSIGN:
        switch(ta->op) {
        case OP_ASSIGNMENT:
            printf("%s = %s;\n", lhs, op1);
            break;
        case OP_PLUS:
            printf("%s = %s + %s;\n", lhs, op1, op2);
            break;
        case OP_MINUS:
            printf("%s = %s - %s;\n", lhs, op1, op2);
            break;
        case OP_EQUAL:
            printf("%s = %s == %s;\n", lhs, op1, op2);
            break;
        case OP_NOTEQUAL:
            printf("%s = %s != %s;\n", lhs, op1, op2);
            break;
        case OP_LT:
            printf("%s = %s < %s;\n", lhs, op1, op2);
            break;
        case OP_GT:
            printf("%s = %s > %s;\n", lhs, op1, op2);
            break;
        case OP_LE:
            printf("%s = %s <= %s;\n", lhs, op1, op2);
            break;
        case OP_GE:
            printf("%s = %s >= %s;\n", lhs, op1, op2);
            break;
        case OP_NOT:
            printf("%s = !%s;\n", lhs, op1);
            break;
        case OP_SLASH:
            printf("%s = %s / %s;\n", lhs, op1, op2);
            break;
        case OP_STAR:
            printf("%s = %s * %s;\n", lhs, op1, op2);
            break;
        case OP_MOD:
            printf("%s = %s %% %s;\n", lhs, op1, op2);
            break;
        case OP_OR:
            printf("%s = %s || %s;\n", lhs, op1, op2);
            break;
        case OP_AND:
            printf("%s = %s && %s;\n", lhs, op1, op2);
            break;
        default:
            printf("Invalid 3-address assignment type <%d>, op <%d>, LHS <%s>, op1 <%s>, next <%p>, next_b1 <%p>\n", ta->type, ta->op, lhs, op1, ta->next, ta->next_b1);
        }
        break;
    case THREE_ADDR_T_BRANCH:
        if (ta->next_b1 == NULL) {
            if (ta->next_b2 == NULL) {
                printf("jump exit;");
            } else {
                printf("branch(%s, %s, %d);\n", op1, "exit", ta->next_b2->unique_id);
            }
        } else {
            if (ta->next_b2 == NULL) {
                printf("branch(%s, %d, %s);\n", op1, ta->next_b1->unique_id, "exit");
            } else {
                printf("branch(%s, %d, %d);\n", op1, ta->next_b1->unique_id, ta->next_b2->unique_id);
            }
        }
        break;
    case THREE_ADDR_T_DUMMY:
        printf("No Op; // lhs = %s, op = %d\n", lhs, ta->op);
        break;
    default:
        printf("Invalid 3-address code <%d>\n", ta->type);
        error_flag = 1;
    }
}

int seen_block(struct basic_block_t *block)
{
    int i;
    for (i = 0; i < seen_blocks_count; i++) {
        if (seen_blocks[i] == block) {
            return 1;
        }
    }
    return 0;
}

void mark_block_seen(struct basic_block_t *block)
{
    /* Check if there is room for the block */
    if (seen_blocks_size == seen_blocks_count) {
        /* Not enough room to add block -- double the size */
        struct basic_block_t **new_seen_blocks = (struct basic_block_t**) malloc(sizeof(struct basic_block_t*) * seen_blocks_size * 2);
        memcpy(new_seen_blocks, seen_blocks, seen_blocks_size);
        free(seen_blocks);
        seen_blocks = new_seen_blocks;
        seen_blocks_size *= 2;
    }

    /* Add block to seen blocks */
    seen_blocks[seen_blocks_count] = block;
    seen_blocks_count++;
}

void remove_dummy_nodes(struct basic_block_t *block)
{
    struct three_addr_t *next = block->first;

    while (next != NULL) {
        if (next->next != NULL) {
            if (next->type == THREE_ADDR_T_DUMMY) {
                *next = *(next->next);
                continue;
            }
        }
        next = next->next;
    }
}

void traverse_three_addr(struct three_addr_t *ta)
{
    if (ta->type == THREE_ADDR_T_BRANCH) {
        traverse_block(ta->next_b1 = get_child_nondummy_block(ta->next_b1));
        traverse_block(ta->next_b2 = get_child_nondummy_block(ta->next_b2));
    }
    
}

void traverse_block(struct basic_block_t *block)
{
    if (block == NULL) {
        return;
    }

    /* Check if block has been seen */
    int block_seen = seen_block(block);

    if (! block_seen) {
        mark_block_seen(block);
    } else {
        /* Already seen this block - don't traverse it again */
        return;
    }

    remove_dummy_nodes(block);

    /* Go through block marking blocks as seen */
    struct three_addr_t *next = block->first;

    while(next != NULL) {
        traverse_three_addr(next);
        
        if (next->next == NULL) {

            traverse_block(next->next_b1 = get_child_nondummy_block(next->next_b1));
            break;
        }
        next = next->next;
    }
}

int is_dummy_block(struct basic_block_t *block)
{
    struct three_addr_t *next = block->first;
    while (next != NULL) {
        if (next->type != THREE_ADDR_T_DUMMY) {
            return 0;
        }
        next = next->next;
    }
    return 1;
}

struct basic_block_t* get_child_nondummy_block(struct basic_block_t *block)
{
    if (block == NULL) {
        return NULL;
    }
    if (! is_dummy_block(block)) {
        return block;
    }

    return get_child_nondummy_block(block->last->next_b1);
}

void print_transition_next_block(struct three_addr_t *next)
{
    if (next->next_b1 == NULL) {
        printf("%d: jump exit;\n", out_line_no++);
    } else if (next->type == THREE_ADDR_T_BRANCH) {
        // Already transitioning 
    } else {
        printf("%d: jump %d;\n", out_line_no++, next->next_b1->unique_id);
    }

}

void print_block(struct basic_block_t *block)
{
    if (block == NULL) {
        printf("ERROR: print NULL block\n");
        error_flag = 1;
        return;
    }

    printf("block %d:\n", block->unique_id);

    struct three_addr_t *next = block->first;

    while(next != NULL) {
        printf("%d: ", out_line_no);
        print_three_addr(next);
        out_line_no++;
        if (next->next == NULL) {
            // printf("%d: jump %d\n", out_line_no++, next->next_b1->unique_id);
            print_transition_next_block(next);
            break;
        }
        next = next->next;
    }
}

void print_blocks(void)
{
    int i;
    for (i = 0; i < seen_blocks_count; i++) {
        print_block(seen_blocks[i]);
    }
    printf("exit:\n");
}

void print_program(void) {
    struct cfg_t *cfg = program->cl->cb->fdl->fd->fb->cfg;
    struct basic_block_t *block = cfg->first;
    remove_dummy_nodes(block);
    traverse_block(block);
    print_blocks();
}

void init_cfg(void)
{
    seen_blocks = (struct basic_block_t**) malloc(sizeof(struct basic_block_t*) * seen_blocks_size);
}
