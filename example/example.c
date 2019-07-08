#include "gtk-layer-shell.h"
#include <gtk/gtk.h>

static void
activate (GtkApplication* app, void *_data)
{
    (void)_data;

    // Create a normal GTK window however you like
    GtkWindow *gtk_window = GTK_WINDOW (gtk_application_window_new (app));

    // Before the window is first realized, set it up to be a layer surface
    gtk_layer_init_for_window (gtk_window);

    // Order below normal windows
    gtk_layer_set_layer (gtk_window, GTK_LAYER_SHELL_LAYER_BOTTOM);

    // Push other windows out of the way
    gtk_layer_auto_exclusive_zone_enable (gtk_window);

    // We don't need to get keybard input
    // gtk_layer_set_keyboard_interactivity (gtk_window, FALSE); // FALSE is default

    // The margins are the gaps around the window's edges
    // Margins and anchors can be set like this...
    gtk_layer_set_margin (gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, 40);
    gtk_layer_set_margin (gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, 40);
    gtk_layer_set_margin (gtk_window, GTK_LAYER_SHELL_EDGE_TOP, 20);
    // gtk_layer_set_margin (gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, 0); // 0 is default

    // ... or like this
    // Anchors are if the window is pinned to each edge of the output
    static const gboolean anchors[] = {TRUE, TRUE, FALSE, TRUE};
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        gtk_layer_set_anchor (gtk_window, i, anchors[i]);
    }

    // Set up a widget
    GtkWidget *label = gtk_label_new ("");
    gtk_label_set_markup (GTK_LABEL (label),
                          "<span font_desc=\"20.0\">"
                              "GTK Layer Shell example!"
                          "</span>");
    gtk_container_add (GTK_CONTAINER (gtk_window), label);
    gtk_container_set_border_width (GTK_CONTAINER (gtk_window), 12);
    gtk_widget_show_all (GTK_WIDGET (gtk_window));
}

int
main (int argc, char **argv)
{
    GtkApplication * app = gtk_application_new ("sh.wmww.gtk-layer-example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return status;
}
