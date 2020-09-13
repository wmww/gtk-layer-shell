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

static gboolean quit_timeout(gpointer _data)
{
    (void)_data;
    gtk_main_quit();
    return FALSE;
}

void add_quit_timeout()
{
    g_timeout_add(100, quit_timeout, NULL);
}

void setup_window(GtkWindow* window)
{
    GtkWidget *label = gtk_label_new("");
    gtk_label_set_markup(
        GTK_LABEL(label),
        "<span font_desc=\"20.0\">"
            "Layer shell test"
        "</span>");
    gtk_container_add(GTK_CONTAINER(window), label);
    gtk_container_set_border_width(GTK_CONTAINER(window), 12);
}
