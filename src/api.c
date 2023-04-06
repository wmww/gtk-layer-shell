/* This entire file is licensed under MIT
 *
 * Copyright 2020 Sophie Winter
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "gtk-layer-shell.h"
#include "gtk-wayland.h"
#include "simple-conversions.h"
#include "layer-surface.h"

#include <gdk/wayland/gdkwayland.h>

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

guint
gtk_layer_get_protocol_version ()
{
    if (!GDK_IS_WAYLAND_DISPLAY (gdk_display_get_default ()))
        return 0;
    gtk_wayland_init_if_needed ();
    struct zwlr_layer_shell_v1 *layer_shell_global = gtk_wayland_get_layer_shell_global ();
    if (!layer_shell_global)
        return 0;
    return zwlr_layer_shell_v1_get_version (layer_shell_global);
}

static LayerSurface*
gtk_window_get_layer_surface_or_warn (GtkWindow *window)
{
    g_return_val_if_fail (window, NULL);
    LayerSurface *layer_surface = gtk_window_get_layer_surface (window);
    if (!layer_surface) {
        g_error (MESSAGE_PREFIX "GtkWindow is not a layer surface. Make sure you called gtk_layer_init_for_window ()");
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
        g_warning ("Shell does not support Layer Shell. Falling back to default GTK behavior");
    }
}

gboolean
gtk_layer_is_layer_window (GtkWindow *window)
{
    return gtk_window_get_layer_surface (window) != NULL;
}

struct zwlr_layer_surface_v1 *
gtk_layer_get_zwlr_layer_surface_v1 (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return NULL;
    return layer_surface->layer_surface;
}

void
gtk_layer_set_namespace (GtkWindow *window, char const* name_space)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return;
    layer_surface_set_name_space (layer_surface, name_space);
}

const char *
gtk_layer_get_namespace (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    return layer_surface_get_namespace (layer_surface); // NULL-safe
}

void
gtk_layer_set_layer (GtkWindow *window, GtkLayerShellLayer layer)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return;
    layer_surface_set_layer (layer_surface, layer);
}

GtkLayerShellLayer
gtk_layer_get_layer (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return GTK_LAYER_SHELL_LAYER_TOP;
    return layer_surface->layer;
}

void
gtk_layer_set_monitor (GtkWindow *window, GdkMonitor *monitor)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return;
    layer_surface_set_monitor (layer_surface, monitor);
}

GdkMonitor *
gtk_layer_get_monitor (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return NULL;
    return layer_surface->monitor;
}

void
gtk_layer_set_anchor (GtkWindow *window, GtkLayerShellEdge edge, gboolean anchor_to_edge)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return;
    layer_surface_set_anchor (layer_surface, edge, anchor_to_edge);
}

gboolean
gtk_layer_get_anchor (GtkWindow *window, GtkLayerShellEdge edge)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return FALSE;
    g_return_val_if_fail(edge >= 0 && edge < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER, FALSE);
    return layer_surface->anchors[edge];
}

void
gtk_layer_set_margin (GtkWindow *window, GtkLayerShellEdge edge, int margin_size)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return;
    layer_surface_set_margin (layer_surface, edge, margin_size);
}

int
gtk_layer_get_margin (GtkWindow *window, GtkLayerShellEdge edge)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return 0;
    g_return_val_if_fail(edge >= 0 && edge < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER, FALSE);
    return layer_surface->margins[edge];
}

void
gtk_layer_set_exclusive_zone (GtkWindow *window, int exclusive_zone)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return;
    layer_surface_set_exclusive_zone (layer_surface, exclusive_zone);
}

int
gtk_layer_get_exclusive_zone (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return 0;
    return layer_surface->exclusive_zone;
}

void
gtk_layer_auto_exclusive_zone_enable (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return;
    layer_surface_auto_exclusive_zone_enable (layer_surface);
}

gboolean
gtk_layer_auto_exclusive_zone_is_enabled (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return FALSE;
    return layer_surface->auto_exclusive_zone;
}

void
gtk_layer_set_keyboard_interactivity (GtkWindow *window, gboolean interactivity)
{
    if (interactivity != TRUE && interactivity != FALSE) {
        g_warning (
            "boolean with value %d sent to gtk_layer_set_keyboard_interactivity (), "
            "perhaps gtk_layer_set_keyboard_mode () was intended?",
            interactivity);
    }
    gtk_layer_set_keyboard_mode (
        window,
        interactivity ? GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE : GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
}

gboolean
gtk_layer_get_keyboard_interactivity (GtkWindow *window)
{
    GtkLayerShellKeyboardMode mode = gtk_layer_get_keyboard_mode (window);
    if (mode != GTK_LAYER_SHELL_KEYBOARD_MODE_NONE && mode != GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE) {
        g_warning (
            "gtk_layer_get_keyboard_interactivity () used after keyboard mode set to %d,"
            "consider using gtk_layer_get_keyboard_mode ().",
            mode);
    }
    return mode != GTK_LAYER_SHELL_KEYBOARD_MODE_NONE;
}

void
gtk_layer_set_keyboard_mode (GtkWindow *window, GtkLayerShellKeyboardMode mode)
{
    g_return_if_fail(mode < GTK_LAYER_SHELL_KEYBOARD_MODE_ENTRY_NUMBER);
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return;
    if (mode != GTK_LAYER_SHELL_KEYBOARD_MODE_NONE && mode != GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE) {
        if (gtk_layer_get_protocol_version () < 4) {
            g_warning (
                "Compositor does not support requested keyboard interactivity mode, using exclusive mode.");
            mode = GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE;
        }
    }
    layer_surface_set_keyboard_mode (layer_surface, mode);
}

GtkLayerShellKeyboardMode
gtk_layer_get_keyboard_mode (GtkWindow *window)
{
    LayerSurface *layer_surface = gtk_window_get_layer_surface_or_warn (window);
    if (!layer_surface) return GTK_LAYER_SHELL_KEYBOARD_MODE_NONE;
    return layer_surface->keyboard_mode;
}
