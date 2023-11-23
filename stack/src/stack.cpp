#include "stack.h"

static const char *err_messages[] = {
    "NO ERRORS",
    "BIG SIZE",
    "STACK IS NULL",
    "BUFFER PTR IS NULL",
    "SIZE IS NEGATIVE",
    "CAPACITY IS NEGATIVE",
    "LEFT CANARY DEAD",
    "RIGHT CANARY DEAD",
    #ifdef HASH_PROTECT
    "HASH DEAD",
    #endif
    "BUFFER REALLOC FAILED",
    "CANNOT POP: STACK IS EMPTY",
};


StackErr StackCheck(const Stack *stk) {
    if (!stk) {
        return NULL_PTR;
    }

    if (!stk->data) {
        return BUFF_NULL;
    }

    if (stk->capacity < stk->size) {
        return BIG_SIZE;
    }

    if (stk->size < 0) {
        return SIZE_NEGATIVE;
    }

    if (stk->capacity < 0) {
        return CAP_NEGATIVE;
    }

    my_assert(stk->lc);

    if (*stk->lc != CAN_VAL) {
        return LC_DEAD; // left canary protection
    }

    my_assert(stk->rc);

    if (*stk->rc != CAN_VAL) {
        return RC_DEAD; // right canary protection
    }

    // printf("canary check success\n");
    #ifdef HASH_PROTECT
    if (stk->hash != HashFunc(stk)) {
        printf("hash dead\n");
        // return HASH_DEAD;
    }
    #endif

    // printf("stack check success\n");

    return STACK_OK;
}

void StackDump(const Stack *stk, StackErr errcode, const char *file, int line) {
    system("mkdir logs\n");
    // char *dir = "logs/";
    time_t date = time(NULL);

    char filename[MAXLEN] = {};

    strftime(filename, sizeof(filename), "logs/%Y-%m-%d_%H:%M:%S.log", gmtime(&date));

    // snprintf(filename, MAXLEN, "logs/%s.log", asctime(gmtime(&date)));

    FILE *lf = fopen(filename, "w");

    printf("stack dump written to %s\n", filename);

    if (!errcode) {
        fprintf(lf, "STK is OK\n");
    }

    else {
        fprintf(lf, "ERROR in file %s, line %d:\n", file, line);
    }

    if (!stk) {
        fprintf(lf, "Stack pointer is NULL, file %s, line %d\n", file, line);
    }

    fprintf(lf, "size = %zd\n"
                "capacity = %zd\n"
                "data[%p]:\n",
                 stk->size,
                 stk->capacity,
                 stk->data);

    fprintf(lf, "left canary = %llx\n", *stk->lc);

    if (!stk->data) {
        fprintf(lf, "data is NULL\n");
        return;
    }

    for (ssize_t i = 0; i < stk->size; i++) {
        fprintf(lf, "*[%zd] = " PRINTFFMT "\n", i, stk->data[i]);
    }

    for (ssize_t i = stk->size; i < stk->capacity; i++) {
        fprintf(lf, "[%zd] = " PRINTFFMT "\n", i, stk->data[i]);
    }

    fprintf(lf, "right canary = %llx\n", *stk->rc);
    fprintf(lf, "errcode = %d\n (%s)", errcode, err_messages[errcode]);

    fclose(lf);
    lf = NULL;
}

Stack *StackCtor(Stack *stk) {
    // STACK_ASS(stk); // checks if stack is not OK
    my_assert(stk);
    my_assert(!stk->capacity);
    stk->capacity = INIT_CAP;
    stk->size = 0;

    stk->lc = (Canary_t *)calloc(2 * sizeof(Canary_t) + stk->capacity * sizeof(Elem_t), 1);

    if (!stk->lc) {
        return NULL;
    }

    stk->data = (Elem_t *)(stk->lc + 1);
    stk->rc = (Canary_t *)(stk->data + stk->capacity);

    // stk->rc = stk->data + stk->capacity;
    *stk->lc = *stk->rc = CAN_VAL;

    // printf("canaries assigned\n");
    STACK_ASS(stk); // checks if stk is not OK
    // printf("stk my_assertion success\n");
    #ifdef HASH_PROTECT
    stk->hash = 0;
    #endif

    // printf("Ctor returned successfully\n");
    return stk;
}

void StackDtor(Stack *stk) {
    STACK_ASS(stk);
    stk->size = stk->capacity = -1;
    free(stk->lc);
    stk->data = NULL;
    stk = NULL;
}

#ifdef HASH_PROTECT
unsigned long long HashFunc(const Stack *stk) {
    my_assert(stk);
    unsigned long long hash = 0;

    for (size_t i = 0; i < sizeof(Elem_t) * stk->capacity; i++) {
        hash += ((char *)stk->data)[i];
    }

    return hash;
}
#endif
