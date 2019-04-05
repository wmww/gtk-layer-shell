#include "gtk-layer-shell.h"

#include <gtk/gtk.h>

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
on_layer_selected (GtkComboBox *widget,
                   GtkWindow *layer_window)
{
    g_message ("on_layer_selected ()");

    GtkComboBox *combo_box = widget;

    gchar *layer = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (combo_box));
    if (g_strcmp0 (layer, "Background") == 0) {
        gtk_window_set_layer_layer (layer_window, GTK_LAYER_SHELL_LAYER_BACKGROUND);
    } else if (g_strcmp0 (layer, "Bottom") == 0) {
        gtk_window_set_layer_layer (layer_window, GTK_LAYER_SHELL_LAYER_BOTTOM);
    } else if (g_strcmp0 (layer, "Top") == 0) {
        gtk_window_set_layer_layer (layer_window, GTK_LAYER_SHELL_LAYER_TOP);
    } else if (g_strcmp0 (layer, "Overlay") == 0) {
        gtk_window_set_layer_layer (layer_window, GTK_LAYER_SHELL_LAYER_OVERLAY);
    } else {
        g_assert_not_reached ();
    }
    g_free (layer);
}


static GtkWidget *
layer_selection_new (GtkWindow *layer_window)
{
    GtkWidget *combo_box = gtk_combo_box_text_new ();
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), "Overlay");
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), "Top");
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), "Bottom");
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), "Background");
    g_signal_connect (combo_box, "changed", G_CALLBACK (on_layer_selected), layer_window);
    GtkWidget *label = gtk_label_new ("Layer");
    gtk_widget_set_halign (label, GTK_ALIGN_START);
    GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_set_homogeneous (GTK_BOX (hbox), TRUE);
    gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 12);
    gtk_box_pack_start (GTK_BOX (hbox), combo_box, TRUE, TRUE, 0);
    return hbox;
}

static void
activate (GtkApplication* app, void *_data)
{
    GtkWidget *window = gtk_application_window_new (app);

    gtk_window_init_layer (GTK_WINDOW (window));
    gtk_window_set_layer_anchor (GTK_WINDOW (window), TRUE, FALSE, TRUE, FALSE);
    gtk_window_set_layer_exclusive_zone (GTK_WINDOW (window), 20);

    gtk_window_set_default_size (GTK_WINDOW (window), -1, -1);
    gtk_window_set_title (GTK_WINDOW (window), "Window");

    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    GtkWidget *spacer_button = gtk_button_new_with_label ("Useless");
    gtk_widget_set_tooltip_text (spacer_button, "This is a tooltip");
    gtk_container_add (GTK_CONTAINER (vbox), spacer_button);
    GtkWidget *button = gtk_button_new_with_label ("Menu");
    g_signal_connect (button, "button_press_event",  G_CALLBACK (on_button_press), NULL);
    gtk_container_add (GTK_CONTAINER (vbox), button);
    gtk_container_add (GTK_CONTAINER (vbox), layer_selection_new (GTK_WINDOW (window)));
    gtk_widget_show_all (window);
}

int
main (int argc, char **argv)
{
    GtkApplication * app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return status;
}
