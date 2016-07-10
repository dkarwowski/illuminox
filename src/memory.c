#include "memory.h"

void
Z_InitStack(struct Stack *stack, void *base, size_t size)
{
    stack->base  = (u8 *)base;
    stack->size  = size;
    stack->used  = 0;
    stack->count = 0;
}

void
Z_InitSubStack(struct Stack *slave, struct Stack *master, size_t size)
{
    ASSERT(master->used + size <= master->size);
    Z_InitStack(slave, master->base + master->used, size);
    master->used += size;
}

void
Z_ClearStack(struct Stack *stack)
{
    Z_InitStack(stack, stack->base, stack->size);
}

size_t
Z_RemainingStack(struct Stack *stack)
{
    size_t result = stack->size - stack->used;
    return result;
}

void
Z_BeginLocalStack(struct LocalStack *lstack, struct Stack *stack)
{
    lstack->stack = stack;
    lstack->used  = stack->used;
    stack->count++;
}

void
Z_EndLocalStack(struct LocalStack *lstack)
{
    struct Stack *stack = lstack->stack;

    /* problem if locally removed more than started with */
    ASSERT(stack->used >= lstack->used);
    stack->used = lstack->used;

    ASSERT(stack->count > 0);
    stack->count--;
}

void
Z_ZeroSize(void *base, size_t size)
{
    char *byte = (char *)base;
    while (size--)
        *(byte++) = 0;
}

void *
Z_PushSize_(struct Stack *stack, size_t size, bool clear)
{
    ASSERT((stack->used + size) <= stack->size);

    void *result = stack->base + stack->used;
    stack->used += size;

    if (clear)
        Z_ZeroSize(result, size);

    return result;
}

void *
Z_PushCopy_(struct Stack *stack, void *src, size_t size)
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

