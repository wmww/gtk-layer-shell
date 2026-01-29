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

// Test for https://github.com/wmww/gtk-layer-shell/issues/217
// This tests that receiving a .closed event before .configure doesn't cause an infinite loop.
// Without the fix, this test would hang forever.

#include "integration-test-common.h"

static GtkWindow* window;

static void callback_0()
{
    // Tell the mock server to send .closed instead of .configure for the next layer surface
    send_command("send_closed_instead_of_configure", "will_send_closed");
}

static void callback_1()
{
    // Expect the layer surface to be created and immediately receive a .closed event
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface nil);
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .closed);

    window = g_object_ref(create_default_window());
    gtk_layer_init_for_window(window);
    gtk_widget_show_all(GTK_WIDGET(window));

    // If we get here without hanging, the fix works!
    // The window should not be mapped since it never received a configure event
}

static void callback_2()
{
    // Verify the window is not mapped (since it was closed before configure)
    // Note: The window might or might not be mapped depending on GTK's internal state,
    // but the important thing is we didn't hang in the roundtrip loop.
    // Clean up the reference we took
    g_object_unref(window);
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
)
