#ifndef _MATH_h_
#define _MATH_h_

#include <math.h>
#include "config.h"

struct Vec2 {
    union {
        struct {
            r32 x, y;
        };
        struct {
            r32 w, h;
        };
        r32 e[2];
    };
};

static inline
struct Vec2
V2_Add(struct Vec2 a, struct Vec2 b)
{
    struct Vec2 result = { a.x + b.x, a.y + b.y };
    return result;
}

static inline
struct Vec2
V2_Sub(struct Vec2 a, struct Vec2 b)
{
    struct Vec2 result = { a.x - b.x, a.y - b.y };
    return result;
}

static inline
struct Vec2
V2_Neg(struct Vec2 a)
{
    struct Vec2 result = { -a.x, -a.y };
    return result;
}

static inline
struct Vec2
V2_Mul(r32 f, struct Vec2 a)
{
    struct Vec2 result = { f * a.x, f * a.y };
    return result;
}

static inline
struct Vec2
V2_Hadamard(struct Vec2 a, struct Vec2 b)
{
    struct Vec2 result = { a.x * b.x, a.y * b.y };
    return result;
}

static inline
r32
V2_Dot(struct Vec2 a, struct Vec2 b)
{
    r32 result = a.x * b.x + a.y * b.y;
    return result;
}

static inline
r32
V2_Wedge(struct Vec2 a, struct Vec2 b)
{
    r32 result = a.x * b.y - a.y * b.x;
    return result;
}

static inline
r32
V2_SqLen(struct Vec2 a)
{
    r32 result = V2_Dot(a, a);
    return result;
}

static inline
r32
V2_Len(struct Vec2 a)
{
    r32 result = sqrt(V2_SqLen(a));
    return result;
}

static inline
struct Vec2
V2_Norm(struct Vec2 a)
{
    r32 len = V2_Len(a);
    struct Vec2 result = (fabsf(len) > 0.0001f) ? V2_Mul(1.0f/len, a) : V2_Mul(0.0f, a);
    return result;
}

struct Vec3 {
    union {
        struct {
            r32 x, y, z;
        };
        r32 e[3];
    };
};

static inline
struct Vec3
V3_Add(struct Vec3 a, struct Vec3 b)
{
    struct Vec3 result = { a.x + b.x, a.y + b.y, a.z + b.z };
    return result;
}

static inline
struct Vec3
V3_Sub(struct Vec3 a, struct Vec3 b)
{
    struct Vec3 result = { a.x - b.x, a.y - b.y, a.z - b.z };
    return result;
}

static inline
struct Vec3
V3_Neg(struct Vec3 a)
{
    struct Vec3 result = { -a.x, -a.y, -a.z };
    return result;
}

static inline
struct Vec3
V3_Mul(r32 f, struct Vec3 a)
{
    struct Vec3 result = { f * a.x, f * a.y, f * a.z };
    return result;
}

static inline
struct Vec3
V3_Hadamard(struct Vec3 a, struct Vec3 b)
{
    struct Vec3 result = { a.x * b.x, a.y * b.y, a.z * b.z };
    return result;
}

#endif
