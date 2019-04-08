#ifndef GTK_LAYER_SHELL_H
#define GTK_LAYER_SHELL_H

#include <gtk/gtk.h>

// Set the window up to be a layer surface once it is mapped
// This must be called before the window is realized
// The "namespace" of the layer surface will come from the window's title
void gtk_layer_init_for_window (GtkWindow *window);

// The layers a layer surface can be on, see the layer shell protocol for more info
typedef enum {
	GTK_LAYER_BACKGROUND,
	GTK_LAYER_BOTTOM,
	GTK_LAYER_TOP,
	GTK_LAYER_OVERLAY,
} GtkLayerShellLayer;

void gtk_layer_set_layer (GtkWindow *window, GtkLayerShellLayer layer);

// Set whether the surface is anchored to each edge, defaults to FALSE for all
// If two opposite edges are anchored, the window will be stretched across the screen in that direction, Unless the
//   window's default size has been set
void gtk_layer_set_anchor_left (GtkWindow *window, gboolean anchor_left);
void gtk_layer_set_anchor_right (GtkWindow *window, gboolean anchor_right);
void gtk_layer_set_anchor_top (GtkWindow *window, gboolean anchor_top);
void gtk_layer_set_anchor_bottom (GtkWindow *window, gboolean anchor_bottom);

void gtk_layer_set_exclusive_zone (GtkWindow *window, int exclusive_zone);

#endif // GTK_LAYER_SHELL_H
