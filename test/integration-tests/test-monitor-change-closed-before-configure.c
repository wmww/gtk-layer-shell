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
// This tests that the closed_before_initial_configure flag prevents infinite loops
// when monitor changes trigger remapping of a window that the compositor keeps closing.
// Without the fix, this would cause an infinite loop or signal cascade.

#include "integration-test-common.h"

static GtkWindow* window;

static void callback_0()
{
    // First, create a window and show it normally
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface nil);
    window = create_default_window();
    gtk_layer_init_for_window(window);
    // Don't set a specific monitor, so monitor changes will trigger remap
    gtk_widget_show_all(GTK_WIDGET(window));
}

static void callback_1()
{
    // Window is now shown and configured. Now set up the mock server to send
    // .closed instead of .configure for the next layer surface (which will be
    // created when monitor changes trigger a remap)
    send_command("send_closed_instead_of_configure", "will_send_closed");
}

static void callback_2()
{
    // The layer surface created by the remap should receive .closed
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface nil);
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .closed);

    // Add a new output, which triggers monitor-added signal
    // This should cause monitor_changed to call custom_shell_surface_remap
    // The remap creates a new layer surface, which receives .closed
    // Without the fix, this would loop forever
    send_command("create_output 800 600", "output_created");
}

static void callback_3()
{
    // If we get here, the fix works - we didn't hang in an infinite loop
    // The window should be unmapped since it received .closed before .configure
    ASSERT_EQ(gtk_widget_get_mapped(GTK_WIDGET(window)), FALSE, "%d");
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
    callback_3,
)
