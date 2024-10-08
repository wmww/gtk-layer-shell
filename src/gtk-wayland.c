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

#include "gtk-wayland.h"

#include "custom-shell-surface.h"
#include "xdg-popup-surface.h"
#include "gtk-priv-access.h"

#include "xdg-shell-client.h"
#include "wlr-layer-shell-unstable-v1-client.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>

static const char *gtk_window_key = "linked-gtk-window";
static const char *popup_position_key = "custom-popup-position";

static struct wl_registry *wl_registry_global = NULL;
static struct xdg_wm_base *xdg_wm_base_global = NULL;
static struct zwlr_layer_shell_v1 *layer_shell_global = NULL;

static gboolean has_initialized = FALSE;

static void 
xdg_wm_base_handle_ping (void *_data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    (void)_data;

    xdg_wm_base_pong (xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_handle_ping,
};

gboolean
gtk_wayland_get_has_initialized (void)
{
    return has_initialized;
}

struct zwlr_layer_shell_v1 *
gtk_wayland_get_layer_shell_global ()
{
    return layer_shell_global;
}

struct xdg_wm_base *
gtk_wayland_get_xdg_wm_base_global ()
{
    return xdg_wm_base_global;
}

static void
wl_registry_handle_global (void *_data,
                           struct wl_registry *registry,
                           uint32_t id,
                           const char *interface,
                           uint32_t version)
{
    (void)_data;

    // pull out needed globals
    if (strcmp (interface, zwlr_layer_shell_v1_interface.name) == 0) {
        g_warn_if_fail (zwlr_layer_shell_v1_interface.version >= 3);
        layer_shell_global = wl_registry_bind (registry,
                                               id,
                                               &zwlr_layer_shell_v1_interface,
                                               MIN((uint32_t)zwlr_layer_shell_v1_interface.version, version));
    } else if (strcmp (interface, xdg_wm_base_interface.name) == 0) {
        g_warn_if_fail (xdg_wm_base_interface.version >= 2);
        xdg_wm_base_global = wl_registry_bind (registry,
                                               id,
                                               &xdg_wm_base_interface,
                                               MIN((uint32_t)xdg_wm_base_interface.version, version));
        xdg_wm_base_add_listener (xdg_wm_base_global, &xdg_wm_base_listener, NULL);
    }
}

static void
wl_registry_handle_global_remove (void *_data,
                                  struct wl_registry *_registry,
                                  uint32_t _id)
{
    (void)_data;
    (void)_registry;
    (void)_id;

    // TODO
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = wl_registry_handle_global,
    .global_remove = wl_registry_handle_global_remove,
};

// Does not take ownership of position
static void
gtk_wayland_setup_custom_popup (GtkWindow *gtk_window, XdgPopupPosition const *position)
{
    CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (gtk_window);
    if (shell_surface) {
        XdgPopupSurface *popup_surface = custom_shell_surface_get_xdg_popup (shell_surface);
        // If there's already a custom surface on the window, it better be a popup
        g_return_if_fail (popup_surface);
        xdg_popup_surface_update_position (popup_surface, position);
    } else {
        xdg_popup_surface_new (gtk_window, position);
    }
}

// This function associates a GTK window with a GDK window
// It overrides the default so it can run for EVERY window without needed to be attached to each one
static void
gtk_wayland_override_on_window_realize (GtkWindow *gtk_window, void *_data)
{
    (void)_data;

    // Call the super class's realize handler
    GValue args[1] = { G_VALUE_INIT };
    g_value_init_from_instance (&args[0], gtk_window);
    g_signal_chain_from_overridden (args, NULL);
    g_value_unset (&args[0]);

    GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (gtk_window));
    g_object_set_data (G_OBJECT (gdk_window), gtk_window_key, gtk_window);

    XdgPopupPosition *position = g_object_get_data (G_OBJECT (gdk_window), popup_position_key);
    if (position) {
        // This is a custom popup waiting to be realized
        gtk_wayland_setup_custom_popup (gtk_window, position);
        g_object_set_data (G_OBJECT (gdk_window), popup_position_key, NULL);
    }
}

// This callback must override the default unmap handler, so it can run first
// The custom surface's unmap method must be called before GtkWidget's unmap, or Wayland objects are destroyed in the wrong order
static void
gtk_wayland_override_on_window_unmap (GtkWindow *gtk_window, void *_data)
{
    (void)_data;

    CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (gtk_window);
    if (shell_surface)
        custom_shell_surface_unmap (shell_surface);

    // Call the super class's unmap handler
    GValue args[1] = { G_VALUE_INIT };
    g_value_init_from_instance (&args[0], gtk_window);
    g_signal_chain_from_overridden (args, NULL);
    g_value_unset (&args[0]);
}

void
gtk_wayland_init_if_needed ()
{
    if (has_initialized)
        return;

    GdkDisplay *gdk_display = gdk_display_get_default ();
    g_return_if_fail (gdk_display);
    g_return_if_fail (GDK_IS_WAYLAND_DISPLAY (gdk_display));

    struct wl_display *wl_display = gdk_wayland_display_get_wl_display (gdk_display);
    wl_registry_global = wl_display_get_registry (wl_display);
    wl_registry_add_listener (wl_registry_global, &wl_registry_listener, NULL);
    wl_display_roundtrip (wl_display);

    if (!layer_shell_global)
        g_warning ("It appears your Wayland compositor does not support the Layer Shell protocol");

    if (!xdg_wm_base_global)
        g_warning ("It appears your Wayland compositor does not support the XDG Shell stable protocol");

    gint realize_signal_id = g_signal_lookup ("realize", GTK_TYPE_WINDOW);
    GClosure *realize_closure = g_cclosure_new (G_CALLBACK (gtk_wayland_override_on_window_realize), NULL, NULL);
    g_signal_override_class_closure (realize_signal_id, GTK_TYPE_WINDOW, realize_closure);

    gint unmap_signal_id = g_signal_lookup ("unmap", GTK_TYPE_WINDOW);
    GClosure *unmap_closure = g_cclosure_new (G_CALLBACK (gtk_wayland_override_on_window_unmap), NULL, NULL);
    g_signal_override_class_closure (unmap_signal_id, GTK_TYPE_WINDOW, unmap_closure);

    has_initialized = TRUE;
}

GtkWindow *
gtk_wayland_gdk_to_gtk_window (GdkWindow *gdk_window)
{
    return GTK_WINDOW (g_object_get_data (G_OBJECT (gdk_window), gtk_window_key));
}

void
gtk_wayland_setup_window_as_custom_popup (GdkWindow *gdk_window, XdgPopupPosition const *position)
{
    GtkWindow *gtk_window = gtk_wayland_gdk_to_gtk_window (gdk_window);
    if (GTK_IS_WINDOW (gtk_window)) {
        // The GDK window has been connected to a GTK window
        gtk_wayland_setup_custom_popup (gtk_window, position);
    } else {
        // We need to hold the position and wait for a connected GTK window to be realized
        XdgPopupPosition *position_owned = g_new (XdgPopupPosition, 1);
        *position_owned = *position;
        g_object_set_data_full (G_OBJECT (gdk_window), popup_position_key, position_owned, g_free);
    }
}
