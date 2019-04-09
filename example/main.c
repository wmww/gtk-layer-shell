#include "example.h"

static const gboolean default_left = FALSE;
static const gboolean default_right = TRUE;
static const gboolean default_top = TRUE;
static const gboolean default_bottom = FALSE;

static const GtkLayerShellLayer default_layer = GTK_LAYER_TOP;

gboolean
on_button_press (GtkWidget *parent, GdkEventButton *event, void *_data)
{
    GtkWidget *menu = gtk_menu_new ();
    for (int i = 0; i < 3; i++)
    {
        GString *label = g_string_new ("");
        g_string_printf (label, "Menu item %d", i);
        GtkWidget *menu_item = gtk_menu_item_new_with_label (label->str);
        g_string_free (label, TRUE);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
    }
    gtk_window_set_attached_to (GTK_WINDOW (gtk_widget_get_toplevel (menu)), parent);
    gtk_widget_show_all (menu);
    gtk_menu_popup_at_widget (GTK_MENU (menu), parent, GDK_GRAVITY_NORTH_EAST, GDK_GRAVITY_SOUTH_WEST, (GdkEvent *)event);
    return TRUE;
}

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
    GtkWidget *window = gtk_application_window_new (app);

    gtk_layer_init_for_window (GTK_WINDOW (window));
    gtk_layer_set_exclusive_zone (GTK_WINDOW (window), 20);

    gtk_layer_set_anchor_left (GTK_WINDOW (window), default_left);
    gtk_layer_set_anchor_right (GTK_WINDOW (window), default_right);
    gtk_layer_set_anchor_top (GTK_WINDOW (window), default_top);
    gtk_layer_set_anchor_bottom (GTK_WINDOW (window), default_bottom);

    gtk_layer_set_layer (GTK_WINDOW (window), default_layer);

    gtk_window_set_default_size (GTK_WINDOW (window), -1, -1);
    gtk_window_set_title (GTK_WINDOW (window), "Window");

    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    g_signal_connect (window, "orientation-changed", G_CALLBACK (on_orientation_changed), vbox);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    GtkWidget *spacer_button = gtk_button_new_with_label ("Useless");
    gtk_widget_set_tooltip_text (spacer_button, "This is a tooltip");
    gtk_container_add (GTK_CONTAINER (vbox), spacer_button);
    GtkWidget *button = gtk_button_new_with_label ("Menu");
    g_signal_connect (button, "button_press_event",  G_CALLBACK (on_button_press), NULL);
    gtk_container_add (GTK_CONTAINER (vbox), button);
    gtk_container_add (GTK_CONTAINER (vbox), layer_selection_new (GTK_WINDOW (window),
                                                                  default_layer));
    gtk_container_add (GTK_CONTAINER (vbox), anchor_control_new (GTK_WINDOW (window),
                                                                 default_left,
                                                                 default_right,
                                                                 default_top,
                                                                 default_bottom));
    gtk_widget_show_all (window);
}

int
main (int argc, char **argv)
{
    g_signal_new("orientation-changed",
                 GTK_TYPE_WINDOW,
                 G_SIGNAL_RUN_FIRST,
                 0, NULL, NULL,
                 g_cclosure_marshal_VOID__INT,
                 G_TYPE_NONE, 1, G_TYPE_INT); // WindowOrientation
    GtkApplication * app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return status;
}
