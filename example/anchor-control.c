#include "example.h"

typedef struct {
    gboolean edges[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER];
    WindowOrientation orientation;
    GtkWindow *layer_window;
} AnchorEdges;

typedef struct {
    GtkLayerShellEdge edge;
    AnchorEdges *window_edges;
} AnchorButtonData;

static void
anchor_edges_update_orientation (AnchorEdges *edges)
{
    gboolean horiz = edges->edges[GTK_LAYER_SHELL_EDGE_LEFT] == edges->edges[GTK_LAYER_SHELL_EDGE_RIGHT];
    gboolean vert = edges->edges[GTK_LAYER_SHELL_EDGE_TOP] == edges->edges[GTK_LAYER_SHELL_EDGE_BOTTOM];
    WindowOrientation orientation = WINDOW_ORIENTATION_NONE;
    if (horiz && (!vert || (edges->edges[GTK_LAYER_SHELL_EDGE_LEFT] && !edges->edges[GTK_LAYER_SHELL_EDGE_TOP]))) {
        orientation = WINDOW_ORIENTATION_HORIZONTAL;
    } else if (vert && (!horiz || (edges->edges[GTK_LAYER_SHELL_EDGE_TOP] && !edges->edges[GTK_LAYER_SHELL_EDGE_LEFT]))) {
        orientation = WINDOW_ORIENTATION_VERTICAL;
    }
    if (orientation != edges->orientation) {
        edges->orientation = orientation;
        g_signal_emit_by_name(edges->layer_window, "orientation-changed", orientation);
    }
}

static void
on_anchor_toggled (GtkToggleButton *button, AnchorButtonData *data)
{
    gboolean is_anchored = gtk_toggle_button_get_active (button);
    data->window_edges->edges[data->edge] = is_anchored;
    anchor_edges_update_orientation (data->window_edges);
    gtk_layer_set_anchor (data->window_edges->layer_window, data->edge, is_anchored);
}

static GtkWidget *
anchor_edge_button_new (AnchorEdges *window_edges, GtkLayerShellEdge edge, const char *icon_name, const char *tooltip)
{
    GtkWidget *button = gtk_toggle_button_new ();
    gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_icon_name (icon_name, GTK_ICON_SIZE_BUTTON));
    gtk_widget_set_tooltip_text (button, tooltip);
    AnchorButtonData *data = g_new0 (AnchorButtonData, 1);
    *data = (AnchorButtonData) {
        .edge = edge,
        .window_edges = window_edges,
    };
    g_signal_connect_data (button,
                           "clicked",
                           G_CALLBACK (on_anchor_toggled),
                           data,
                           (GClosureNotify)g_free,
                           (GConnectFlags)0);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), data->window_edges->edges[data->edge]);
    return button;
}

GtkWidget *
anchor_control_new (GtkWindow *layer_window, const gboolean default_anchors[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER])
{
    AnchorEdges *anchor_edges = g_new0 (AnchorEdges, 1);
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++)
        anchor_edges->edges[i] = default_anchors[i];
    anchor_edges->layer_window = layer_window;
    anchor_edges->orientation = -1; // invalid value will force anchor_edges_update_orientation to update
    anchor_edges_update_orientation (anchor_edges);
    // This is never accessed, but is set for memeory management
    g_object_set_data_full (G_OBJECT (layer_window), "anchor_edges", anchor_edges, g_free);

    GtkWidget *outside_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    {
        GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
        gtk_box_pack_start (GTK_BOX (outside_hbox), hbox, TRUE, FALSE, 0);
        {
            GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_add (GTK_CONTAINER (hbox), vbox);
            {
                GtkWidget *button = anchor_edge_button_new (anchor_edges, GTK_LAYER_SHELL_EDGE_LEFT, "go-first", "Anchor left");
                gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, FALSE, 0);
            }
        }{
            GtkWidget *center_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
            gtk_container_add (GTK_CONTAINER (hbox), center_vbox);
            {
                GtkWidget *button = anchor_edge_button_new (anchor_edges, GTK_LAYER_SHELL_EDGE_TOP, "go-top", "Anchor top");
                gtk_box_pack_start (GTK_BOX (center_vbox), button, FALSE, FALSE, 0);
            }{
                GtkWidget *button = anchor_edge_button_new (anchor_edges, GTK_LAYER_SHELL_EDGE_BOTTOM, "go-bottom", "Anchor bottom");
                gtk_box_pack_end (GTK_BOX (center_vbox), button, FALSE, FALSE, 0);
            }
        }{
            GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_add (GTK_CONTAINER (hbox), vbox);
            {
                GtkWidget *button = anchor_edge_button_new (anchor_edges, GTK_LAYER_SHELL_EDGE_RIGHT, "go-last", "Anchor right");
                gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, FALSE, 0);
            }
        }
    }

    return outside_hbox;
}
