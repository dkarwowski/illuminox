#include "memory.h"

struct Stack {
    u8 *base;
    size_t size;
    size_t used;

    u32 count;
};

/**
 * Initialize a stack passed in
 *
 * @stack : struct to intialize
 * @base  : the start of the stack
 * @size  : how big the stack is
 *
 * Stack should be a valid pointer
 */
static
void
Z_InitStack(struct Stack *stack, void *base, size_t size)
{
    ASSERT(stack);
    stack->base  = (u8 *)base;
    stack->size  = size;
    stack->used  = 0;
    stack->count = 0;
}

/**
 * Initialize a slave stack in the master.
 *
 * @slave : stack to initialize
 * @master : initialize within this stack
 * @size : how big the slave should be
 *
 * All should be valid pointers
 */
static
void
Z_InitSubStack(struct Stack *slave, struct Stack *master, size_t size)
{
    ASSERT(slave && master);
    ASSERT(master->used + size <= master->size);
    Z_InitStack(slave, master->base + master->used, size);
    master->used += size;
}

/**
 * Create a new stack at the given address
 *
 * @base : where to create the stack
 * @size : how big the stack should be
 *
 * The @size needs to be bigger than the size of a stack
 */
struct Stack *
Z_NewStack(void *base, size_t size)
{
    ASSERT(size > sizeof(struct Stack));
    struct Stack *result = base;
    Z_InitStack(result, (u8 *)base + sizeof(struct Stack), size - sizeof(struct Stack));
    return result;
}

/**
 * Create a new sub stack in the master
 *
 * @master : where the sub stack will exist
 * @size   : how big the sub stack should be
 *
 * The @size should be bigger than a stack
 */
struct Stack *
Z_NewSubStack(struct Stack *master, size_t size)
{
    ASSERT(size > sizeof(struct Stack));
    struct Stack *slave = Z_PushStruct(master, struct Stack, true);
    Z_InitSubStack(slave, master, size - sizeof(struct Stack));
    return slave;
}

/**
 * Clear the contents of a stack
 *
 * @stack : what to clear
 */
void
Z_ClearStack(struct Stack *stack)
{
    ASSERT(stack);
    Z_InitStack(stack, stack->base, stack->size);
}

/**
 * Get how big the remaining stack is
 *
 * @stack : what to check
 */
size_t
Z_RemainingStack(struct Stack *stack)
{
    ASSERT(stack);
    size_t result = stack->size - stack->used;
    return result;
}

/**
 * Initialize a local stack within the stack
 *
 * @lstack : to initialize
 * @stack  : where to build the local stack
 */
void
Z_BeginLocalStack(struct LocalStack *lstack, struct Stack *stack)
{
    ASSERT(lstack && stack);
    lstack->stack = stack;
    lstack->used  = stack->used;
    stack->count++;
}

/**
 * Clear and remove a local stack
 *
 * @lstack: local stack to clear
 */
void
Z_EndLocalStack(struct LocalStack *lstack)
{
    ASSERT(lstack && lstack->stack);
    struct Stack *stack = lstack->stack;

    /* problem if locally removed more than started with */
    ASSERT(stack->used >= lstack->used);
    stack->used = lstack->used;

    ASSERT(stack->count > 0);
    stack->count--;
}

/**
 * Zero a chunk of memory
 *
 * @base : memory to start at
 * @size : how much should be cleared
 *
 * Note that this sets bits to 0, not values
 */
void
Z_ZeroSize(void *base, size_t size)
{
    char *byte = (char *)base;
    while (size--)
        *(byte++) = 0;
}

/**
 * Allocate an amount of memory onto the stack
 *
 * @stack : where to allocate
 * @size  : how big the chunk should be
 * @clear : Whether or not to set everything to 0
 * 
 * @return : pointer to the memory that was allocated
 */
void *
Z_PushSize_(struct Stack *stack, size_t size, bool clear)
{
    ASSERT(stack);
    ASSERT((stack->used + size) <= stack->size);

    void *result = stack->base + stack->used;
    stack->used += size;

    if (clear)
        Z_ZeroSize(result, size);

    return result;
}

/**
 * Push memory onto the stack
 *
 * @stack : where to allocate
 * @src   : memory to be pushed onto the stack
 * @size  : how much should be pushed
 *
 * @return : pointer to the newly allocated memory
 */
void *
Z_PushCopy_(struct Stack *stack, void *src, size_t size)
{
    ASSERT(src);
    ASSERT((stack->used + size) <= stack->size);

    void *result = stack->base + stack->used;
    stack->used += size;

    char *dest = (char *)result;
    char *bsrc = (char *)src;

    while (size--)
        *(dest++) = *(bsrc++);

    return result;
}

