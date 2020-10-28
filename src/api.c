/* This entire file is licensed under MIT
 *
 * Copyright 2020 William Wold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "gtk-layer-shell.h"
#include "gtk-wayland.h"
#include "custom-shell-surface.h"
#include "simple-conversions.h"
#include "layer-surface.h"
#include "xdg-toplevel-surface.h"

#include <gdk/gdkwayland.h>

guint
gtk_layer_get_major_version ()
{
    return GTK_LAYER_SHELL_MAJOR;
}

guint
gtk_layer_get_minor_version ()
{
    return GTK_LAYER_SHELL_MINOR;
}

guint
gtk_layer_get_micro_version ()
{
    return GTK_LAYER_SHELL_MICRO;
}

gboolean
gtk_layer_is_supported ()
{
    if (!GDK_IS_WAYLAND_DISPLAY (gdk_display_get_default ()))
        return FALSE;
    gtk_wayland_init_if_needed ();
    return gtk_wayland_get_layer_shell_global () != NULL;
}

static LayerSurface*
gtk_window_get_layer_surface (GtkWindow *window)
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

struct zwlr_layer_surface_v1 *
gtk_layer_get_zwlr_layer_surface_v1 (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return NULL; // Error message already shown in gtk_window_get_layer_surface
    return layer_surface->layer_surface;
}

void
gtk_layer_set_namespace (GtkWindow *window, char const* name_space)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_name_space (layer_surface, name_space);
}

const char *
gtk_layer_get_namespace (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    // If layer_surface is NULL, error message already shown in gtk_window_get_layer_surface
    return layer_surface_get_namespace (layer_surface); // NULL-safe
}

void
gtk_layer_set_layer (GtkWindow *window, GtkLayerShellLayer layer)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_layer (layer_surface, layer);
}

GtkLayerShellLayer
gtk_layer_get_layer (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return GTK_LAYER_SHELL_LAYER_TOP; // Error message already shown in gtk_window_get_layer_surface
    return layer_surface->layer;
}

void
gtk_layer_set_monitor (GtkWindow *window, GdkMonitor *monitor)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_monitor (layer_surface, monitor);
}

GdkMonitor *
gtk_layer_get_monitor (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return NULL; // Error message already shown in gtk_window_get_layer_surface
    return layer_surface->monitor;
}

void
gtk_layer_set_anchor (GtkWindow *window, GtkLayerShellEdge edge, gboolean anchor_to_edge)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_anchor (layer_surface, edge, anchor_to_edge);
}

gboolean
gtk_layer_get_anchor (GtkWindow *window, GtkLayerShellEdge edge)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return FALSE; // Error message already shown in gtk_window_get_layer_surface
    g_return_val_if_fail(edge >= 0 && edge < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER, FALSE);
    return layer_surface->anchors[edge];
}

void
gtk_layer_set_margin (GtkWindow *window, GtkLayerShellEdge edge, int margin_size)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_margin (layer_surface, edge, margin_size);
}

int
gtk_layer_get_margin (GtkWindow *window, GtkLayerShellEdge edge)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return 0; // Error message already shown in gtk_window_get_layer_surface
    g_return_val_if_fail(edge >= 0 && edge < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER, FALSE);
    return layer_surface->margins[edge];
}

void
gtk_layer_set_exclusive_zone (GtkWindow *window, int exclusive_zone)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_exclusive_zone (layer_surface, exclusive_zone);
}

int
gtk_layer_get_exclusive_zone (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return 0; // Error message already shown in gtk_window_get_layer_surface
    return layer_surface->exclusive_zone;
}

void
gtk_layer_auto_exclusive_zone_enable (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_auto_exclusive_zone_enable (layer_surface);
}

gboolean
gtk_layer_auto_exclusive_zone_is_enabled (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return FALSE; // Error message already shown in gtk_window_get_layer_surface
    return layer_surface->auto_exclusive_zone;
}

void
gtk_layer_set_keyboard_interactivity (GtkWindow *window, gboolean interacitvity)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return; // Error message already shown in gtk_window_get_layer_surface
    layer_surface_set_keyboard_interactivity (layer_surface, interacitvity);
}

gboolean
gtk_layer_get_keyboard_interactivity (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) return FALSE; // Error message already shown in gtk_window_get_layer_surface
    return layer_surface->keyboard_interactivity;
}
