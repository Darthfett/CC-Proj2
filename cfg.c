#include "cfg.h"

extern struct program_t *program;
extern int error_flag;

void print_three_addr(struct three_addr_t *ta) {
    switch (ta->type) {
    case THREE_ADDR_T_ASSIGN:
        switch(ta->op) {
        case OP_ASSIGNMENT:
            printf("%d = %d;\n", ta->LHS, ta->op1);
            break;
        case OP_PLUS:
            printf("%d = %d + %d;\n", ta->LHS, ta->op1, ta->op2);
            break;
        case OP_MINUS:
            printf("%d = %d - %d;\n", ta->LHS, ta->op1, ta->op2);
            break;
        case OP_EQUAL:
            printf("%d = %d == %d;\n", ta->LHS, ta->op1, ta->op2);
            break;
        case OP_NOTEQUAL:
            printf("%d = %d != %d;\n", ta->LHS, ta->op1, ta->op2);
            break;
        case OP_LT:
            printf("%d = %d < %d;\n", ta->LHS, ta->op1, ta->op2);
            break;
        case OP_GT:
            printf("%d = %d > %d;\n", ta->LHS, ta->op1, ta->op2);
            break;
        case OP_LE:
            printf("%d = %d <= %d;\n", ta->LHS, ta->op1, ta->op2);
            break;
        case OP_GE:
            printf("%d = %d >= %d;\n", ta->LHS, ta->op1, ta->op2);
            break;
        case OP_NOT:
            printf("%d = !%d;\n", ta->LHS, ta->op1);
            break;
        default:
            printf("Invalid 3-address assignment op <%d>\n", ta->op);
        }
        break;
    case THREE_ADDR_T_BRANCH:
        printf("branch(%d, %p, %p);\n", ta->op1, ta->next_b1, ta->next_b2);
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
        next = next->next;
        i++;
    }
}
