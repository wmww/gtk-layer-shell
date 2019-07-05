#include "gtk-layer-shell.h"
#include "gtk-wayland.h"
#include "custom-shell-surface.h"
#include "simple-conversions.h"
#include "layer-surface.h"
#include "xdg-toplevel-surface.h"

static
LayerSurface* gtk_window_get_layer_surface (GtkWindow *window)
{
    g_return_val_if_fail (window, NULL);
    CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (window);
    if (!shell_surface) {
        g_critical ("GtkWindow is not a layer surface. Make sure you called gtk_layer_init_for_window ()");
        return NULL;
    }
    LayerSurface *layer_surface = custom_shell_surface_get_layer_surface (shell_surface);
    if (!layer_surface) {
        g_critical ("Custom wayland shell surface is not a layer surface, your Wayland compositor may not support Layer Shell");
        return NULL;
    }
    return layer_surface;
}

void
gtk_layer_init_for_window (GtkWindow *window)
{
    gtk_wayland_init_if_needed ();
    LayerSurface* layer_surface = layer_surface_new (window);
    if (!layer_surface) {
        g_warning ("Falling back to XDG shell instead of Layer Shell (surface should appear but layer features will not work)");
        XdgToplevelSurface* toplevel_surface = xdg_toplevel_surface_new (window);
        if (!toplevel_surface)
        {
            g_warning ("Shell does not support XDG shell stable. Falling back to default GTK behavior");
        }
    }
}

void
gtk_layer_set_namespace (GtkWindow *window, char const* name_space)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_name_space (layer_surface, name_space);
}

void
gtk_layer_set_layer (GtkWindow *window, GtkLayerShellLayer layer)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_layer (layer_surface, gtk_layer_shell_layer_get_zwlr_layer_shell_v1_layer(layer));
}

void
gtk_layer_set_monitor (GtkWindow *window, GdkMonitor *monitor)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_monitor (layer_surface, monitor);
}

void
gtk_layer_set_anchor (GtkWindow *window, GtkLayerShellEdge edge, gboolean anchor_to_edge)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_anchor (layer_surface, edge, anchor_to_edge);
}

void
gtk_layer_set_margin (GtkWindow *window, GtkLayerShellEdge edge, int margin_size)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_margin (layer_surface, edge, margin_size);
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
