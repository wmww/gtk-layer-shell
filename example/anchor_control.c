#include "example.h"

typedef void (*AnchorEdgeSetter) (GtkWindow *window, gboolean anchor);

struct AnchorEdge {
    AnchorEdgeSetter setter;
    gboolean is_anchored;
    GtkWindow *layer_window;
};

static void
on_anchor_toggled (GtkButton *button, struct AnchorEdge *edge)
{
    edge->is_anchored = !edge->is_anchored;
    gtk_button_set_relief (button, edge->is_anchored ? GTK_RELIEF_NORMAL : GTK_RELIEF_NONE);
    edge->setter (edge->layer_window, edge->is_anchored);
}

static void
anchor_edge_setup_button (GtkButton *button, GtkWindow *layer_window, AnchorEdgeSetter setter, gboolean anchor)
{
    struct AnchorEdge *data = g_new0 (struct AnchorEdge, 1);
    *data = (struct AnchorEdge) {
        .setter = setter,
        .is_anchored = !anchor, // when we call on_anchor_toggled this will get flipped
        .layer_window = layer_window,
    };
    g_signal_connect_data (button,
                           "clicked",
                           G_CALLBACK (on_anchor_toggled),
                           data,
                           (GClosureNotify)g_free,
                           (GConnectFlags)0);
    on_anchor_toggled (button, data);
}

GtkWidget *
anchor_control_new (GtkWindow *layer_window, gboolean left, gboolean right, gboolean top, gboolean bottom)
{
    GtkWidget *outside_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    {
        GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_box_pack_start (GTK_BOX (outside_hbox), hbox, TRUE, FALSE, 0);
        {
            GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_add (GTK_CONTAINER (hbox), vbox);
            {
                GtkWidget *button = gtk_button_new_from_icon_name ("go-first", GTK_ICON_SIZE_BUTTON);
                gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, FALSE, 0);
                anchor_edge_setup_button (GTK_BUTTON (button), layer_window, gtk_layer_set_anchor_left, left);
            }
        }{
            GtkWidget *center_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 16);
            gtk_container_add (GTK_CONTAINER (hbox), center_vbox);
            {
                GtkWidget *button = gtk_button_new_from_icon_name ("go-top", GTK_ICON_SIZE_BUTTON);
                gtk_box_pack_start (GTK_BOX (center_vbox), button, FALSE, FALSE, 0);
                anchor_edge_setup_button (GTK_BUTTON (button), layer_window, gtk_layer_set_anchor_top, top);
            }{
                GtkWidget *button = gtk_button_new_from_icon_name ("go-bottom", GTK_ICON_SIZE_BUTTON);
                gtk_box_pack_end (GTK_BOX (center_vbox), button, FALSE, FALSE, 0);
                anchor_edge_setup_button (GTK_BUTTON (button), layer_window, gtk_layer_set_anchor_bottom, bottom);
            }
        }{
            GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_add (GTK_CONTAINER (hbox), vbox);
            {
                GtkWidget *button = gtk_button_new_from_icon_name ("go-last", GTK_ICON_SIZE_BUTTON);
                gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, FALSE, 0);
                anchor_edge_setup_button (GTK_BUTTON (button), layer_window, gtk_layer_set_anchor_right, right);
            }
        }
    }

    return outside_hbox;
}
