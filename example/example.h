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
anchor_control_new (GtkWindow *layer_window, const gboolean default_anchors[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER]);

GtkWidget *
margin_control_new (GtkWindow *layer_window, const int default_margins[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER]);

GtkWidget *
mscl_toggles_new (GtkWindow *layer_window,
                  gboolean default_auto_exclusive_zone,
                  gboolean default_keyboard_interactivity);

GtkWidget *
menu_bar_new (GtkWindow *layer_window);

#endif // GTK_LAYER_SHELL_EXAMPLE_H
