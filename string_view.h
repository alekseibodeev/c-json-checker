#ifndef STRING_VIEW_H_
#define STRING_VIEW_H_

#include <stddef.h>

typedef struct {
    const char *data;
    size_t      size;
} StringView;

/* Create String View from null-terminated C string */
#define SV(cs) (StringView) { cs, sizeof(cs) - 1 }

#endif
