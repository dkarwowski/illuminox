#ifndef _MEMORY_h_
#define _MEMORY_h_

#include "config.h"

struct Stack;

struct LocalStack {
    struct Stack *stack;
    size_t used;
};

/* general stacks */
struct Stack *Z_NewStack(void *base, size_t size);
struct Stack *Z_NewSubStack(struct Stack *master, size_t size);
void          Z_ClearStack(struct Stack *stack);
size_t        Z_RemainingStack(struct Stack *stack);

/* local stack for functions */
void Z_BeginLocalStack(struct LocalStack *lstack, struct Stack *stack);
void Z_EndLocalStack(struct LocalStack *lstack);

/* zero areas of memory */
void    Z_ZeroSize(void *base, size_t size);
#define Z_ZeroStruct(instance)    ZeroSize(&instance, sizeof(instance))
#define Z_ZeroArray(array, count) ZeroSize((void *)array, sizeof(array[0])*count)

/* malloc an area of memory */
void *  Z_PushSize_(struct Stack *stack, size_t size, bool clear);
#define Z_PushStruct(stack, type, ...)       (type *)Z_PushSize_(stack, sizeof(type), ## __VA_ARGS__ )
#define Z_PushArray(stack, type, count, ...) (type *)Z_PushSize_(stack, sizeof(type)*count, ## __VA_ARGS__ )

/* push memory onto the stack */
void *  Z_PushCopy_(struct Stack *stack, void *src, size_t size);
#define Z_PushCopyStruct(stack, src, type)       (type *)Z_PushCopy_(stack, (void *)src, sizeof(type))
#define Z_PushCopyArray(stack, src, type, count) (type *)Z_PushCopy_(stack, (void *)src, sizeof(type)*count)

#endif
