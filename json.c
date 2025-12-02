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

    consume_spaces(scanner);

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
    consume_spaces(scanner);
    return !(scanner->pos == scanner->size);
}

static int
consume_char(Scanner *scanner, char c)
{
    consume_spaces(scanner);

    if (scanner->data[scanner->pos] != c) {
        return 1;
    }

    scanner->pos++;
    return 0;
}

static int
consume_hex_digit(Scanner *scanner)
{
    if (isdigit(scanner->data[scanner->pos]) ||
        (scanner->data[scanner->pos] >= 'A' &&
         scanner->data[scanner->pos] <= 'F') ||
        (scanner->data[scanner->pos] >= 'a' &&
         scanner->data[scanner->pos] <= 'f'))
    {
        scanner->pos++;
        return 0;
    }

    return 1;
}

static int
consume_unicode_escape(Scanner *scanner)
{
    return (consume_char(scanner, 'u') ||
            consume_hex_digit(scanner) ||
            consume_hex_digit(scanner) ||
            consume_hex_digit(scanner) ||
            consume_hex_digit(scanner));
}

static int
consume_escape(Scanner *scanner)
{
    return (consume_char(scanner, '"') &&
            consume_char(scanner, '\\') &&
            consume_char(scanner, '/') &&
            consume_char(scanner, 'b') &&
            consume_char(scanner, 'f') &&
            consume_char(scanner, 'n') &&
            consume_char(scanner, 'r') &&
            consume_char(scanner, 't') &&
            consume_unicode_escape(scanner));
}

static int
consume_string_val(Scanner *scanner)
{
    bool escaped = false;

    while (scanner->pos < scanner->size &&
           (escaped || scanner->data[scanner->pos] != '"'))
    {
        if (escaped) {
            if (consume_escape(scanner)) {
                return 1;
            }

            escaped = false;
        } else {
            if (scanner->data[scanner->pos] == '\\') {
                escaped = true;
            }

            scanner->pos++;
        }
    }

    return 0;
}

static int
consume_string(Scanner *scanner)
{
    return (consume_char(scanner, '"') ||
            consume_string_val(scanner) ||
            consume_char(scanner, '"'));
}

static int
consume_number(Scanner *scanner)
{
    consume_spaces(scanner);

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

    if (scanner->data[scanner->pos] == 'e' ||
        scanner->data[scanner->pos] == 'E')
    {
        scanner->pos++;

        if (scanner->data[scanner->pos] == '+' ||
            scanner->data[scanner->pos] == '-')
        {
            scanner->pos++;
        }

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

    consume_spaces(scanner);

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
    return (consume_true(scanner) &&
            consume_false(scanner) &&
            consume_null(scanner) &&
            consume_string(scanner) &&
            consume_number(scanner) &&
            consume_array(scanner) &&
            consume_object(scanner));
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
