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
    GtkLayerShellKeyboardMode value;
} const all_kb_settings[] = {
    {"None", GTK_LAYER_SHELL_KEYBOARD_MODE_NONE},
    {"Exclusive", GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE},
    {"On demand", GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND},
};

static void
on_kb_selected (GtkComboBox *widget, GtkWindow *layer_window)
{
    GtkComboBox *combo_box = widget;

    gchar *keyboard = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (combo_box));
    gboolean kb_was_set = FALSE;
    for (unsigned i = 0; i < sizeof(all_kb_settings) / sizeof(all_kb_settings[0]); i++) {
        if (g_strcmp0 (keyboard, all_kb_settings[i].name) == 0) {
            gtk_layer_set_keyboard_mode (layer_window, all_kb_settings[i].value);
            kb_was_set = TRUE;
            break;
        }
    }
    g_free (keyboard);
    g_return_if_fail (kb_was_set);
}

GtkWidget *
keyboard_selection_new (GtkWindow *layer_window, GtkLayerShellKeyboardMode default_kb)
{
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    {
        GtkWidget *label = gtk_label_new ("Keyboard:");
        GtkWidget *combo_box = gtk_combo_box_text_new ();
        gtk_widget_set_tooltip_text (combo_box, "Keyboard interactivity mode");
        for (unsigned i = 0; i < sizeof(all_kb_settings) / sizeof(all_kb_settings[0]); i++) {
            gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), all_kb_settings[i].name);
            if (all_kb_settings[i].value == default_kb)
                gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), i);
        }
        g_signal_connect (combo_box, "changed", G_CALLBACK (on_kb_selected), layer_window);
        gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (vbox), combo_box, FALSE, FALSE, 0);
    }

    return vbox;
}
