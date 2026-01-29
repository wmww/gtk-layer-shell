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
// cascading remap() calls. Without the fix, each remap creates a new layer surface,
// which triggers another monitor event, which triggers another remap, etc.
//
// Strategy: Configure the mock server to create a new output each time it sees a
// get_layer_surface request. Without the fix, showing one window cascades into many
// layer surface creations and many new outputs. With the fix, only one layer surface
// is created because remap is skipped during configure wait.

#include "integration-test-common.h"

static GtkWindow* window;

static void callback_0()
{
    // Enable configure delay so the window's roundtrip has time to process other events
    send_command("enable_configure_delay", "configure_delay_enabled");
}

static void callback_1()
{
    // Enable the cascade trigger: create a new output each time get_layer_surface is called
    // We allow up to 5 creations, but with the fix only 1 should happen
    send_command("enable_output_on_layer_surface 5", "output_on_layer_surface_enabled");
}

static void callback_2()
{
    // Create the window
    window = create_default_window();
    gtk_layer_init_for_window(window);

    // Show the window - this is the critical moment
    // Without fix: get_layer_surface triggers output creation, which triggers monitor_changed,
    //              which calls remap(), which creates another get_layer_surface, etc.
    // With fix: remap() is skipped during configure wait, so only one get_layer_surface
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface nil);
    gtk_widget_show_all(GTK_WIDGET(window));
}

static void callback_3()
{
    // If we get here without timing out, verify the window is mapped
    ASSERT_EQ(gtk_widget_get_mapped(GTK_WIDGET(window)), TRUE, "%d");

    // Check how many monitors we have
    // - Started with 1 (default output)
    // - With fix: 1 more created = 2 total
    // - Without fix: up to 6 created (1 + 5 from cascade) = 6 total
    int n_monitors = gdk_display_get_n_monitors(gdk_display_get_default());
    fprintf(stderr, "Number of monitors: %d\n", n_monitors);

    // With the fix, we should have at most 2 monitors (original + 1 from initial show)
    // Without the fix, we'd have 6 (original + 5 from cascade)
    ASSERT(n_monitors <= 2);
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
    callback_3,
)
