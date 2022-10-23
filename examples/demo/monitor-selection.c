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

const char *current_monitor_key = "current_layer_monitor";

static void
on_monitor_selected (GtkComboBox *combo_box, GtkWindow *layer_window)
{
    int monitor_index = gtk_combo_box_get_active (combo_box) - 1; // 1st element is default monitor
    GdkMonitor *monitor = NULL;
    if (monitor_index >= 0) {
        GdkDisplay *display = gdk_display_get_default ();
        g_return_if_fail (monitor_index < gdk_display_get_n_monitors (display));
        monitor = gdk_display_get_monitor (display, monitor_index);
    }
    g_object_set_data (G_OBJECT (combo_box), current_monitor_key, monitor);
    gtk_layer_set_monitor (layer_window, monitor);
}

void
on_monitors_changed (GdkDisplay *display, GdkMonitor *_monitor, GtkComboBox *combo_box)
{
    (void)_monitor;

    gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (combo_box));
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), "Default");
    GdkMonitor *current_monitor = g_object_get_data (G_OBJECT (combo_box), current_monitor_key);
    if (current_monitor == NULL) {
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), 0);
    }
    for (int i = 0; i < gdk_display_get_n_monitors (display); i++) {
        GdkMonitor *monitor = gdk_display_get_monitor (display, i);
        GString *text = g_string_new ("");
        g_string_printf (text, "%d. %s", i + 1, gdk_monitor_get_model (monitor));
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), text->str);
        g_string_free (text, TRUE);
        if (monitor == current_monitor) {
            gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), i + 1);
        }
    }
}

GtkWidget *
monitor_selection_new (GtkWindow *layer_window)
{
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    {
        GtkWidget *combo_box = gtk_combo_box_text_new ();
        gtk_widget_set_tooltip_text (combo_box, "Monitor");
        gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (combo_box), NULL, "Default");
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), 0);
        GdkDisplay *display = gdk_display_get_default ();
        g_signal_connect (display, "monitor-added", G_CALLBACK (on_monitors_changed), combo_box);
        g_signal_connect (display, "monitor-removed", G_CALLBACK (on_monitors_changed), combo_box);
        on_monitors_changed (display, NULL, GTK_COMBO_BOX (combo_box));
        g_signal_connect (combo_box, "changed", G_CALLBACK (on_monitor_selected), layer_window);
        gtk_box_pack_start (GTK_BOX (vbox), combo_box, FALSE, FALSE, 0);
    }

    return vbox;
}
