#ifndef STACK
#define STACK

// #define HASH_PROTECT
// #define NDEBUG

//#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(...) __VA_ARGS__

#else
#define ON_DEBUG(...) ;
#endif // NDEBUG

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>

#ifndef NDEBUG

#define STACK_ASS(stk)                                          \
    if (StackCheck(stk)) {                                      \
        StackDump(stk, StackCheck(stk), __FILE__, __LINE__);    \
        abort();                                                \
    }

#else
#define STACK_ASS(stk) ;
#endif

#define my_assert(expr) \
    if (!(expr)) {  \
        fprintf(stderr, "%s %s:%d: My assertion \"" #expr "\" failed\n", __FILE__, __func__, __LINE__); \
        abort(); \
}

#define GETVNAME(var) (#var)

typedef const void * Elem_t;
typedef unsigned long long Canary_t;

#define PRINTFFMT "%p"

/**
 * @brief Holds stack
*/
typedef struct {
    Canary_t *lc;               ///< left protection boarder ("canary")
    Elem_t *data;               ///< buffer to store stack data
    ssize_t size;               ///< current position in stack
    ssize_t capacity;           ///< capacity of the buffer
    #ifdef HASH_PROTECT
    unsigned long long hash;    ///< hash sum
    #endif
    Canary_t *rc;               ///< right "canary"
} Stack;

const Canary_t CAN_VAL = 0xDEDEDEDEDEDEDEDE;

/**
 * Enum for error codes
*/
enum StackErr {
    STACK_OK,
    BIG_SIZE,
    NULL_PTR,
    BUFF_NULL,
    SIZE_NEGATIVE,
    CAP_NEGATIVE,
    LC_DEAD,
    RC_DEAD,
    #ifdef HASH_PROTECT
    HASH_DEAD,
    #endif
    REALLOC_FAILED,
    POP_EMPTY,
    // POP_VAL_NULL,
};

/**
 * @brief Checks if stack damaged
 * @param stk pointer to stack
 * @return Error code (0 if check succeed)
*/
StackErr StackCheck(const Stack *stk);

const size_t MAXLEN = 255; // max len for string functions

void StackDump(const Stack *stk, StackErr errcode, const char *file, int line);

const int INIT_CAP = 1;

/**
 * @brief Needed for full correct initializing of stack
 * @param stk pointer to newly created stack (must be preinitiazed with {}; otherwise there is no guarantee)
 * @return pointer to completely initialized stack
*/
Stack *StackCtor(Stack *stk);

/**
 * @brief Destructs the stack to prevent its reusage
*/
void StackDtor(Stack *stk);

const size_t COEFF = 2; // StackRealloc multiplies capacity to this value

/**
 * @brief Reallocates the stack buffer
 * @param stk pointer to stack
 * @param newcap needed capacity for new buffer
 * @return the pointer to the same stack; in case of error NULL
*/
Stack *StackRealloc(Stack *stk, size_t newcap);

/**
 * @brief Pushes the value to the stack
*/
StackErr Push(Stack *stk, Elem_t value);

/**
 * @brief Gets the last element of the stack
 * @param stk pointer to stack
 * @param value pointer to write the value
 * @return Error code; 0 if no errors
*/
StackErr Pop(Stack *stk, Elem_t *value);

#ifdef HASH_PROTECT
unsigned long long HashFunc(const Stack *stk);
#endif

// void *Recalloc(void *ptr, size_t oldsize, size_t newsize);

#endif // STACK
