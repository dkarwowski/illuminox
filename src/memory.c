#include "memory.h"

void
InitStack(struct Stack *stack, void *base, size_t size)
{
    stack->base  = (u8 *)base;
    stack->size  = size;
    stack->used  = 0;
    stack->count = 0;
}

void
InitSubStack(struct Stack *slave, struct Stack *master, size_t size)
{
    ASSERT(master->used + size <= master->size);
    InitStack(slave, master->base + master->used, size);
    master->used += size;
}

void
ClearStack(struct Stack *stack)
{
    InitStack(stack, stack->base, stack->size);
}

size_t
RemainingStack(struct Stack *stack)
{
    size_t result = stack->size - stack->used;
    return result;
}

void
BeginLocalStack(struct LocalStack *lstack, struct Stack *stack)
{
    lstack->stack = stack;
    lstack->used  = stack->used;
    stack->count++;
}

void
EndLocalStack(struct LocalStack *lstack)
{
    struct Stack *stack = lstack->stack;

    /* problem if locally removed more than started with */
    ASSERT(stack->used >= lstack->used);
    stack->used = lstack->used;

    ASSERT(stack->count > 0);
    stack->count--;
}

void
ZeroSize(void *base, size_t size)
{
    char *byte = (char *)base;
    while (size--)
        *(byte++) = 0;
}

void *
PushSize_(struct Stack *stack, size_t size, bool clear)
{
    ASSERT((stack->used + size) <= stack->size);

    void *result = stack->base + stack->used;
    stack->used += size;

    if (clear)
        ZeroSize(result, size);

    return result;
}

void *
PushCopy_(struct Stack *stack, void *src, size_t size)
{
    ASSERT((stack->used + size) <= stack->size);

    void *result = stack->base + stack->used;
    stack->used += size;

    char *dest = (char *)result;
    char *bsrc = (char *)src;

    while (size--)
        *(dest++) = *(bsrc++);

    return result;
}

