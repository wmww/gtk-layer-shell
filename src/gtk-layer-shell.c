#include "gtk-layer-shell.h"
#include "gtk-wayland.h"
#include "custom-shell-surface.h"
#include "simple-conversions.h"
#include "layer-surface.h"

void
gtk_window_init_layer (GtkWindow *window)
{
    gtk_wayland_init_if_needed ();
    layer_surface_new (window);
}

void gtk_window_set_layer_layer(GtkWindow *window, GtkLayerShellLayer layer)
{
    CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (window);
    g_return_if_fail (shell_surface);
    LayerSurface *layer_surface = custom_shell_surface_get_layer_surface (shell_surface);
    g_return_if_fail (layer_surface);
    layer_surface_set_layer (layer_surface, gtk_layer_shell_layer_get_zwlr_layer_shell_v1_layer(layer));
}

void gtk_window_set_layer_anchor (GtkWindow *window, gboolean left, gboolean right, gboolean top, gboolean bottom)
{
    CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (window);
    g_return_if_fail (shell_surface);
    LayerSurface *layer_surface = custom_shell_surface_get_layer_surface (shell_surface);
    g_return_if_fail (layer_surface);
    uint32_t bitfield_anchor = (left ? ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT : 0) |
                               (right ? ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT : 0) |
                               (top ? ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP : 0) |
                               (bottom ? ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM : 0);
    layer_surface_set_anchor (layer_surface, bitfield_anchor);
}

void gtk_window_set_layer_exclusive_zone (GtkWindow *window, int exclusive_zone)
{
    CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (window);
    g_return_if_fail (shell_surface);
    LayerSurface *layer_surface = custom_shell_surface_get_layer_surface (shell_surface);
    g_return_if_fail (layer_surface);
    layer_surface_set_exclusive_zone (layer_surface, exclusive_zone);
}
