#include "cfg.h"
#include "symtab.h"

extern struct program_t *program;
extern int error_flag;

void print_three_addr(struct three_addr_t *ta) {
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
        default:
            printf("Invalid 3-address assignment type <%d>, op <%d>, LHS <%s>, op1 <%s>, next <%p>, next_b1 <%p>\n", ta->type, ta->op, lhs, op1, ta->next, ta->next_b1);
        }
        break;
    case THREE_ADDR_T_BRANCH:
        printf("branch(%s, %p, %p);\n", op1, ta->next_b1, ta->next_b2);
        break;
    case THREE_ADDR_T_DUMMY:
        printf("No Op;\n");
        break;
    default:
        printf("Invalid 3-address code <%d>\n", ta->type);
        error_flag = 1;
    }
}

void print_program() {
    struct cfg_t *cfg = program->cl->cb->fdl->fd->fb->cfg;
    struct three_addr_t *next = cfg->first->first;
    int i = 1;

    while(next != NULL) {
        printf("%d: ", i);
        print_three_addr(next);
        if (next->next == NULL && next->next_b1 != NULL) {
            next = next->next_b1->first;
        } else {
            next = next->next;
        }
        i++;
    }
}
