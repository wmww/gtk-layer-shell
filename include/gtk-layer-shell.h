#ifndef GTK_LAYER_SHELL_H
#define GTK_LAYER_SHELL_H

#include <gtk/gtk.h>

G_BEGIN_DECLS // literally just the extern "C" if C++ thing

// The layers a layer surface can be on, see the layer shell protocol for more info
typedef enum {
    GTK_LAYER_SHELL_LAYER_BACKGROUND,
    GTK_LAYER_SHELL_LAYER_BOTTOM,
    GTK_LAYER_SHELL_LAYER_TOP,
    GTK_LAYER_SHELL_LAYER_OVERLAY,
    GTK_LAYER_SHELL_LAYER_ENTRY_NUMBER,
} GtkLayerShellLayer;

typedef enum {
    GTK_LAYER_SHELL_EDGE_LEFT = 0,
    GTK_LAYER_SHELL_EDGE_RIGHT,
    GTK_LAYER_SHELL_EDGE_TOP,
    GTK_LAYER_SHELL_EDGE_BOTTOM,
    GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER,
} GtkLayerShellEdge;

// Set the window up to be a layer surface once it is mapped
// This must be called before the window is realized
// The "namespace" of the layer surface will come from the window's title
void gtk_layer_init_for_window (GtkWindow *window);

void gtk_layer_set_layer (GtkWindow *window, GtkLayerShellLayer layer);

// Set if the surface is anchored to an edge, defaults to FALSE for all
// If two opposite edges are anchored, the window will be stretched across the screen in that direction
// (unless the window's default size has been set)
void gtk_layer_set_anchor (GtkWindow *window, GtkLayerShellEdge edge, gboolean anchor_to_edge);

// If auto exclusive zone is enabled, the exclusive zone will be set to the size of the window
// To disable auto exclusive zone, just set the exclusive zone to 0 or any other fixed value
void gtk_layer_set_exclusive_zone (GtkWindow *window, int exclusive_zone);
void gtk_layer_auto_exclusive_zone_enable (GtkWindow *window);

void gtk_layer_set_keyboard_interactivity (GtkWindow *window, gboolean interacitvity);

G_END_DECLS

#endif // GTK_LAYER_SHELL_H
