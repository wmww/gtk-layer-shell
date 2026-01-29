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
// This tests rapid monitor changes while compositor keeps closing the layer surface.
// This simulates the TTY switching scenario where multiple output events happen quickly.

#include "integration-test-common.h"

static GtkWindow* window;
static int remap_count = 0;

static void callback_0()
{
    // First create multiple outputs so we can destroy them to trigger multiple monitor changes
    send_command("create_output 800 600", "output_created");
}

static void callback_1()
{
    send_command("create_output 1024 768", "output_created");
}

static void callback_2()
{
    // Create a window - it will use the default output (slot 0)
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface nil);
    window = create_default_window();
    gtk_layer_init_for_window(window);
    gtk_widget_show_all(GTK_WIDGET(window));
}

static void callback_3()
{
    // Now set up the compositor to close any new layer surfaces immediately
    send_command("send_closed_instead_of_configure", "will_send_closed");
}

static void callback_4()
{
    // Destroy outputs 1 and 2 in quick succession
    // Each destruction triggers a monitor-removed signal
    // If the window tries to remap and gets closed, and then another monitor event triggers
    // another remap, we could get an infinite loop

    // We expect:
    // - First destroy triggers monitor_changed, which calls remap
    // - Remap shows window, compositor sends .closed
    // - Window is unmapped
    // - Second destroy triggers monitor_changed
    // - If layer_surface is already NULL, no remap happens (correct behavior)
    // - If there's a bug, we'd get infinite remapping

    EXPECT_MESSAGE(zwlr_layer_surface_v1 .closed);
    send_command("destroy_output 1", "output_destroyed");
}

static void callback_5()
{
    // Second output destruction - this should NOT cause a remap since the window was already closed
    send_command("destroy_output 2", "output_destroyed");
}

static void callback_6()
{
    // If we get here, we didn't hang. The window should be unmapped.
    ASSERT_EQ(gtk_widget_get_mapped(GTK_WIDGET(window)), FALSE, "%d");
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
    callback_3,
    callback_4,
    callback_5,
    callback_6,
)
