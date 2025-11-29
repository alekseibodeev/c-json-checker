#ifndef SCANNER_H_
#define SCANNER_H_

#include <stddef.h>

typedef struct {
    const char *data;
    size_t      size;
    size_t      pos;
} Scanner;

#endif
