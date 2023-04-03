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

gboolean
on_exclusive_zone_state_set (GtkToggleButton *_toggle_button, gboolean state, GtkWindow *layer_window)
{
    (void)_toggle_button;

    if (state) {
        gtk_layer_auto_exclusive_zone_enable (layer_window);
    } else {
        gtk_layer_set_exclusive_zone (layer_window, 0);
    }
    return FALSE;
}

gboolean
on_fixed_size_set (GtkToggleButton *_toggle_button, gboolean state, GtkWindow *layer_window)
{
    (void)_toggle_button;

    if (state) {
        gtk_widget_set_size_request (GTK_WIDGET (layer_window), fixed_size_width, fixed_size_height);
    } else {
        gtk_widget_set_size_request (GTK_WIDGET (layer_window), -1, -1);
    }
    gtk_window_resize (layer_window, 1, 1);
    return FALSE;
}

struct {
    const char *name;
    const char *tooltip;
    gboolean (*callback) (GtkToggleButton *toggle_button, gboolean state, GtkWindow *layer_window);
} const mscl_toggles[] = {
    {"Exclusive", "Create an exclusive zone when anchored", on_exclusive_zone_state_set},
    {"Fixed size", "Set a fixed window size (ignored depending on anchors)", on_fixed_size_set},
};

GtkWidget *
mscl_toggles_new (GtkWindow *layer_window,
                  gboolean default_auto_exclusive_zone,
                  gboolean default_fixed_size)
{
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    for (unsigned i = 0; i < sizeof (mscl_toggles) / sizeof (mscl_toggles[0]); i++) {
        GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
        gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
        {
            GtkWidget *label = gtk_label_new (mscl_toggles[i].name);
            gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
        }{
            GtkWidget *toggle = gtk_switch_new ();
            gtk_widget_set_tooltip_text (toggle, mscl_toggles[i].tooltip);
            gboolean default_value;
            if (mscl_toggles[i].callback == on_exclusive_zone_state_set)
                default_value = default_auto_exclusive_zone;
            else if (mscl_toggles[i].callback == on_fixed_size_set)
                default_value = default_fixed_size;
            else
                g_assert_not_reached ();
            gtk_switch_set_active (GTK_SWITCH (toggle), default_value);
            g_signal_connect (toggle, "state-set", G_CALLBACK (mscl_toggles[i].callback), layer_window);
            gtk_box_pack_end (GTK_BOX (hbox), toggle, FALSE, FALSE, 0);
        }
    }
    return vbox;
}
