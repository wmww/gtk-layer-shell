#ifndef GTK_LAYER_SHELL_H
#define GTK_LAYER_SHELL_H

#include <gtk/gtk.h>

// Create a new GtkLayerSurface
// The "namespace" of the layer surface will come from the window's title
// window: The contents of the surface, and it controls the surface's size and visibility
// output: The wl_output the surface should appear on, or NULL for default
// layer: The layer this surface should be on
void gtk_window_init_layer (GtkWindow *window);

void gtk_window_set_layer_anchor (GtkWindow *window, gboolean left, gboolean right, gboolean top, gboolean bottom);

void gtk_window_set_layer_exclusive_zone (GtkWindow *window, int exclusive_zone);

#endif // GTK_LAYER_SHELL_H
