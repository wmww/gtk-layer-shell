#include "gtk-layer-shell.h"

#include <gtk/gtk.h>

static void window_realize_cb (GtkWidget *window, void *_data)
{
    if (!gtk_layer_shell_has_initialized ())
        wayland_shell_surface_global_init (NULL);
    WaylandShellSurface *layer_surface = wayland_shell_surface_new_layer_surface (GTK_WINDOW (window), NULL, 2, "gtk_example");
    wayland_shell_surface_set_layer_surface_info (layer_surface, 4, 20);
}

static void
wayland_popup_map_callback (WaylandShellSurface *shell_surface)
{
    GdkPoint offset = {0, 12};
    GdkGravity anchor = GDK_GRAVITY_SOUTH_WEST;
    GdkGravity gravity = GDK_GRAVITY_SOUTH_EAST;
    wayland_shell_surface_map_popup (shell_surface, anchor, gravity, offset);
}

static void activate (GtkApplication* app, void *_data)
{
    wayland_shell_surface_global_init (wayland_popup_map_callback);
    GtkWidget *window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
    GtkWidget *button = gtk_button_new_with_label ("Hello");
    gtk_widget_set_tooltip_text (button, "This is a tooltip");
    gtk_container_add (GTK_CONTAINER (window), button);
    g_signal_connect (window, "realize", G_CALLBACK (window_realize_cb), NULL);
    gtk_widget_show_all (window);
}

int main (int argc, char **argv)
{
    GtkApplication * app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return status;
}
