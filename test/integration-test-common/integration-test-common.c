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

#include "integration-test-common.h"

// How long each callback has to run
#define STEP_TIME 300

static int return_code = 0;
static int callback_index = 0;

static gboolean next_step(gpointer _data)
{
    (void)_data;
    CHECK_EXPECTATIONS();
    if (test_callbacks[callback_index])
    {
        test_callbacks[callback_index]();
        callback_index++;
        return TRUE;
    }
    else
    {
        gtk_main_quit();
        return FALSE;
    }
}

GtkWindow* create_default_window()
{
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    GtkWidget *label = gtk_label_new("");
    gtk_label_set_markup(
        GTK_LABEL(label),
        "<span font_desc=\"20.0\">"
            "Layer shell test"
        "</span>");
    gtk_container_add(GTK_CONTAINER(window), label);
    gtk_container_set_border_width(GTK_CONTAINER(window), 12);
    return window;
}

void wayland_roundtrip()
{
    GdkDisplay* gdk_display = gdk_display_get_default();
    struct wl_display* wl_display = gdk_wayland_display_get_wl_display(GDK_WAYLAND_DISPLAY(gdk_display));
    wl_display_roundtrip(wl_display);
}

int main()
{
    gtk_init(0, NULL);
    next_step(NULL);
    g_timeout_add(STEP_TIME, next_step, NULL);
    gtk_main();
    return return_code;
}
