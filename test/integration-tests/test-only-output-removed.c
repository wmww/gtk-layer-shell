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

#include "integration-test-common.h"

static GtkWindow* window;

static void callback_0()
{
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface nil);
    window = create_default_window();
    gtk_layer_init_for_window(window);
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) gtk_layer_set_anchor(window, i, TRUE);
    ASSERT_EQ(gdk_display_get_n_monitors(gdk_display_get_default()), 1, "%d");
    gtk_widget_show_all(GTK_WIDGET(window));
}

static void callback_1()
{
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .closed);
    UNEXPECT_MESSAGE(wl_surface .destroy);
    send_command("destroy_output 0", "output_destroyed");
}

static void callback_2()
{
    ASSERT(gtk_widget_get_mapped(GTK_WIDGET(window)));
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .configure 808 911);
    send_command("create_output 808 911", "output_created");
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
)
