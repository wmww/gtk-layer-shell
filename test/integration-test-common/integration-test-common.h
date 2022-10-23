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

#ifndef TEST_CLIENT_COMMON_H
#define TEST_CLIENT_COMMON_H

#include "gtk-layer-shell.h"
#include "test-common.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>
#include <stdio.h>

// Tell the test script that a request containing the given space-separated components is expected
#define EXPECT_MESSAGE(message) fprintf(stderr, "EXPECT: %s\n", #message)
// Tell the test script that all expected messages should now be fufilled
// (called automatically before each callback and at the end of the test)
#define CHECK_EXPECTATIONS() fprintf(stderr, "CHECK EXPECTATIONS COMPLETED\n")

// NULL-terminated list of callbacks that will be called before quiting
// Should be defined in the test file using TEST_CALLBACKS()
extern void (* test_callbacks[])(void);

// Input is a sequence of callback names with a trailing comma
#define TEST_CALLBACKS(...) void (* test_callbacks[])(void) = {__VA_ARGS__ NULL};

GtkWindow* create_default_window();

#endif // TEST_CLIENT_COMMON_H
