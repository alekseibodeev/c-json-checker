#include <ctype.h>
#include <stdbool.h>

#include "json.h"
#include "scanner.h"
#include "string_view.h"

static int consume_json_value(Scanner *scanner);

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
consume_false(Scanner *scanner)
{
    return consume_str(scanner, SV("false"));
}

static int
consume_null(Scanner *scanner)
{
    return consume_str(scanner, SV("null"));
}

static int
consume_end(Scanner *scanner)
{
    return !(scanner->pos == scanner->size);
}

static int
consume_char(Scanner *scanner, char c)
{
    if (scanner->data[scanner->pos] != c) {
        return 1;
    }

    scanner->pos++;
    return 0;
}

static void
consume_string_val(Scanner *scanner)
{
    bool escaped = false;

    while (scanner->pos < scanner->size &&
           (escaped || scanner->data[scanner->pos] != '"'))
    {
        escaped = scanner->data[scanner->pos] == '\\' && !escaped;
        scanner->pos++;
    }
}

static int
consume_string(Scanner *scanner)
{
    if (consume_char(scanner, '"')) {
        return 1;
    }

    consume_string_val(scanner);
    return consume_char(scanner, '"');
}

static int
consume_number(Scanner *scanner)
{
    if (scanner->data[scanner->pos] == '-') {
        scanner->pos++;
    }

    if (!isdigit(scanner->data[scanner->pos])) {
        return 1;
    } else if (scanner->data[scanner->pos] == '0') {
        scanner->pos++;
    } else {
        while (isdigit(scanner->data[scanner->pos])) {
            scanner->pos++;
        }
    }

    if (scanner->data[scanner->pos] == '.') {
        scanner->pos++;

        if (!isdigit(scanner->data[scanner->pos])) {
            return 1;
        }

        while (isdigit(scanner->data[scanner->pos])) {
            scanner->pos++;
        }
    }

    return 0;
}

static int
consume_array(Scanner *scanner)
{
    if (consume_char(scanner, '[')) {
        return 1;
    }

    if (scanner->data[scanner->pos] != ']') {
        do {
            if (consume_json_value(scanner)) {
                return 1;
            }
        } while (consume_char(scanner, ',') == 0);
    }

    return consume_char(scanner, ']');
}

static int
consume_object(Scanner *scanner)
{
    if (consume_char(scanner, '{')) {
        return 1;
    }

    if (scanner->data[scanner->pos] != '}') {
        do {
            consume_spaces(scanner);

            if (consume_string(scanner) ||
                consume_char(scanner, ':') ||
                consume_json_value(scanner))
            {
                return 1;
            }
        } while (consume_char(scanner, ',') == 0);
    }

    return consume_char(scanner, '}');
}

static int
consume_json_value(Scanner *scanner)
{
    consume_spaces(scanner);

    if (consume_true(scanner) &&
        consume_false(scanner) &&
        consume_null(scanner) &&
        consume_string(scanner) &&
        consume_number(scanner) &&
        consume_array(scanner) &&
        consume_object(scanner))
    {
        return 1;
    }

    consume_spaces(scanner);

    return 0;
}

/*
 * Test whether string s is a valid JSON value
 *
 * s: string to test
 * returns: 0 on success
 *          1 on failure
 */
int
json_check(StringView s)
{
    Scanner scanner = { s.data, s.size, 0 };

    return consume_json_value(&scanner) || consume_end(&scanner);
}
