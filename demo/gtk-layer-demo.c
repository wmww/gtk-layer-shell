#include "gtk-layer-demo.h"

static const gboolean default_anchors[] = {FALSE, FALSE, FALSE, FALSE};
static const int default_margins[] = {0, 0, 0, 0};

static const GtkLayerShellLayer default_layer = GTK_LAYER_SHELL_LAYER_TOP;

static const gboolean default_auto_exclusive_zone = FALSE;
static const gboolean default_keyboard_interactivity = FALSE;

const char *anchor_edges_key = "anchor_edges";

void
layer_window_update_orientation (GtkWindow *layer_window)
{
    ToplevelData *data = g_object_get_data (G_OBJECT (layer_window), anchor_edges_key);
    gboolean horiz = data->edges[GTK_LAYER_SHELL_EDGE_LEFT] == data->edges[GTK_LAYER_SHELL_EDGE_RIGHT];
    gboolean vert = data->edges[GTK_LAYER_SHELL_EDGE_TOP] == data->edges[GTK_LAYER_SHELL_EDGE_BOTTOM];
    WindowOrientation orientation = WINDOW_ORIENTATION_NONE;
    if (horiz && (!vert || (data->edges[GTK_LAYER_SHELL_EDGE_LEFT] && !data->edges[GTK_LAYER_SHELL_EDGE_TOP]))) {
        orientation = WINDOW_ORIENTATION_HORIZONTAL;
    } else if (vert && (!horiz || (data->edges[GTK_LAYER_SHELL_EDGE_TOP] && !data->edges[GTK_LAYER_SHELL_EDGE_LEFT]))) {
        orientation = WINDOW_ORIENTATION_VERTICAL;
    }
    if (orientation != data->orientation) {
        data->orientation = orientation;
        g_signal_emit_by_name(layer_window, "orientation-changed", orientation);
    }
}

static void
on_orientation_changed (GtkWindow *window, WindowOrientation orientation, ToplevelData *data)
{
    GtkOrientation orient_toplevel, orient_sub;
    switch (orientation) {
        case WINDOW_ORIENTATION_HORIZONTAL:
            orient_toplevel = GTK_ORIENTATION_HORIZONTAL;
            orient_sub = GTK_ORIENTATION_HORIZONTAL;
            break;
        case WINDOW_ORIENTATION_VERTICAL:
            orient_toplevel = GTK_ORIENTATION_VERTICAL;
            orient_sub = GTK_ORIENTATION_VERTICAL;
            break;
        case WINDOW_ORIENTATION_NONE:
            orient_toplevel = GTK_ORIENTATION_HORIZONTAL;
            orient_sub = GTK_ORIENTATION_VERTICAL;
            break;
    }
    gtk_orientable_set_orientation (GTK_ORIENTABLE (data->toplevel_box), orient_toplevel);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (data->first_box), orient_sub);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (data->second_box), orient_sub);
}

static GtkWidget *
layer_window_new ()
{
    GtkWindow *gtk_window = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_layer_init_for_window (gtk_window);

    ToplevelData *data = g_new0 (ToplevelData, 1);
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        data->edges[i] = default_anchors[i];
        gtk_layer_set_anchor (gtk_window, i, default_anchors[i]);
    }
    g_object_set_data_full (G_OBJECT (gtk_window), anchor_edges_key, data, g_free);
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++)
        gtk_layer_set_margin (gtk_window, i, default_margins[i]);
    gtk_layer_set_layer (gtk_window, default_layer);
    gtk_layer_set_exclusive_zone (gtk_window, default_auto_exclusive_zone);
    gtk_layer_set_keyboard_interactivity (gtk_window, default_keyboard_interactivity);
    gtk_window_set_title (gtk_window, "gtk-layer-shell-example"); // Used for layer namespace

    GtkWidget *centered_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (gtk_window), centered_vbox);
    GtkWidget *centered_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (centered_vbox), centered_hbox, TRUE, FALSE, 0);
    data->toplevel_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width (GTK_CONTAINER (data->toplevel_box), 16);
    gtk_box_pack_start (GTK_BOX (centered_hbox), data->toplevel_box, TRUE, FALSE, 0);
    {
        data->first_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
        gtk_box_pack_start (GTK_BOX (data->toplevel_box), data->first_box, FALSE, FALSE, 0);
        {
            GtkWidget *selections_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
            gtk_box_pack_start (GTK_BOX (selections_box), menu_bar_new (gtk_window), FALSE, FALSE, 0);
            gtk_box_pack_start (GTK_BOX (data->first_box), selections_box, FALSE, FALSE, 0);
            gtk_box_pack_start (GTK_BOX (selections_box),
                                monitor_selection_new (gtk_window),
                                FALSE, FALSE, 0);
            gtk_box_pack_start (GTK_BOX (selections_box),
                                layer_selection_new (gtk_window, default_layer),
                                FALSE, FALSE, 0);
        }{
            gtk_box_pack_start (GTK_BOX (data->first_box),
                                anchor_control_new (gtk_window, default_anchors),
                                FALSE, FALSE, 0);
        }
    }{
        GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
        gtk_box_pack_start (GTK_BOX (data->toplevel_box), vbox, FALSE, FALSE, 0);
        {
            data->second_box = mscl_toggles_new (gtk_window,
                                                 default_auto_exclusive_zone,
                                                 default_keyboard_interactivity);
            gtk_box_pack_start (GTK_BOX (vbox),
                                data->second_box,
                                FALSE, FALSE, 0);
            gtk_box_pack_start (GTK_BOX (vbox),
                                margin_control_new (gtk_window, default_margins),
                                FALSE, FALSE, 0);
        }
    }

    g_signal_connect (gtk_window, "orientation-changed", G_CALLBACK (on_orientation_changed), data);
    data->orientation = -1; // invalid value will force anchor_edges_update_orientation to update
    layer_window_update_orientation (gtk_window);

    return GTK_WIDGET (gtk_window);
}

int
main (int argc, char **argv)
{
    gtk_init(&argc, &argv);
    // The int arg is an enum of type WindowOrientation
    // Signal is emitted in anchor-control.c
    g_signal_new("orientation-changed",
                 GTK_TYPE_WINDOW,
                 G_SIGNAL_RUN_FIRST,
                 0, NULL, NULL,
                 g_cclosure_marshal_VOID__INT,
                 G_TYPE_NONE, 1, G_TYPE_INT);

    GtkWidget *initial_window = layer_window_new ();
    gtk_widget_show_all (GTK_WIDGET (initial_window));

    gtk_main ();
}
