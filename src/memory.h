#ifndef _MEMORY_h_
#include "config.h"

struct Stack {
    u8 *base;
    size_t size;
    size_t used;

    u32 count;
};

struct LocalStack {
    struct Stack *stack;
    size_t used;
};

/* general stacks */
void   InitStack(struct Stack *stack, void *base, size_t size);
void   InitSubStack(struct Stack *slave, struct Stack *master, size_t size);
void   ClearStack(struct Stack *stack);
size_t RemainingStack(struct Stack *stack);

/* local stack for functions */
void BeginLocalStack(struct LocalStack *lstack, struct Stack *stack);
void EndLocalStack(struct LocalStack *lstack);

/* zero areas of memory */
void    ZeroSize(void *base, size_t size);
#define ZeroStruct(instance)    ZeroSize(&instance, sizeof(instance))
#define ZeroArray(array, count) ZeroSize((void *)array, sizeof(array[0])*count)

/* malloc an area of memory */
void *  PushSize_(struct Stack *stack, size_t size, bool clear);
#define PushStruct(stack, type, ...)       (type *)PushSize_(stack, sizeof(type), ## __VA_ARGS__ )
#define PushArray(stack, type, count, ...) (type *)PushSize_(stack, sizeof(type)*count, ## __VA_ARGS__ )

/* push memory onto the stack */
void *  PushCopy_(struct Stack *stack, void *src, size_t size);
#define PushCopyStruct(stack, src, type)       (type *)PushCopy_(stack, (void *)src, sizeof(type))
#define PushCopyArray(stack, src, type, count) (type *)PushCopy_(stack, (void *)src, sizeof(type)*count)

#define _MEMORY_h_
#endif
