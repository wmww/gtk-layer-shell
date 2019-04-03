#include "xdg-popup-surface.h"

#include "custom-shell-surface.h"
#include "gtk-wayland.h"
#include "gdk-window-hack.h"
#include "simple-conversions.h"

#include "protocol/xdg-shell-client.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>

struct _XdgPopupSurface
{
    CustomShellSurface super;

    CustomShellSurface *transient_for;
    GdkRectangle cached_allocation;

    // These can be NULL
    struct xdg_surface *xdg_surface;
    struct xdg_popup *xdg_popup;
};

static void
xdg_surface_handle_configure (void *data,
                              struct xdg_surface *xdg_surface,
                              uint32_t serial)
{
    xdg_surface_ack_configure (xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

static void
xdg_popup_handle_configure (void *data,
                            struct xdg_popup *xdg_popup,
                            int32_t x,
                            int32_t y,
                            int32_t width,
                            int32_t height)
{
    XdgPopupSurface *self = data;
    if (width > 0 && height > 0) {
        GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
        gtk_window_resize (gtk_window, width, height);
        GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (gtk_window));
        g_return_if_fail (gdk_window);
        // calculating the correct values is hard, but we're not required to provide them
        g_signal_emit_by_name (gdk_window, "moved-to-rect", NULL, NULL, FALSE, FALSE);
    }
}

static void
xdg_popup_handle_popup_done (void *data,
                             struct xdg_popup *xdg_popup)
{
    XdgPopupSurface *self = data;
    GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
    gtk_widget_unmap (GTK_WIDGET (gtk_window));
}

static const struct xdg_popup_listener xdg_popup_listener = {
    .configure = xdg_popup_handle_configure,
    .popup_done = xdg_popup_handle_popup_done,
};

static void
xdg_popup_surface_map (CustomShellSurface *super, struct wl_surface *wl_surface)
{
    XdgPopupSurface *self = (XdgPopupSurface *)super;

    g_return_if_fail (!self->xdg_popup);
    g_return_if_fail (!self->xdg_surface);
    g_return_if_fail (self->transient_for);

    GtkWindow *gtk_window = custom_shell_surface_get_gtk_window (super);
    GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (gtk_window));
    g_return_if_fail (gdk_window);
    GdkWinowHackPosition *position = gtk_window_hack_get_position (gdk_window);
    g_return_if_fail (position);
    struct xdg_wm_base *xdg_wm_base_global = gtk_wayland_get_xdg_wm_base_global ();
    g_return_if_fail (xdg_wm_base_global);
    struct xdg_positioner *positioner = xdg_wm_base_create_positioner (xdg_wm_base_global);
    GdkRectangle popup_geom = gtk_wayland_get_logical_geom (gtk_window);
    enum xdg_positioner_anchor anchor = gdk_gravity_get_xdg_positioner_anchor(position->rect_anchor);
    enum xdg_positioner_gravity gravity = gdk_gravity_get_xdg_positioner_gravity(position->window_anchor);
    enum xdg_positioner_constraint_adjustment constraint_adjustment =
        gdk_anchor_hints_get_xdg_positioner_constraint_adjustment (position->anchor_hints);

    xdg_positioner_set_size (positioner, popup_geom.width, popup_geom.height);
    xdg_positioner_set_anchor_rect (positioner,
                                    position->rect.x, position->rect.y,
                                    position->rect.width, position->rect.height);
    xdg_positioner_set_offset (positioner, position->rect_anchor_dx, position->rect_anchor_dy);
    xdg_positioner_set_anchor (positioner, anchor);
    xdg_positioner_set_gravity (positioner, gravity);
    xdg_positioner_set_constraint_adjustment (positioner, constraint_adjustment);

    self->xdg_surface = xdg_wm_base_get_xdg_surface (xdg_wm_base_global, wl_surface);
    g_return_if_fail (self->xdg_surface);
    xdg_surface_add_listener (self->xdg_surface, &xdg_surface_listener, self);

    self->xdg_popup = self->transient_for->virtual->get_popup (self->transient_for, self->xdg_surface, positioner);
    g_return_if_fail (self->xdg_popup);
    xdg_popup_add_listener (self->xdg_popup, &xdg_popup_listener, self);

    xdg_positioner_destroy (positioner);

    wl_surface_commit (wl_surface);
    wl_display_roundtrip (gdk_wayland_display_get_wl_display (gdk_display_get_default ()));
}

static void
xdg_popup_surface_unmap (CustomShellSurface *super)
{
    XdgPopupSurface *self = (XdgPopupSurface *)super;

    if (self->xdg_popup) {
        xdg_popup_destroy (self->xdg_popup);
        self->xdg_popup = NULL;
    }

    if (self->xdg_surface) {
        xdg_surface_destroy (self->xdg_surface);
        self->xdg_surface = NULL;
    }
}

static struct xdg_popup *
xdg_popup_surface_get_popup (CustomShellSurface *super,
                             struct xdg_surface *popup_xdg_surface,
                             struct xdg_positioner *positioner)
{
    XdgPopupSurface *self = (XdgPopupSurface *)super;

    if (!self->xdg_surface) {
        g_critical ("xdg_popup_surface_get_popup () called when the xdg surface wayland object has not yet been created");
        return NULL;
    }

    return xdg_surface_get_popup (popup_xdg_surface, self->xdg_surface, positioner);
}

static const CustomShellSurfaceVirtual xdg_popup_surface_virtual = {
    .map = xdg_popup_surface_map,
    .unmap = xdg_popup_surface_unmap,
    .finalize = xdg_popup_surface_unmap, // nothing but unmapping is needed to finalize
    .get_popup = xdg_popup_surface_get_popup,
};

static void
xdg_popup_surface_on_size_allocate (GtkWidget *widget,
                                    GdkRectangle *allocation,
                                    XdgPopupSurface *self)
{
    if (self->xdg_surface && !gdk_rectangle_equal (&self->cached_allocation, allocation)) {
        self->cached_allocation = *allocation;
        // allocation only used for catching duplicate calls. To get the correct geom we need to check something else
        GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
        GdkRectangle new_geom = gtk_wayland_get_logical_geom (gtk_window);
        xdg_surface_set_window_geometry (self->xdg_surface, new_geom.x, new_geom.y, new_geom.width, new_geom.height);
    }
}

XdgPopupSurface *
xdg_popup_surface_new (GtkWindow *gtk_window)
{
    XdgPopupSurface *self = g_new0 (XdgPopupSurface, 1);
    self->super.virtual = &xdg_popup_surface_virtual;
    custom_shell_surface_init ((CustomShellSurface *)self, gtk_window);

    self->transient_for = NULL;
    self->cached_allocation = (GdkRectangle) {
        .x = 0,
        .y = 0,
        .width = 0,
        .height = 0,
    };
    self->xdg_surface = NULL;
    self->xdg_popup = NULL;

    g_signal_connect (gtk_window, "size-allocate", G_CALLBACK (xdg_popup_surface_on_size_allocate), self);

    return self;
}

XdgPopupSurface *
custom_shell_surface_get_xdg_popup (CustomShellSurface *shell_surface)
{
    if (shell_surface && shell_surface->virtual == &xdg_popup_surface_virtual)
        return (XdgPopupSurface *)shell_surface;
    else
        return NULL;
}

void
xdg_popup_surface_set_transient_for (XdgPopupSurface *self, CustomShellSurface *transient_for)
{
    self->transient_for = transient_for;
}

