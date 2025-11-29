/* Simple test runner */

#include <stdio.h>

#include "json.h"
#include "string_view.h"

#define ASNI_COLOR_RED   "\x1b[31m"
#define ASNI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define SET_OUTPUT_COLOR(actual, expected) \
    ((actual) == (expected) ? ASNI_COLOR_GREEN : ASNI_COLOR_RED)

#define SET_OUTPUT_PREFIX(actual, expected) \
    ((actual) == (expected) ? "PASSED" : "FAILED")

/*
 * Test actual value against expected value
 * Print formatted result message to stdout
 *
 * message: test description
 * actual: actual value
 * expected: expected value
 * returns: 0 on success
 *          1 on failure
 */
int
test(const char *message, int actual, int expected)
{
    printf("%s%s: %s%s\n",
           SET_OUTPUT_COLOR(actual, expected),
           SET_OUTPUT_PREFIX(actual, expected),
           message,
           ANSI_COLOR_RESET);
    return !(actual == expected);
}

int
main(int argc, char **argv)
{
    test("true keyword", json_validate(SV("true")), 0);
    test("false keyword", json_validate(SV("false")), 0);
    test("null keyword", json_validate(SV("null")), 0);
    test("JSON keyword is part of invalid string",
         json_validate(SV("truefalse")),
         1);
    test("JSON value has leading spaces", json_validate(SV("  true")), 0);
    test("JSON value has trailing spaces", json_validate(SV("true  ")), 0);
    test("JSON value has both leading and trailing spaces",
         json_validate(SV(" true ")),
         0);
    test("simple string", json_validate(SV("\"text\"")), 0);
    test("string with escaped quotation mark",
         json_validate(SV("\"te\\\"xt\"")),
         0);
    test("string with escaped escape character before quotation mark",
         json_validate(SV("\"te\\\\\"xt\"")),
         1);
    test("positive integer number", json_validate(SV("42")), 0);
    test("negative integer number", json_validate(SV("-42")), 0);
    test("zero number", json_validate(SV("0")), 0);
    test("negative zero number", json_validate(SV("-0")), 0);
    test("integer with leading zero", json_validate(SV("042")), 1);
    test("positive float number", json_validate(SV("42.5")), 0);
    test("negative float number", json_validate(SV("-42.5")), 0);
    test("float number with trailing zeroes", json_validate(SV("42.500")), 0);
    test("empty array", json_validate(SV("[]")), 0);
    test("array with one value", json_validate(SV("[100]")), 0);
    test("array with multiple elements",
         json_validate(SV("[100, \"hello\", null]")),
         0);
    return 0;
}
