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
static GtkWidget* child;

static void callback_0()
{
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .set_size 600 500);

    window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    child = gtk_label_new("foo");
    gtk_container_add(GTK_CONTAINER(window), child);

    gtk_layer_init_for_window(window);
    gtk_widget_set_size_request(GTK_WIDGET(window), 600, 500);
    gtk_widget_show_all(GTK_WIDGET(window));
}

static void callback_1()
{
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .set_size 500 400);

    gtk_widget_set_size_request(GTK_WIDGET(window), 500, 400);
    gtk_window_resize(window, 1, 1);
}

static void callback_2()
{
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .set_size 400 300);

    gtk_widget_set_size_request(GTK_WIDGET(window), -1, -1);
    gtk_widget_set_size_request(child, 400, 300);
    gtk_window_resize(window, 1, 1);
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
)
