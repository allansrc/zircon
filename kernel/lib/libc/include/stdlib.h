// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2008-2014 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef ZIRCON_KERNEL_LIB_LIBC_INCLUDE_STDLIB_H_
#define ZIRCON_KERNEL_LIB_LIBC_INCLUDE_STDLIB_H_

#include <endian.h>
#include <malloc.h>
#include <rand.h>
#include <stddef.h>
#include <sys/types.h>
#include <zircon/compiler.h>

#include <arch/defines.h>

__BEGIN_CDECLS

int atoi(const char *num);
unsigned int atoui(const char *num);
long atol(const char *num);
unsigned long atoul(const char *num);
unsigned long long atoull(const char *num);

long strtol(const char *nptr, char **endptr, int base);
long long strtoll(const char *nptr, char **endptr, int base);

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define ROUNDUP(a, b) (((a) + ((b)-1)) & ~((b)-1))
#define ROUNDDOWN(a, b) ((a) & ~((b)-1))

#define ALIGN(a, b) ROUNDUP(a, b)
#define IS_ALIGNED(a, b) (!(((uintptr_t)(a)) & (((uintptr_t)(b)) - 1)))

void abort(void) __attribute__((noreturn));
void qsort(void *aa, size_t n, size_t es, int (*cmp)(const void *, const void *));
void *bsearch(const void *key, const void *base, size_t num_elems, size_t size,
              int (*compare)(const void *, const void *));
unsigned long int strtoul(const char *nptr, char **endptr, int base);
char *getenv(const char *name);

__END_CDECLS

#endif  // ZIRCON_KERNEL_LIB_LIBC_INCLUDE_STDLIB_H_
