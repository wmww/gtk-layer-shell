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

#include "gtk-layer-demo.h"

GtkWidget *
version_info_new ()
{
    gchar *version_str = g_strdup_printf("GTK LS v%d.%d.%d\nGTK v%d.%d.%d",
                                         gtk_layer_get_major_version (),
                                         gtk_layer_get_minor_version (),
                                         gtk_layer_get_micro_version (),
                                         gtk_get_major_version (),
                                         gtk_get_minor_version (),
                                         gtk_get_micro_version ());
    GtkWidget *version_info = gtk_label_new(version_str);
    g_free(version_str);

    gtk_label_set_justify(GTK_LABEL(version_info), GTK_JUSTIFY_CENTER);
    gtk_label_set_xalign(GTK_LABEL(version_info), 0.5);
    gtk_label_set_yalign(GTK_LABEL(version_info), 0.25);
    return version_info;
}
