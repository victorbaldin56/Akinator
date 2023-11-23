#include "stack.h"

Stack *StackRealloc(Stack *stk, size_t newcap) {
    STACK_ASS(stk);

    if (stk->capacity == newcap) {
        return stk; // do nothing
    }

    Canary_t *new_data = (Canary_t *)realloc(stk->lc, newcap * sizeof(Elem_t) + 2 * sizeof(Canary_t));

    if (!new_data) {
        return NULL; // error: could not reallocate memory
    }

    stk->lc = new_data;

    stk->data = (Elem_t *)(stk->lc + 1);

    if (stk->capacity < newcap) {
        // initializing the rest of the memory
        memset(stk->data + stk->capacity, 0, (newcap - stk->capacity) * sizeof(Elem_t));
    }

    stk->capacity = newcap;
    stk->rc = (Canary_t *)(stk->data + stk->capacity);
    *stk->rc = CAN_VAL;
    return stk;
}
