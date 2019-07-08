#include "xdg-toplevel-surface.h"

#include "custom-shell-surface.h"
#include "gtk-wayland.h"
#include "simple-conversions.h"

#include "xdg-shell-client.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>

struct _XdgToplevelSurface
{
    CustomShellSurface super;

    GdkRectangle cached_allocation;
    GdkRectangle geom;

    // These can be NULL
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
};

static void
xdg_surface_handle_configure (void *_data,
                              struct xdg_surface *xdg_surface,
                              uint32_t serial)
{
    (void)_data;

    xdg_surface_ack_configure (xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

static void
xdg_toplevel_handle_configure (void *data,
                               struct xdg_toplevel *_xdg_toplevel,
                               int32_t width,
                               int32_t height,
                               struct wl_array *_states)
{
    XdgToplevelSurface *self = data;
    (void)_xdg_toplevel;
    (void)_states;

    // Technically this should not be applied until we get a xdg_surface.configure
    if (width > 0 || height > 0) {
        GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
        gtk_window_resize (gtk_window, width, height);
    }
    // Ignore the states
}

static void
xdg_toplevel_handle_close (void *data,
                           struct xdg_toplevel *_xdg_toplevel)
{
    XdgToplevelSurface *self = data;
    (void)_xdg_toplevel;

    GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
    gtk_window_close (gtk_window);
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_handle_configure,
    .close = xdg_toplevel_handle_close,
};

static void
xdg_toplevel_surface_map (CustomShellSurface *super, struct wl_surface *wl_surface)
{
    XdgToplevelSurface *self = (XdgToplevelSurface *)super;

    g_return_if_fail (!self->xdg_toplevel);
    g_return_if_fail (!self->xdg_surface);

    struct xdg_wm_base *xdg_wm_base_global = gtk_wayland_get_xdg_wm_base_global ();
    g_return_if_fail (xdg_wm_base_global);

    self->xdg_surface = xdg_wm_base_get_xdg_surface (xdg_wm_base_global, wl_surface);
    g_return_if_fail (self->xdg_surface);
    xdg_surface_add_listener (self->xdg_surface, &xdg_surface_listener, self);

    self->xdg_toplevel = xdg_surface_get_toplevel (self->xdg_surface);

    // name is either static or managed by the window widget
    const char *name = gtk_window_get_title (custom_shell_surface_get_gtk_window (super));
    if (name == NULL)
        name = "gtk-layer-shell";
    xdg_toplevel_set_title (self->xdg_toplevel, name);

    GtkWindow *gtk_window = custom_shell_surface_get_gtk_window (super);
    self->geom = gtk_wayland_get_logical_geom (gtk_window);
    xdg_surface_set_window_geometry (self->xdg_surface,
                                     self->geom.x,
                                     self->geom.y,
                                     self->geom.width,
                                     self->geom.height);
    xdg_toplevel_add_listener (self->xdg_toplevel, &xdg_toplevel_listener, self);

    wl_surface_commit (wl_surface);
    wl_display_roundtrip (gdk_wayland_display_get_wl_display (gdk_display_get_default ()));
}

static void
xdg_toplevel_surface_unmap (CustomShellSurface *super)
{
    XdgToplevelSurface *self = (XdgToplevelSurface *)super;

    if (self->xdg_toplevel) {
        xdg_toplevel_destroy (self->xdg_toplevel);
        self->xdg_toplevel = NULL;
    }

    if (self->xdg_surface) {
        xdg_surface_destroy (self->xdg_surface);
        self->xdg_surface = NULL;
    }
}

static void
xdg_toplevel_surface_finalize (CustomShellSurface *super)
{
    xdg_toplevel_surface_unmap (super);
}

static struct xdg_popup *
xdg_toplevel_surface_get_popup (CustomShellSurface *super,
                             struct xdg_surface *popup_xdg_surface,
                             struct xdg_positioner *positioner)
{
    XdgToplevelSurface *self = (XdgToplevelSurface *)super;

    if (!self->xdg_surface) {
        g_critical ("xdg_toplevel_surface_get_popup () called when the xdg surface wayland object has not yet been created");
        return NULL;
    }

    return xdg_surface_get_popup (popup_xdg_surface, self->xdg_surface, positioner);
}

static GdkRectangle
xdg_toplevel_surface_get_logical_geom (CustomShellSurface *super)
{
    XdgToplevelSurface *self = (XdgToplevelSurface *)super;
    return self->geom;
}

static const CustomShellSurfaceVirtual xdg_toplevel_surface_virtual = {
    .map = xdg_toplevel_surface_map,
    .unmap = xdg_toplevel_surface_unmap,
    .finalize = xdg_toplevel_surface_finalize,
    .get_popup = xdg_toplevel_surface_get_popup,
    .get_logical_geom = xdg_toplevel_surface_get_logical_geom,
};

static void
xdg_toplevel_surface_on_size_allocate (GtkWidget *_widget,
                                       GdkRectangle *allocation,
                                       XdgToplevelSurface *self)
{
    (void)_widget;

    if (self->xdg_surface && !gdk_rectangle_equal (&self->cached_allocation, allocation)) {
        self->cached_allocation = *allocation;
        // allocation only used for catching duplicate calls. To get the correct geom we need to check something else
        GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
        self->geom = gtk_wayland_get_logical_geom (gtk_window);
        xdg_surface_set_window_geometry (self->xdg_surface,
                                         self->geom.x,
                                         self->geom.y,
                                         self->geom.width,
                                         self->geom.height);
    }
}

XdgToplevelSurface *
xdg_toplevel_surface_new (GtkWindow *gtk_window)
{
    g_return_val_if_fail (gtk_wayland_get_xdg_wm_base_global (), NULL);

    XdgToplevelSurface *self = g_new0 (XdgToplevelSurface, 1);
    self->super.virtual = &xdg_toplevel_surface_virtual;
    custom_shell_surface_init ((CustomShellSurface *)self, gtk_window);

    self->cached_allocation = (GdkRectangle) {
        .x = 0,
        .y = 0,
        .width = 0,
        .height = 0,
    };
    self->xdg_surface = NULL;
    self->xdg_toplevel = NULL;

    gtk_window_set_decorated (gtk_window, FALSE);
    g_signal_connect (gtk_window, "size-allocate", G_CALLBACK (xdg_toplevel_surface_on_size_allocate), self);

    return self;
}

XdgToplevelSurface *
custom_shell_surface_get_xdg_toplevel (CustomShellSurface *shell_surface)
{
    if (shell_surface && shell_surface->virtual == &xdg_toplevel_surface_virtual)
        return (XdgToplevelSurface *)shell_surface;
    else
        return NULL;
}

