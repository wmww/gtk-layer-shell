#include "example.h"

static const gboolean default_left = FALSE;
static const gboolean default_right = FALSE;
static const gboolean default_top = FALSE;
static const gboolean default_bottom = FALSE;

static const GtkLayerShellLayer default_layer = GTK_LAYER_SHELL_LAYER_TOP;

static const gboolean default_auto_exclusive_zone = FALSE;
static const gboolean default_keyboard_interactivity = FALSE;

static void
on_orientation_changed (GtkWindow *window, WindowOrientation orientation, GtkWidget *box)
{
    switch (orientation) {
        case WINDOW_ORIENTATION_HORIZONTAL:
            gtk_orientable_set_orientation (GTK_ORIENTABLE (box), GTK_ORIENTATION_HORIZONTAL);
            break;
        case WINDOW_ORIENTATION_VERTICAL:
            gtk_orientable_set_orientation (GTK_ORIENTABLE (box), GTK_ORIENTATION_VERTICAL);
            break;
        case WINDOW_ORIENTATION_NONE:
            gtk_orientable_set_orientation (GTK_ORIENTABLE (box), GTK_ORIENTATION_VERTICAL);
            break;
    }
}

static void
activate (GtkApplication* app, void *_data)
{
    GtkWindow *gtk_window = GTK_WINDOW (gtk_application_window_new (app));

    gtk_layer_init_for_window (gtk_window);

    gtk_layer_set_anchor (gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, default_left);
    gtk_layer_set_anchor (gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, default_right);
    gtk_layer_set_anchor (gtk_window, GTK_LAYER_SHELL_EDGE_TOP, default_top);
    gtk_layer_set_anchor (gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, default_bottom);
    gtk_layer_set_layer (gtk_window, default_layer);
    gtk_layer_set_exclusive_zone (gtk_window, default_auto_exclusive_zone);
    gtk_layer_set_keyboard_interactivity (gtk_window, default_keyboard_interactivity);
    gtk_window_set_title (gtk_window, "gtk-layer-shell-example"); // Used for layer namespace

    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 16);
    gtk_container_add (GTK_CONTAINER (gtk_window), vbox);
    gtk_container_add (GTK_CONTAINER (vbox), menu_bar_new (gtk_window));
    gtk_container_add (GTK_CONTAINER (vbox),
                       mscl_toggles_new (gtk_window,
                                         default_auto_exclusive_zone,
                                         default_keyboard_interactivity));
    gtk_container_add (GTK_CONTAINER (vbox),
                       layer_selection_new (gtk_window, default_layer));
    gtk_container_add (GTK_CONTAINER (vbox),
                       anchor_control_new (gtk_window,
                                           default_left,
                                           default_right,
                                           default_top,
                                           default_bottom));
    g_signal_connect (gtk_window, "orientation-changed", G_CALLBACK (on_orientation_changed), vbox);
    gtk_widget_show_all (GTK_WIDGET (gtk_window));
}

int
main (int argc, char **argv)
{
    // The int arg is an enum of type WindowOrientation
    // Signal is emitted in anchor-control.c
    g_signal_new("orientation-changed",
                 GTK_TYPE_WINDOW,
                 G_SIGNAL_RUN_FIRST,
                 0, NULL, NULL,
                 g_cclosure_marshal_VOID__INT,
                 G_TYPE_NONE, 1, G_TYPE_INT);
    GtkApplication * app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return status;
}
