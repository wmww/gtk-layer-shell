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

struct {
    const char *name;
    GtkLayerShellLayer value;
} const all_layers[] = {
    {"Overlay", GTK_LAYER_SHELL_LAYER_OVERLAY},
    {"Top", GTK_LAYER_SHELL_LAYER_TOP},
    {"Bottom", GTK_LAYER_SHELL_LAYER_BOTTOM},
    {"Background", GTK_LAYER_SHELL_LAYER_BACKGROUND},
};

static void
on_layer_selected (GtkComboBox *widget, GtkWindow *layer_window)
{
    GtkComboBox *combo_box = widget;

    gchar *layer = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (combo_box));
    gboolean layer_was_set = FALSE;
    for (unsigned i = 0; i < sizeof(all_layers) / sizeof(all_layers[0]); i++) {
        if (g_strcmp0 (layer, all_layers[i].name) == 0) {
            gtk_layer_set_layer (layer_window, all_layers[i].value);
            layer_was_set = TRUE;
            break;
        }
    }
    g_free (layer);
    g_return_if_fail (layer_was_set);
}

GtkWidget *
layer_selection_new (GtkWindow *layer_window, GtkLayerShellLayer default_layer)
{
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    {
        GtkWidget *combo_box = gtk_combo_box_text_new ();
        gtk_widget_set_tooltip_text (combo_box, "Layer");
        for (unsigned i = 0; i < sizeof(all_layers) / sizeof(all_layers[0]); i++) {
            gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), all_layers[i].name);
            if (all_layers[i].value == default_layer)
                gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), i);
        }
        g_signal_connect (combo_box, "changed", G_CALLBACK (on_layer_selected), layer_window);
        gtk_box_pack_start (GTK_BOX (vbox), combo_box, FALSE, FALSE, 0);
    }

    return vbox;
}
