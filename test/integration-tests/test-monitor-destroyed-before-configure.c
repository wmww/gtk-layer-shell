/* This entire file is licensed under MIT
 *
 * Copyright 2026 Sophie Winter
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
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface);
    EXPECT_MESSAGE(WARNING Compositor closed layer surface before sending initial .configure);
    EXPECT_MESSAGE(wl_surface .destroy);
    UNEXPECT_MESSAGE(zwlr_layer_surface_v1 .configure);

    send_command("enable_configure_delay", "configure_delay_enabled");
    send_command("destroy_outputs_on_layer_surface_create", "destroy_outputs_on_layer_surface_create_enabled");

    window = create_default_window();
    gtk_layer_init_for_window(window);
    gtk_widget_show_all(GTK_WIDGET(window));
    ASSERT(!gtk_widget_get_mapped(GTK_WIDGET(window)));
}

static void callback_1()
{
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface);
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .configure);
    ASSERT(!gtk_widget_get_mapped(GTK_WIDGET(window)));
    send_command("create_output 1000 1000", "output_created");
}

static void callback_2()
{
    EXPECT_MESSAGE(wl_surface .destroy);
    ASSERT(gtk_widget_get_mapped(GTK_WIDGET(window)));
    gtk_widget_unmap(GTK_WIDGET(window));
    ASSERT(!gtk_widget_get_mapped(GTK_WIDGET(window)));
}

static void callback_3()
{
    UNEXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface);
    send_command("destroy_output 1", "output_destroyed");
    send_command("create_output 1000 1000", "output_created");
    ASSERT(!gtk_widget_get_mapped(GTK_WIDGET(window)));
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
    callback_3,
)
