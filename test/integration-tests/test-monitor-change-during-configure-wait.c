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
// This tests that monitor changes during the initial configure wait don't cause
// nested map operations that could lead to state corruption or infinite loops.
//
// Strategy: Create an output, then immediately show a window. The window's roundtrip
// should process the pending output event, triggering monitor_changed. Without the fix,
// this causes nested map operations. With the fix, remap is skipped during configure wait.

#include "integration-test-common.h"

static GtkWindow* window;

static void callback_0()
{
    // Enable configure delay so the window's roundtrip has time to process other events
    send_command("enable_configure_delay", "configure_delay_enabled");
}

static void callback_1()
{
    // Create the window and prepare it
    window = create_default_window();
    gtk_layer_init_for_window(window);

    // Create a new output - the global announcement will be sent to the client
    // but not yet processed (no roundtrip yet)
    send_command("create_output 800 600", "output_created");

    // Show the window. During the roundtrip, the pending output event will be processed,
    // triggering monitor_changed. Without the fix (checking awaiting_initial_configure),
    // remap would be called, creating nested map operations.
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface nil);
    gtk_widget_show_all(GTK_WIDGET(window));
}

static void callback_2()
{
    // If we get here, we didn't hang
    ASSERT_EQ(gtk_widget_get_mapped(GTK_WIDGET(window)), TRUE, "%d");
    // Check we have 2 monitors (original + the one we created)
    ASSERT_EQ(gdk_display_get_n_monitors(gdk_display_get_default()), 2, "%d");
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
)
