#ifndef GTK_LAYER_SHELL_EXAMPLE_H
#define GTK_LAYER_SHELL_EXAMPLE_H

#include "gtk-layer-shell.h"
#include <gtk/gtk.h>

GtkWidget *
layer_selection_new (GtkWindow *layer_window, GtkLayerShellLayer starting_layer);

GtkWidget *
anchor_control_new (GtkWindow *layer_window,
                    gboolean default_left,
                    gboolean default_right,
                    gboolean default_top,
                    gboolean default_bottom);

#endif // GTK_LAYER_SHELL_EXAMPLE_H
