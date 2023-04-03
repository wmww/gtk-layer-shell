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
    window = create_default_window();
    gtk_layer_init_for_window(window);
    ASSERT_EQ(gdk_display_get_n_monitors(gdk_display_get_default()), 1, "%d");
    GdkMonitor *monitor = gdk_display_get_monitor(gdk_display_get_default(), 0);
    ASSERT(GDK_IS_MONITOR(monitor));
    ASSERT_EQ(gtk_layer_get_monitor(window), NULL, "%p");
    gtk_layer_set_monitor(window, monitor);
    ASSERT_EQ(gtk_layer_get_monitor(window), monitor, "%p");
    gtk_widget_show_all(GTK_WIDGET(window));
    ASSERT_EQ(gtk_layer_get_monitor(window), monitor, "%p");
    gtk_layer_set_monitor(window, NULL);
    ASSERT_EQ(gtk_layer_get_monitor(window), NULL, "%p");
}

TEST_CALLBACKS(
    callback_0,
)
