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

#include "test-client-common.h"

static GtkWindow* window;

static void callback_0()
{
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .set_exclusive_zone 240);

    window = create_default_window();

    gtk_layer_init_for_window(window);
    gtk_layer_auto_exclusive_zone_enable(window);
    gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(window), 320, 240);
    gtk_widget_show_all(GTK_WIDGET(window));
}

static void callback_1()
{
    // Bottom margin should have no effect on exclusive zone but top margin should
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .set_exclusive_zone 250);

    gtk_layer_set_margin(window, GTK_LAYER_SHELL_EDGE_TOP, 10);
    gtk_layer_set_margin(window, GTK_LAYER_SHELL_EDGE_BOTTOM, 5);
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
)
