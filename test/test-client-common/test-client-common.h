/* This entire file is licensed under MIT
 *
 * Copyright 2020 William Wold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef TEST_CLIENT_COMMON
#define TEST_CLIENT_COMMON

#include "gtk-layer-shell.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>
#include <stdio.h>

// Tell the test script that a request containing the given space-separated components is expected
#define EXPECT_MESSAGE(message) printf("WL: %s\n", #message)

// Test failures quit GTK main and set a non-zero return code, but let GTK shut down instead of exiting immediately
// do {...} while (0) construct is used to force ; at the end of lines
#define FAIL_TEST_FMT(format, ...) do {fprintf(stderr, "Failure at %s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__); mark_test_failed();} while (0)
#define FAIL_TEST(message) FAIL_TEST_FMT(message"%s", "")
#define ASSERT(assertion) do {if (!(assertion)) {FAIL_TEST_FMT("assertion failed: %s", #assertion);}} while (0)

// Implemented in test-client-common.c
void add_quit_timeout();
void setup_window(GtkWindow* window);
void mark_test_failed();
void wayland_roundtrip();

// Implemented in the test
void emit_expectations();
void run_test();

#endif // TEST_CLIENT_COMMON
