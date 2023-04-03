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

typedef struct {
    GtkLayerShellEdge edge;
    GtkWindow *layer_window;
} AnchorButtonData;

static void
on_anchor_toggled (GtkToggleButton *button, AnchorButtonData *data)
{
    gboolean is_anchored = gtk_toggle_button_get_active (button);
    ToplevelData *toplevel_data = g_object_get_data (G_OBJECT (data->layer_window), anchor_edges_key);
    g_return_if_fail (toplevel_data);
    toplevel_data->edges[data->edge] = is_anchored;
    layer_window_update_orientation (data->layer_window);
    gtk_layer_set_anchor (data->layer_window, data->edge, is_anchored);
}

static GtkWidget *
anchor_edge_button_new (GtkWindow *layer_window,
                        GtkLayerShellEdge edge,
                        const gboolean defaults[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER],
                        const char *icon_name,
                        const char *tooltip)
{
    GtkWidget *button = gtk_toggle_button_new ();
    gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_icon_name (icon_name, GTK_ICON_SIZE_BUTTON));
    gtk_widget_set_tooltip_text (button, tooltip);
    AnchorButtonData *data = g_new0 (AnchorButtonData, 1);
    *data = (AnchorButtonData) {
        .edge = edge,
        .layer_window = layer_window,
    };
    g_object_set_data_full(G_OBJECT (button), "clicked_signal_data", data, (GDestroyNotify)g_free);
    g_signal_connect (button, "clicked", G_CALLBACK (on_anchor_toggled), data);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), defaults[edge]);
    return button;
}

GtkWidget *
anchor_control_new (GtkWindow *layer_window, const gboolean default_anchors[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER])
{
    GtkWidget *outside_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *outside_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (outside_vbox), outside_hbox, TRUE, FALSE, 0);
    {
        GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
        gtk_box_pack_start (GTK_BOX (outside_hbox), hbox, TRUE, FALSE, 0);
        {
            GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_add (GTK_CONTAINER (hbox), vbox);
            {
                GtkWidget *button = anchor_edge_button_new (layer_window, GTK_LAYER_SHELL_EDGE_LEFT, default_anchors, "go-first", "Anchor left");
                gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, FALSE, 0);
            }
        }{
            GtkWidget *center_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 40);
            gtk_container_add (GTK_CONTAINER (hbox), center_vbox);
            {
                GtkWidget *button = anchor_edge_button_new (layer_window, GTK_LAYER_SHELL_EDGE_TOP, default_anchors, "go-top", "Anchor top");
                gtk_box_pack_start (GTK_BOX (center_vbox), button, FALSE, FALSE, 0);
            }{
                GtkWidget *button = anchor_edge_button_new (layer_window, GTK_LAYER_SHELL_EDGE_BOTTOM, default_anchors, "go-bottom", "Anchor bottom");
                gtk_box_pack_end (GTK_BOX (center_vbox), button, FALSE, FALSE, 0);
            }
        }{
            GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_add (GTK_CONTAINER (hbox), vbox);
            {
                GtkWidget *button = anchor_edge_button_new (layer_window, GTK_LAYER_SHELL_EDGE_RIGHT, default_anchors, "go-last", "Anchor right");
                gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, FALSE, 0);
            }
        }
    }

    return outside_vbox;
}
