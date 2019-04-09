#include "gtk-layer-shell.h"
#include "gtk-wayland.h"
#include "custom-shell-surface.h"
#include "simple-conversions.h"
#include "layer-surface.h"

static LayerSurface* gtk_window_get_layer_surface (GtkWindow *window)
{
    g_return_val_if_fail (window, NULL);
    CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (window);
    if (!shell_surface) {
        g_critical ("GtkWindow is not a layer surface, make sure you called gtk_layer_init_for_window () first");
        return NULL;
    }
    LayerSurface *layer_surface = custom_shell_surface_get_layer_surface (shell_surface);
    if (!layer_surface) {
        g_critical ("Custom wayland shell surface is not a layer surface");
        return NULL;
    }
    return layer_surface;
}

void
gtk_layer_init_for_window (GtkWindow *window)
{
    gtk_wayland_init_if_needed ();
    layer_surface_new (window);
}

void
gtk_layer_set_layer (GtkWindow *window, GtkLayerShellLayer layer)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_layer (layer_surface, gtk_layer_shell_layer_get_zwlr_layer_shell_v1_layer(layer));
}

static void
gtk_layer_set_anchor (GtkWindow *window, uint32_t edge, gboolean anchor_to_edge)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    uint32_t anchor = layer_surface_get_anchor (layer_surface);
    if (anchor_to_edge) {
        anchor |= edge;
    } else {
        anchor &= ~edge;
    }
    layer_surface_set_anchor (layer_surface, anchor);
}

void gtk_layer_set_anchor_left (GtkWindow *window, gboolean anchor_left)
{
    gtk_layer_set_anchor (window, ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT, anchor_left);
}

void gtk_layer_set_anchor_right (GtkWindow *window, gboolean anchor_right)
{
    gtk_layer_set_anchor (window, ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT, anchor_right);
}

void gtk_layer_set_anchor_top (GtkWindow *window, gboolean anchor_top)
{
    gtk_layer_set_anchor (window, ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP, anchor_top);
}

void gtk_layer_set_anchor_bottom (GtkWindow *window, gboolean anchor_bottom)
{
    gtk_layer_set_anchor (window, ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM, anchor_bottom);
}

void
gtk_layer_set_exclusive_zone (GtkWindow *window, int exclusive_zone)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_exclusive_zone (layer_surface, exclusive_zone);
}

void
gtk_layer_auto_exclusive_zone_enable (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_auto_exclusive_zone_enable (layer_surface);
}

void
gtk_layer_set_keyboard_interactivity (GtkWindow *window, gboolean interacitvity)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_keyboard_interactivity (layer_surface, interacitvity);
}