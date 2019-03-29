#include "gtk-layer-shell.h"

#include <gtk/gtk.h>

static void
on_window_realize (GtkWidget *window, void *_data)
{
    WaylandShellSurface *layer_surface = wayland_shell_surface_new_layer_surface (GTK_WINDOW (window), NULL, 2, "gtk_example");
    wayland_shell_surface_set_layer_surface_info (layer_surface, 4, 20);
}

static void
on_wayland_popup_map (WaylandShellSurface *shell_surface)
{
    GdkPoint offset = {0, 12};
    GdkGravity anchor = GDK_GRAVITY_SOUTH_WEST;
    GdkGravity gravity = GDK_GRAVITY_SOUTH_EAST;
    wayland_shell_surface_map_popup (shell_surface, anchor, gravity, offset);
}

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
activate (GtkApplication* app, void *_data)
{
    wayland_shell_surface_global_init (on_wayland_popup_map);

    GtkWidget *window = gtk_application_window_new (app);
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    gtk_window_set_title (GTK_WINDOW (window), "Window");
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    GtkWidget *button = gtk_button_new_with_label ("Hello");
    gtk_widget_set_tooltip_text (button, "This is a tooltip");
    g_signal_connect (button, "button_press_event",  G_CALLBACK (on_button_press), NULL);
    gtk_container_add (GTK_CONTAINER (vbox), button);
    g_signal_connect (window, "realize", G_CALLBACK (on_window_realize), NULL);
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
