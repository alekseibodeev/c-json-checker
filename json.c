#include <ctype.h>

#include "json.h"
#include "scanner.h"
#include "string_view.h"

static void
consume_spaces(Scanner *scanner)
{
    while (scanner->pos < scanner->size &&
           isspace(scanner->data[scanner->pos]))
    {
        scanner->pos++;
    }
}

static int
consume_str(Scanner *scanner, StringView pattern)
{
    if (pattern.size > scanner->size - scanner->pos) {
        return 1;
    }

    for (int i = 0; i < pattern.size; ++i) {
        if (scanner->data[scanner->pos + i] != pattern.data[i]) {
            return 1;
        }
    }

    scanner->pos += pattern.size;
    return 0;
}

static int
consume_true(Scanner *scanner)
{
    return consume_str(scanner, SV("true"));
}

static int
consume_end(Scanner *scanner)
{
    return !(scanner->pos == scanner->size);
}

/*
 * Test whether string s is a valid JSON value
 *
 * s: string to test
 * returns: 0 on success
 *          1 on failure
 */
int
json_validate(StringView s)
{
    Scanner scanner = { s.data, s.size, 0 };

    consume_spaces(&scanner);

    if (consume_true(&scanner)) {
        return 1;
    }

    consume_spaces(&scanner);

    return consume_end(&scanner);
}
