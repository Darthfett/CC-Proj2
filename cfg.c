#include <ctype.h>

#include "cfg.h"
#include "symtab.h"

extern struct program_t *program;
extern int error_flag;

int out_line_no = 1;
struct basic_block_t **seen_blocks;
int seen_blocks_count = 0;
int seen_blocks_size = 20;

int *seen_vars;
int seen_vars_count = 0;
int seen_vars_size = 20;

struct three_addr_t **const_vars;
int const_vars_count = 0;
int const_vars_size = 20;

/*
 * Traversal functions
 */

int is_const_var(int var)
{
    char *name = get_hashval_name(var);
    if (is_int(name)) {
        return 1;
    }

    int i;
    for (i = 0; i < const_vars_count; i++) {
        if (const_vars[i]->LHS == var) {
            return 1;
        }
    }
    return 0;
}

int get_const_var_val(int var)
{
    char *name = get_hashval_name(var);
    if (is_int(name)) {
        return atoi(name);
    }
    int i;
    for (i = 0; i < const_vars_count; i++) {
        if (const_vars[i]->LHS == var) {
            return get_const_var_val(const_vars[i]->op1);
        }
    }
    printf("ERROR: Var %d %s marked const, but not in list\n", var, name);
    error_flag = 1;
    return 9001;
}

void mark_var_const(struct three_addr_t *var_three_addr)
{
    struct three_addr_t *LHS = (struct three_addr_t*) malloc(sizeof(struct three_addr_t));
    *LHS = *var_three_addr;
    if (is_const_var(LHS->LHS)) {
        int i;
        for (i = 0; i < const_vars_count; i++) {
            if (const_vars[i]->LHS == LHS->LHS) {
                const_vars[i] = LHS;
                return;
            }
        }
    }
    /* Check if there is room for the var */
    if (const_vars_size == const_vars_count) {
        /* Not enough room to add vars -- double the size */
        struct three_addr_t **new_const_vars = (struct three_addr_t**) malloc(sizeof(struct three_addr_t*) * const_vars_size * 2);
        memcpy(new_const_vars, const_vars, sizeof(struct three_addr_t*) * const_vars_size);
        free(const_vars);
        const_vars = new_const_vars;
        const_vars_size *= 2;
    }

    /* Add var to const vars */
    const_vars[const_vars_count] = LHS;
    const_vars_count++;
}

void clear_const_vars(void)
{
    free(const_vars);
    const_vars_count = 0;
    const_vars = (struct three_addr_t**) malloc(sizeof(struct three_addr_t*) * const_vars_size);
}

int seen_var(int var)
{
    int i;
    for (i = 0; i < seen_vars_count; i++) {
        if (seen_vars[i] == var) {
            return 1;
        }
    }
    return 0;
}

void mark_var_seen(int var)
{
    /* Check if there is room for the var */
    if (seen_vars_size == seen_vars_count) {
        /* Not enough room to add vars -- double the size */
        int *new_seen_vars = (int*) malloc(sizeof(int) * seen_vars_size * 2);
        memcpy(new_seen_vars, seen_vars, sizeof(int) * seen_vars_size);
        free(seen_vars);
        seen_vars = new_seen_vars;
        seen_vars_size *= 2;
    }

    /* Add var to seen vars */
    seen_vars[seen_vars_count] = var;
    seen_vars_count++;
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
        memcpy(new_seen_blocks, seen_blocks, sizeof(struct basic_block_t*) *seen_blocks_size);
        free(seen_blocks);
        seen_blocks = new_seen_blocks;
        seen_blocks_size *= 2;
    }

    /* Add block to seen blocks */
    seen_blocks[seen_blocks_count] = block;
    seen_blocks_count++;
}

void clear_blocks_seen(void)
{
    free(seen_blocks);
    seen_blocks_count = 0;
    seen_blocks = (struct basic_block_t**) malloc(sizeof(struct basic_block_t*) * seen_blocks_size);
}

