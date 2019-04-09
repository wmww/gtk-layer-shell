#include "gtk-wayland.h"

#include "custom-shell-surface.h"
#include "xdg-popup-surface.h"
#include "gdk-window-hack.h"

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

// This callback only does anything for popups of Wayland surfaces
static void
gtk_wayland_override_on_window_realize (GtkWindow *gtk_window, void *_data)
{
    // Call the super class's realize handler
    GValue args[1] = { G_VALUE_INIT };
    g_value_init_from_instance (&args[0], gtk_window);
    g_signal_chain_from_overridden (args, NULL);
    g_value_unset (&args[0]);

    // TODO: figure out how to move this to gtk_wayland_init_if_needed ()
    gdk_window_hack_init (gtk_widget_get_window (GTK_WIDGET (gtk_window)));

    GtkWindow *transient_for = gtk_window_get_transient_for (gtk_window);
    if (!transient_for) {
        GtkWidget *attached_to = gtk_window_get_attached_to (gtk_window);
        GtkWidget *toplevel = attached_to ? gtk_widget_get_toplevel (attached_to) : NULL;
        transient_for = GTK_IS_WINDOW (toplevel) ? GTK_WINDOW (toplevel) : NULL;
    }
    CustomShellSurface *transient_for_shell_surface = gtk_window_get_custom_shell_surface (transient_for);

    if (transient_for_shell_surface)
    {
        CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (gtk_window);
        XdgPopupSurface *popup_surface = custom_shell_surface_get_xdg_popup (shell_surface);
        g_return_if_fail (shell_surface == (CustomShellSurface *)popup_surface); // make sure the cast succeeded
        if (popup_surface) {
            shell_surface->virtual->unmap (shell_surface);
        } else {
            popup_surface = xdg_popup_surface_new (gtk_window);
        }
        xdg_popup_surface_set_transient_for (popup_surface, transient_for_shell_surface);
    }
}

// This callback must override the default unmap handler, so it can run first
// The custom surface's unmap method must be called before GtkWidget's unmap, or Wayland objects are destroyed in the wrong order
static void
gtk_wayland_override_on_window_unmap (GtkWindow *gtk_window, void *_data)
{
    CustomShellSurface *shell_surface = gtk_window_get_custom_shell_surface (gtk_window);
    if (shell_surface)
        shell_surface->virtual->unmap (shell_surface);

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

// Gets the upper left and size of the portion of the window that is actually used (not shadows and whatnot)
// It does this by walking down the gdk_window tree, as long as there is exactly one child
GdkRectangle
gtk_wayland_get_logical_geom (GtkWindow *gtk_window)
{
    GdkWindow *window = gtk_widget_get_window (GTK_WIDGET (gtk_window));
    GList *list = gdk_window_get_children (window);
    if (list && !list->next) // If there is exactly one child window
        window = list->data;
    GdkRectangle geom;
    gdk_window_get_geometry (window, &geom.x, &geom.y, &geom.width, &geom.height);
    return geom;
}