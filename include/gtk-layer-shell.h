#ifndef GTK_LAYER_SHELL_H
#define GTK_LAYER_SHELL_H

#include <gtk/gtk.h>

// Create a new GtkLayerSurface
// The "namespace" of the layer surface will come from the window's title
// window: The contents of the surface, and it controls the surface's size and visibility
// output: The wl_output the surface should appear on, or NULL for default
// layer: The layer this surface should be on
void gtk_window_init_layer (GtkWindow *window);

// Matches the layer shell protocol enum exactly
// The values can never be removed or changed, so hard-coding a copy here should be fine
typedef enum {
	GTK_LAYER_SHELL_LAYER_BACKGROUND = 0,
	GTK_LAYER_SHELL_LAYER_BOTTOM = 1,
	GTK_LAYER_SHELL_LAYER_TOP = 2,
	GTK_LAYER_SHELL_LAYER_OVERLAY = 3,
} GtkLayerShellLayer;

void gtk_window_set_layer_layer(GtkWindow *window, GtkLayerShellLayer layer);

void gtk_window_set_layer_anchor (GtkWindow *window, gboolean left, gboolean right, gboolean top, gboolean bottom);

void gtk_window_set_layer_exclusive_zone (GtkWindow *window, int exclusive_zone);

#endif // GTK_LAYER_SHELL_H