void traverse_three_addr(struct three_addr_t *ta)
{
    if (ta->type == THREE_ADDR_T_BRANCH) {
        traverse_block(ta->next_b1);
        traverse_block(ta->next_b2);
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
    
    /* Go through block marking blocks as seen */
    struct three_addr_t *next = block->first;

    while(next != NULL) {
        traverse_three_addr(next);
        
        if (next->next == NULL) {

            traverse_block(next->next_b1);
            break;
        }
        next = next->next;
    }
}

/*
 * Block dummy functions
 */

int is_int(char *name)
{
    char *iter = name;
    while (*iter != '\0') {
        if (! isdigit(*iter)) {
            return 0;
        }
        iter++;
    }
    return 1;
}

int is_dummy_block(struct basic_block_t *block)
{
    if (block == NULL) {
        return 0;
    }

    if (! seen_block(block)) {
        return 1;
    }

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

struct parent_node_t* get_nondummy_parents(struct basic_block_t *block)
{
    if (block == NULL) {
        return NULL;
    }

    struct parent_node_t *parent_list = NULL;

    struct parent_node_t *p = block->parents;
    while (p != NULL) {
        if (is_dummy_block(p->parent)) {
            struct parent_node_t *p2 = get_nondummy_parents(p->parent);
            if (p2 != NULL) {
                struct parent_node_t *last = p2;
                while (last->next != NULL)
                    last = last->next;
                last->next = parent_list;
                parent_list = p2;
            }
        } else {
            struct parent_node_t *p2 = (struct parent_node_t*) malloc(sizeof(struct parent_node_t));
            p2->parent = p->parent;
            p2->next = parent_list;
            parent_list = p2;
        }
        p = p->next;
    }
    return parent_list;
}

/*
 * Merging functions
 */

int is_const(int hashval)
{
    if (is_int(get_hashval_name(hashval))) {
        return 1;
    }
    if (is_const_var(hashval)) {
        return 1;
    }
    return 0;
}

int is_unary_op(int op)
{
    if (op == OP_ASSIGNMENT ||
        op == OP_NOT) {
        return 1;
    }
    return 0;
}

int perform_operation(int op, int op1, int op2, int is_branch)
{
    op1 = get_const_var_val(op1);
    if (is_branch) {
        return op1;
    }
    if (! is_unary_op(op)) {
        op2 = get_const_var_val(op2);
    }
    switch(op) {
    case OP_ASSIGNMENT:
        return op1;
    case OP_PLUS:
        return op1 + op2;
    case OP_MINUS:
        return op1 - op2;
    case OP_EQUAL:
        return op1 == op2;
    case OP_NOTEQUAL:
        return op1 != op2;
    case OP_LT:
        return op1 < op2;
    case OP_GT:
        return op1 > op2;
    case OP_LE:
        return op1 <= op2;
    case OP_GE:
        return op1 >= op2;
    case OP_NOT:
        return ! op1;
    case OP_OR:
        return op1 || op2;
    case OP_STAR:
        return op1 * op2;
    case OP_SLASH:
        return op1 / op2;
    case OP_MOD:
        return op1 % op2;
    case OP_AND:
        return op1 && op2;
    default:
        break;
    }
    printf("ERROR: Invalid operation op %d\n", op);
    error_flag = 1;
    return 9001;
}

int is_temp_var(int hashval)
{
    char *name = get_hashval_name(hashval);
    if (strncmp(name, "__usr_t", strlen("__usr_t")) == 0) {
        return 1;
    }
    return 0;
}

void eval_constants_in_block(struct basic_block_t *block)
{
    struct three_addr_t *next = block->first;
    while (next != NULL) {
        if (is_const_var(next->op1)) {
            if (next->type == THREE_ADDR_T_BRANCH || is_unary_op(next->op) || is_const_var(next->op2)) {
                next->op1 = perform_operation(next->op, next->op1, next->op2, next->type == THREE_ADDR_T_BRANCH);
                char *int_repr = (char*) malloc(sizeof(char) * 20);
                snprintf(int_repr, 20, "%d", next->op1);
                next->op1 = get_name_hashval(int_repr);
                next->op = OP_ASSIGNMENT;
                mark_var_const(next);
                if (is_temp_var(next->LHS)) {
                    if (next->next != NULL) {
                        *next = *(next->next);
                        continue;
                    }
                }
            }
        }
        next = next->next;
    }

}

void eval_constants(void)
{
    int i;
    for (i = 0; i < seen_blocks_count; i++) {
        eval_constants_in_block(seen_blocks[i]);
        clear_const_vars();
    }
}

void merge_dummy_3_addr(struct basic_block_t *block)
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

void merge_dummy_parents(struct basic_block_t *block)
{
    /* Update the parents of block to point to only non-dummy blocks.
       Dummy nodes' recursive non-dummy parents are replaced for dummys
    */
    struct parent_node_t *p = block->parents;
    struct parent_node_t *new_parents = NULL;
    while (p != NULL) {
        if (is_dummy_block(p->parent)) {
            struct parent_node_t *dummy_parents = get_nondummy_parents(p->parent);

            // Merge dummy parents with new parents
            if (dummy_parents != NULL) {
                struct parent_node_t *last = dummy_parents;
                while (last->next != NULL)
                    last = last->next;
                last->next = new_parents;
                new_parents = dummy_parents;
            } else {
                printf("ERROR: Dummy block %d has no parents\n", p->parent->unique_id);
                error_flag = 1;
            }
        } else {
            // Just add the parent to the new_parents
            struct parent_node_t *p2 = (struct parent_node_t*) malloc(sizeof(struct parent_node_t));
            p2->parent = p->parent;
            p2->next = new_parents;
            new_parents = p2;
        }
        p = p->next;
    }
    block->parents = new_parents;
}

void merge_dummy_children(struct basic_block_t *block)
{
    struct three_addr_t *next = block->first;
    while (next != NULL) {
        if (next->type == THREE_ADDR_T_BRANCH) {
            next->next_b1 = get_child_nondummy_block(next->next_b1);
            next->next_b2 = get_child_nondummy_block(next->next_b2);
        } else if (next->next_b1 != NULL) {
            next->next_b1 = get_child_nondummy_block(next->next_b1);
        }
        next = next->next;
    }
}

void merge_dummy_3_addrs(void)
{
    int i;
    for (i = 0; i < seen_blocks_count; i++) {
        merge_dummy_3_addr(seen_blocks[i]);
    }
}

void merge_dummy_blocks(void)
{
    int i;
    for (i = 0; i < seen_blocks_count; i++) {
        merge_dummy_children(seen_blocks[i]);
    }
    for (i = 0; i < seen_blocks_count; i++) {
        merge_dummy_parents(seen_blocks[i]);
    }
}

/*
 * Print functions
 */

void print_hashval(int hashval)
{
    if (seen_var(hashval)) {
        return;
    }
    mark_var_seen(hashval);
    char *str = get_hashval_name(hashval);
    if (!is_int(str)) {
        printf("%s\n", str);
    }
}

void print_vars_in_three_addr(struct three_addr_t *ta)
{
    switch(ta->type) {
    case THREE_ADDR_T_ASSIGN:
        switch(ta->op) {
        case OP_PLUS:
        case OP_MINUS:
        case OP_EQUAL:
        case OP_NOTEQUAL:
        case OP_LT:
        case OP_GT:
        case OP_LE:
        case OP_GE:
        case OP_SLASH:
        case OP_STAR:
        case OP_MOD:
        case OP_OR:
        case OP_AND:
            print_hashval(ta->op2);
        case OP_ASSIGNMENT:
        case OP_NOT:
            print_hashval(ta->LHS);
            print_hashval(ta->op1);
            break;
        default:
            break;
        }
        break;
    case THREE_ADDR_T_BRANCH:
        print_hashval(ta->op1);
        break;
    case THREE_ADDR_T_DUMMY:
        break;
    default:
        break;
    }
}

void print_vars_seen_in_block(struct basic_block_t *block)
{
    struct three_addr_t *next = block->first;
    while (next != NULL) {
        print_vars_in_three_addr(next);
        next = next->next;
    }
}

void print_vars_seen(void)
{
    int i;
    for (i = 0; i < seen_blocks_count; i++) {
        print_vars_seen_in_block(seen_blocks[i]);
    }
}

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

void print_block_parents(struct basic_block_t *block)
{
    printf("Parents of %d: ", block->unique_id);
    struct parent_node_t *p = block->parents;
    while (p != NULL) {
        printf("%d", p->parent->unique_id);
        p = p->next;
        if (p != NULL) {
            printf(", ");
        }
    }
    printf("\n");
}

void print_block_children(struct basic_block_t *block)
{
    struct three_addr_t *next = block->first;
    int first = 1;
    printf("Children of %d: ", block->unique_id);
    while (next != NULL) {
        if (next->type == THREE_ADDR_T_BRANCH) {
            if (next->next_b1 != NULL) {
                if (! first) {
                    printf(", ");
                }
                first = 0;
                printf("%d", next->next_b1->unique_id);
            }
            if (next->next_b2 != NULL) {
                if (! first) {
                    printf(", ");
                }
                first = 0;
                printf("%d", next->next_b2->unique_id);
            }
        } else if (next->next == NULL) {
            if (next->next_b1 != NULL) {
                if (! first) {
                    printf(", ");
                }
                first = 0;
                printf("%d", next->next_b1->unique_id);
            }
        }
        next = next->next;
    }
    printf("\n");
}

void print_children_blocks(void)
{
    int i;
    for (i = 0; i < seen_blocks_count; i++) {
        print_block_children(seen_blocks[i]);
    }
}

void print_parent_blocks(void)
{
    int i;
    for (i = 0; i < seen_blocks_count; i++) {
        print_block_parents(seen_blocks[i]);
    }
}

void print_program(void)
{
    struct cfg_t *cfg = program->cl->cb->fdl->fd->fb->cfg;
    struct basic_block_t *block = cfg->first;

    // Get a list of all seen blocks
    traverse_block(block);
    
    // Fix up dummy 3addr and blocks
    merge_dummy_3_addrs();
    merge_dummy_blocks();

    // Re-traverse which will no longer list dummy blocks
    clear_blocks_seen();
    traverse_block(block);

    // Constant evaluation
    eval_constants();

    // Print variables used in code

    printf("\n\
####################\n\
   Variables used\n\
####################\n");

    print_vars_seen();

    // Generate 3-address code

    printf("\n\
####################\n\
 Three Address Code\n\
####################\n");

    print_blocks();

    // Print blocks and their parents

    printf("\n\
####################\n\
    Parent blocks\n\
####################\n");

    print_parent_blocks();

    // Print blocks and their children

    printf("\n\
####################\n\
   Children blocks\n\
####################\n");

    print_children_blocks();
}

/*
 * Init functions
 */

void init_cfg(void)
{
    seen_blocks = (struct basic_block_t**) malloc(sizeof(struct basic_block_t*) * seen_blocks_size);
    seen_vars = (int*) malloc(sizeof(int) * seen_vars_size);
    const_vars = (struct three_addr_t**) malloc(sizeof(struct three_addr_t*) * const_vars_size);
}
