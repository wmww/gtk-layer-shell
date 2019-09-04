#ifndef GTK_LAYER_SHELL_H
#define GTK_LAYER_SHELL_H

#include <gtk/gtk.h>

/**
 * SECTION:gtk-layer-shell
 * @title: Gtk Layer Shell
 * @short_description: A library to write GTK Applications using Layer Shell
 *
 * insert more general verbiage here
 *
 * # Forcing Window Size
 * If you wish to force your layer surface window to be a different size than it
 * is by default:
 * |[<!-- language="C" -->
 *   gtk_widget_set_size_request (GTK_WIDGET (layer_gtk_window), width, height);
 *   // force the window to resize to the new request
 *   gtk_window_resize (layer_gtk_window, 1, 1);
 * ]|
 * If width or height is -1, the default is used for that axis. If the window is
 * anchored to opposite edges of the output (see gtk_layer_set_anchor ()), the 
 * size request is ignored. If you later wish to use the default window size,
 * simply repeat the two calls but with both width and height as -1.
 */

G_BEGIN_DECLS

/**
 * GtkLayerShellLayer:
 * @GTK_LAYER_SHELL_LAYER_BACKGROUND: The background layer.
 * @GTK_LAYER_SHELL_LAYER_BOTTOM: The bottom layer.
 * @GTK_LAYER_SHELL_LAYER_TOP: The top layer.
 * @GTK_LAYER_SHELL_LAYER_OVERLAY: The overlay layer.
 * @GTK_LAYER_SHELL_LAYER_ENTRY_NUMBER: Should not be used except to get the number of entries
 */
typedef enum {
    GTK_LAYER_SHELL_LAYER_BACKGROUND,
    GTK_LAYER_SHELL_LAYER_BOTTOM,
    GTK_LAYER_SHELL_LAYER_TOP,
    GTK_LAYER_SHELL_LAYER_OVERLAY,
    GTK_LAYER_SHELL_LAYER_ENTRY_NUMBER,
} GtkLayerShellLayer;

/**
 * GtkLayerShellEdge:
 * @GTK_LAYER_SHELL_EDGE_LEFT: The left edge of the screen.
 * @GTK_LAYER_SHELL_EDGE_RIGHT: The right edge of the screen.
 * @GTK_LAYER_SHELL_EDGE_TOP: The top edge of the screen.
 * @GTK_LAYER_SHELL_EDGE_BOTTOM: The bottom edge of the screen.
 * @GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER: Should not be used except to get the number of entries
 */
typedef enum {
    GTK_LAYER_SHELL_EDGE_LEFT = 0,
    GTK_LAYER_SHELL_EDGE_RIGHT,
    GTK_LAYER_SHELL_EDGE_TOP,
    GTK_LAYER_SHELL_EDGE_BOTTOM,
    GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER, // Should not be used except to get the number of entries
} GtkLayerShellEdge;

/**
 * gtk_layer_init_for_window:
 * @window: A #GtkWindow to be turned into a layer surface.
 *
 * Set the @window up to be a layer surface once it is mapped:
 * This must be called before the @window is realized.
 */
void gtk_layer_init_for_window (GtkWindow *window);

/**
 * gtk_layer_set_namespace:
 * @window: A layer surface.
 * @name_space: The namespace of this layer surface.
 *
 * Set the "namespace" of the surface.
 *
 * No one is quite sure what this is for, but it probably should be something generic
 * ("panel", "osk", etc). The @name_space string is copied, and caller maintians
 * ownership of original. If the window is currently mapped, it will get remapped so
 * the change can take effect.
 *
 * Default is "gtk-layer-shell" (which will be used if set to %NULL)
 */
void gtk_layer_set_namespace (GtkWindow *window, char const* name_space);

/**
 * gtk_layer_set_layer:
 * @window: A layer surface.
 * @layer: The layer on which this surface appears.
 *
 * Set the "layer" on which the surface appears (controls if it is over top of or below other surfaces). If the @window is currently mapped, it will get remapped so the change
 * can take effect.
 *
 * Default is #GTK_LAYER_SHELL_LAYER_TOP
 */
void gtk_layer_set_layer (GtkWindow *window, GtkLayerShellLayer layer);

/**
 * gtk_layer_set_monitor:
 * @window: A layer surface.
 * @monitor: The output this layer surface will be placed on (%NULL to let the compositor decide).
 *
 * Set the output for the window to be placed on, or %NULL to let the compositor choose.
 * If the window is currently mapped, it will get remapped so the change can take effect.
 *
 * Default is %NULL
 */
void gtk_layer_set_monitor (GtkWindow *window, GdkMonitor *monitor);

/**
 * gtk_layer_set_anchor:
 * @window: A layer surface.
 * @edge: A #GtkLayerShellEdge this layer suface may be anchored to.
 * @anchor_to_edge: Whether or not to anchor this layer surface to @edge.
 *
 * Set whether @window should be anchored to @edge.
 * - If two perpendicular edges are anchored, the surface with be anchored to that corner
 * - If two opposite edges are anchored, the window will be stretched across the screen in that direction
 *
 * Default is %FALSE for each #GtkLayerShellEdge
 */
void gtk_layer_set_anchor (GtkWindow *window, GtkLayerShellEdge edge, gboolean anchor_to_edge);

/**
 * gtk_layer_set_margin:
 * @window: A layer surface.
 * @edge: The #GtkLayerShellEdge for which to set the margin.
 * @margin_size: The margin for @edge to be set.
 *
 * Set the margin for a specific @edge of a @window. Effects both surface's distance from
 * the edge and its exclusive zone size (if auto exclusive zone enabled).
 *
 * Default is 0 for each #GtkLayerShellEdge
 */
void gtk_layer_set_margin (GtkWindow *window, GtkLayerShellEdge edge, int margin_size);

/**
 * gtk_layer_set_exclusive_zone:
 * @window: A layer surface.
 * @exclusive_zone: The size of the exclusive zone.
 *
 * If auto exclusive zone is enabled, exclusive zone will automatically be set to the
 * size of the @window + relevant margin. To disable auto exclusive zone, just set the
 * exclusive zone to 0 or any other fixed value. There is no need to manually set the
 * exclusive zone size when using auto exclusive zone.
 *
 * Default is 0
 */
void gtk_layer_set_exclusive_zone (GtkWindow *window, int exclusive_zone);

/**
 * gtk_layer_auto_exclusive_zone_enable:
 * @window: A layer surface.
 *
 * Enables auto exclusive zone for @window.
 */
void gtk_layer_auto_exclusive_zone_enable (GtkWindow *window);

/**
 * gtk_layer_set_keyboard_interactivity:
 * @window: A layer surface.
 * @interacitvity: Whether the layer surface should receive keyboard events.
 *
 * Whether the @window should receive keyboard events from the compositor.
 *
 * Default is %FALSE
 */
void gtk_layer_set_keyboard_interactivity (GtkWindow *window, gboolean interacitvity);

G_END_DECLS

#endif // GTK_LAYER_SHELL_H
