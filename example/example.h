#ifndef GTK_LAYER_SHELL_EXAMPLE_H
#define GTK_LAYER_SHELL_EXAMPLE_H

#include "gtk-layer-shell.h"
#include <gtk/gtk.h>

typedef enum {
    WINDOW_ORIENTATION_NONE = 0,
    WINDOW_ORIENTATION_HORIZONTAL,
    WINDOW_ORIENTATION_VERTICAL,
} WindowOrientation;

// The layer_window has am "orientation-changed" signal, whos argument is WindowOrientation

GtkWidget *
layer_selection_new (GtkWindow *layer_window, GtkLayerShellLayer default_layer);

GtkWidget *
anchor_control_new (GtkWindow *layer_window,
                    gboolean default_left,
                    gboolean default_right,
                    gboolean default_top,
                    gboolean default_bottom);

#endif // GTK_LAYER_SHELL_EXAMPLE_H
