#include "stack.h"

int main() {
    printf("# A demo program for stack structure\n"
           "# (c) Victor Baldin, 2023\n\n");

    printf("Enter values to push; to stop input, press ctrl+D:\n");
    Stack stk = {};
    StackCtor(&stk);
    // StackCtor(&stk);
    // StackCtor(NULL);

    Elem_t value = 0;
    // stk.data = NULL;

    // for (Elem_t el = 0; el < 100000; el++) {
    //     Push(&stk, el);
    // }

    while (scanf(PRINTFFMT, &value) > 0) {
        Push(&stk, value);
    }

    StackDump(&stk, STACK_OK, __FILE__, __LINE__);

    while (Pop(&stk, &value) != POP_EMPTY) {
        printf("Pop: " PRINTFFMT "\n", value);
    }

    StackDtor(&stk);

    // Stack badstk = {};
    // StackCtor(&badstk);
    // badstk.size = -1;
    // STACK_ASS(&badstk);

    return 0;
}
