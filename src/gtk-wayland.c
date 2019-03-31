#include "gtk-wayland.h"

#include "custom-shell-surface.h"

#include "protocol/xdg-shell-client.h"
#include "protocol/wlr-layer-shell-unstable-v1-client.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>

static struct wl_registry *wl_registry_global = NULL;
static struct xdg_wm_base *xdg_wm_base_global = NULL;
static struct zwlr_layer_shell_v1 *layer_shell_global = NULL;

static gboolean has_initialized = FALSE;

gboolean
gtk_wayland_get_has_initialized (void)
{
    return has_initialized;
}

static void
wl_registry_handle_global (void *data,
                           struct wl_registry *registry,
                           uint32_t id,
                           const char *interface,
                           uint32_t version)
{
    // pull out needed globals
    if (strcmp (interface, zwlr_layer_shell_v1_interface.name) == 0) {
        layer_shell_global = wl_registry_bind (registry, id, &zwlr_layer_shell_v1_interface, 1);
    } else if (strcmp (interface, xdg_wm_base_interface.name) == 0) {
        xdg_wm_base_global = wl_registry_bind (registry, id, &xdg_wm_base_interface, 1);
    }
}

static void
wl_registry_handle_global_remove (void *data,
                  struct wl_registry *registry,
                  uint32_t id)
{
    // TODO
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = wl_registry_handle_global,
    .global_remove = wl_registry_handle_global_remove,
};

// This callback must override the default unmap handler, so it can run first
// The custom surface's unmap method must be called before GtkWidget's unmap, or Wayland objects are destroyed in the wrong order
static void
gtk_wayland_override_on_unmap (GtkWindow *gtk_window, void *_data)
{
    CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (gtk_window);
    if (shell_surface)
        shell_surface->virtual->unmap (shell_surface);

    // Call the default unmap handler
    GValue args[1] = { G_VALUE_INIT };
    g_value_init_from_instance (&args[0], gtk_window);
    g_signal_chain_from_overridden (args, NULL);
    g_value_unset (&args[0]);
}

void
gtk_wayland_init_if_needed()
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

    // For popups:
    // gint realize_signal_id = g_signal_lookup ("realize", GTK_TYPE_WINDOW);
    // GClosure *realize_closure = g_cclosure_new (G_CALLBACK (wayland_window_realize_override_cb), NULL, NULL);
    // g_signal_override_class_closure (realize_signal_id, GTK_TYPE_WINDOW, realize_closure);

    gint unmap_signal_id = g_signal_lookup ("unmap", GTK_TYPE_WINDOW);
    GClosure *unmap_closure = g_cclosure_new (G_CALLBACK (gtk_wayland_override_on_unmap), NULL, NULL);
    g_signal_override_class_closure (unmap_signal_id, GTK_TYPE_WINDOW, unmap_closure);

    has_initialized = TRUE;
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
