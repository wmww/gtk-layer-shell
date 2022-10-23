/* This entire file is licensed under MIT
 *
 * Copyright 2020 Sophie Winter
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_OUTPUT_WIDTH 1920
#define DEFAULT_OUTPUT_HEIGHT 1080

#define FATAL_FMT(format, ...) do {fprintf(stderr, "Fatal error at %s:%d in %s(): " format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); exit(1);} while (0)
#define FATAL(message) FATAL_FMT(message"%s", "")
#define ASSERT(assertion) do {if (!(assertion)) {FATAL_FMT("\n  assertion failed: %s", #assertion);}} while (0)
#define ASSERT_EQ(a, b, format) do {if (!((a) == (b))) {FATAL_FMT("\n  expected: %s == %s\n  actual:   " format " != " format "\n", #a, #b, a, b);}} while (0)
#define ASSERT_STR_EQ(a, b) do {if (strcmp(a, b)) {FATAL_FMT("\n  expected: %s ≈ %s\n  actual:   \"%s\" ≠ \"%s\"\n", #a, #b, a, b);}} while (0)

#endif // TEST_COMMON_H

