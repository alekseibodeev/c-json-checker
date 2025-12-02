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
    /* Keyword tests */
    test("true keyword",
         json_check(SV("true")),
         0);
    test("false keyword",
         json_check(SV("false")),
         0);
    test("null keyword",
         json_check(SV("null")),
         0);
    test("concatenated keywords",
         json_check(SV("truefalse")),
         1);

    /* String tests */
    /* Valid */
    test("empty string",
         json_check(SV("\"\"")),
         0);
    test("simple string",
         json_check(SV("\"text\"")),
         0);
    test("escape quotation mark",
         json_check(SV("\"\\\"\"")),
         0);
    test("escape reverse solidus",
         json_check(SV("\"\\\\\"")),
         0);
    test("escape solidus",
         json_check(SV("\"\\/\"")),
         0);
    test("escape backspace",
         json_check(SV("\"\\b\"")),
         0);
    test("escape formeed",
         json_check(SV("\"\\f\"")),
         0);
    test("escape linefeed",
         json_check(SV("\"\\n\"")),
         0);
    test("escape carriage return",
         json_check(SV("\"\\r\"")),
         0);
    test("escape horizontal tab",
         json_check(SV("\"\\t\"")),
         0);
    test("escape unicode hex sequence",
         json_check(SV("\"\\u0411\"")),
         0);
    /* Invalid */
    test("unterminated string",
         json_check(SV("\"text")),
         1);
    test("invalid escape character",
         json_check(SV("\"\\a\"")),
         1);
    test("non-hex digits in unicode sequence",
         json_check(SV("\"\\uxxxx\"")),
         1);
    test("less that 4 hex digits in unicode sequence",
         json_check(SV("\"\\u041\"")),
         1);

    /* Number tests */
    /* Valid */
    test("positive integer",
         json_check(SV("42")),
         0);
    test("negative integer",
         json_check(SV("-42")),
         0);
    test("zero",
         json_check(SV("0")),
         0);
    test("negative zero",
         json_check(SV("-0")),
         0);
    test("float",
         json_check(SV("42.5")),
         0);
    test("negative float number",
         json_check(SV("-42.5")),
         0);
    test("float with trailing zeroes",
         json_check(SV("42.500")),
         0);
    test("float with leading zero",
         json_check(SV("0.5")),
         0);
    test("exponent",
         json_check(SV("42e05")),
         0);
    test("exponent capitalized",
         json_check(SV("42E05")),
         0);
    test("positive sign exponent",
         json_check(SV("42e+05")),
         0);
    test("negative sign exponent",
         json_check(SV("42e-05")),
         0);
    /* Invalid */
    test("integer with leading zero",
         json_check(SV("042")),
         1);
    test("missing fraction",
         json_check(SV("42.")),
         1);
    test("missing exponent",
         json_check(SV("42e")),
         1);
    test("double unary minus",
         json_check(SV("--42")),
         1);
    test("two floating points in one number",
         json_check(SV("42.0.1")),
         1);

    /* Array tests */
    /* Valid */
    test("empty array",
         json_check(SV("[]")),
         0);
    test("array of one element",
         json_check(SV("[0]")),
         0);
    test("array of multiple elements",
         json_check(SV("[0, \"test\", null, true, false]")),
         0);
    test("nested array",
         json_check(SV("[[0]]")),
         0);
    test("deeply nested array",
         json_check(SV("[[[[[[[[[[[[[[[[0]]]]]]]]]]]]]]]]")),
         0);
    test("empty array with spaces inside",
         json_check(SV("[          ]")),
         0);
    test("array of one element with spaces around brackets",
         json_check(SV("  [  0  ]  ")),
         0);
    test("array of two element with spaces around comma",
         json_check(SV("[ 1  ,  2]")),
         0);
    /* Invalid */
    test("array with unbalanced brackets",
         json_check(SV("[[[[[[[[[[[[[[[[0")),
         1);
    test("array with no comma separator",
         json_check(SV("[1 2]")),
         1);

    /* Object tests */
    /* Valid */
    test("empty object",
         json_check(SV("{}")),
         0);
    test("object of one entry",
         json_check(SV("{\"key\": \"value\"}")),
         0);
    test("object of multiple entries",
         json_check(SV("{\"key1\": true,\n\"key2\": false,\n\"key3\": \"value\"}")),
         0);
    test("nested object",
         json_check(SV("{\"key1\": {\"key2\": {}}}")),
         0);
    test("empty object with spaces around curlies",
         json_check(SV("  {     }  ")),
         0);
    test("object with spaces around key and value",
         json_check(SV("{   \"key\"   : \"value\"   }")),
         0);
    /* Invalid */
    test("object with no key",
         json_check(SV("{0}")),
         1);
    test("object with unbalanced curlies",
         json_check(SV("{\"key\": \"value\"")),
         1);
    test("object with no colon",
         json_check(SV("{\"key\" \"value\"}")),
         1);

    /* Structure tests */
    test("JSON value has leading spaces",
         json_check(SV("  true")),
         0);
    test("JSON value has trailing spaces",
         json_check(SV("true  ")),
         0);
    test("JSON value has both leading and trailing spaces",
         json_check(SV(" true ")),
         0);

    return 0;
}
