#ifndef GTK_LAYER_SHELL_H
#define GTK_LAYER_SHELL_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

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

// Set the "namespace" of the surface
// No one is quite sure what this is for, but it probably should be something generic ("panel", "osk", etc)
// String is copied, and caller maintians ownership of original
// If the window is currently mapped, it will get remapped so the change can take effect
// Default is "gtk-layer-shell" (which will be used if set to NULL)
void gtk_layer_set_namespace (GtkWindow *window, char const* name_space);

// Set the "layer" on which the surface appears (controls if it is over top of or below other surfaces)
// If the window is currently mapped, it will get remapped so the change can take effect
// Default is GTK_LAYER_SHELL_LAYER_TOP
void gtk_layer_set_layer (GtkWindow *window, GtkLayerShellLayer layer);

// Set the output for the window to be placed on, or NULL to let the compositor choose
// If the window is currently mapped, it will get remapped so the change can take effect
// Default is NULL
void gtk_layer_set_monitor (GtkWindow *window, GdkMonitor *monitor);

// Set if the surface is anchored to an edge
// If two opposite edges are anchored, the window will be stretched across the screen in that direction
// (unless the window's default size has been set, in which case it will be centered)
// Default is FALSE for all
void gtk_layer_set_anchor (GtkWindow *window, GtkLayerShellEdge edge, gboolean anchor_to_edge);

// Set the margin for a specific edge of a window
// Effects both surface's distance from the edge and exclusive zone size (if auto exclusive zone enabled)
// Default is 0 for all
void gtk_layer_set_margin (GtkWindow *window, GtkLayerShellEdge edge, int margin_size);

// If auto exclusive zone is enabled, exclusive zone will be set to the size of the window + relevant margin
// To disable auto exclusive zone, just set the exclusive zone to 0 or any other fixed value
// Default is 0
void gtk_layer_set_exclusive_zone (GtkWindow *window, int exclusive_zone);
void gtk_layer_auto_exclusive_zone_enable (GtkWindow *window);

// If the window should recieve keyboard events from the compositor
// Default is FALSE
void gtk_layer_set_keyboard_interactivity (GtkWindow *window, gboolean interacitvity);

G_END_DECLS

#endif // GTK_LAYER_SHELL_H
