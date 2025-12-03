#include <stdio.h>
#include <stdlib.h>

#include "string_view.h"

#define RF_BUFFER_BASE_CAPACITY 128

#define RF_ASSERT_ALLOC(ptr) \
    if (ptr == NULL) { \
        fprintf(stderr, "ERROR: memory allocation failed successfully.\n"); \
        exit(1); \
    }

/*
 * Read file content into string view
 * User is in charge of freeing memory allocated for the string buffer
 *
 * fp: file pointer
 * returns: string view (data block allocated dynamically)
 */
StringView
read_file(FILE *fp)
{
    char *buffer = malloc(RF_BUFFER_BASE_CAPACITY);
    size_t capacity = RF_BUFFER_BASE_CAPACITY;
    size_t size = 0;
    int c;

    RF_ASSERT_ALLOC(buffer);

    while ((c = fgetc(fp)) != EOF) {
        if (size + 1 == capacity) {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
            RF_ASSERT_ALLOC(buffer);
        }

        buffer[size++] = c;
    }

    buffer[size++] = '\0';
    buffer = realloc(buffer, size);
    RF_ASSERT_ALLOC(buffer);

    return (StringView) { buffer, size - 1 };
}
