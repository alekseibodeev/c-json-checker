/*
 * json-checker - determines whether the given file is a valid JSON
 *
 * Accept one optional argument - path to a file,
 * if no file provided, reads from standard input
 *
 * The checker does not output anything to standard output
 * it returns 0 status code on success and 1 on failure
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "read_file.h"
#include "string_view.h"

int
main(int argc, char **argv)
{
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        exit(1);
    }

    FILE *fp = stdin;

    if (argc == 2) {
        fp = fopen(argv[1], "r");

        if (fp == NULL) {
            fprintf(stderr, "ERROR: can't open \"%s\"\n", argv[1]);
            exit(1);
        }
    }

    StringView s = read_file(fp);
    int status = json_check(s);

    free((void *) s.data);
    fclose(fp);
    return status;
}
