/* This entire file is licensed under GPLv3
 *
 * Copyright 2026 Kerem Söke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "integration-test-common.h"
#include <wayland-client.h>

static void callback_0()
{
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface);
    EXPECT_MESSAGE(Wayland connection lost while waiting for initial .configure);
    UNEXPECT_MESSAGE(Timed out waiting for initial .configure);

    GtkWindow* window = create_default_window();
    gtk_layer_init_for_window(window);

    struct wl_display* display = gdk_wayland_display_get_wl_display(gdk_display_get_default());
    wl_registry_bind(wl_display_get_registry(display), 0xfffffff0, &wl_seat_interface, 1);
    wl_display_flush(display);

    gtk_widget_show_all(GTK_WIDGET(window));

    ASSERT(wl_display_get_error(display) != 0);
    CHECK_EXPECTATIONS();
    exit(0);
}

TEST_CALLBACKS(
    callback_0,
)
