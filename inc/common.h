#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

#define fatalf(fmt, ...) (fprintf(stderr, "%s:%d: " fmt "\n", __FILE__, __LINE__, __VA_ARGS__), exit(1))
#define fatal(msg) fatalf("%s", msg)
#define unreachable() (fatal("unreachable"), __builtin_unreachable())

#define ROUNDDOWN(x, k) ((x) & -(k))
#define ROUNDUP(x, k)   (((x) + (k) - 1) & -(k))
#define MIN(a, b)       (a) < (b) ? (a) : (b)
#define MAX(a, b)       (a) < (b) ? (b) : (a)

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

#define _

#endif // COMMON_H
