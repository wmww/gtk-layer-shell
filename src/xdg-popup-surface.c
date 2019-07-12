#include "xdg-popup-surface.h"

#include "custom-shell-surface.h"
#include "gtk-wayland.h"
#include "simple-conversions.h"

#include "xdg-shell-client.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>

struct _XdgPopupSurface
{
    CustomShellSurface super;

    XdgPopupPosition position;

    GdkRectangle cached_allocation;
    GdkRectangle geom;

    // These can be NULL
    struct xdg_surface *xdg_surface;
    struct xdg_popup *xdg_popup;
};

static void
xdg_surface_handle_configure (void *data,
                              struct xdg_surface *_xdg_surface,
                              uint32_t serial)
{
    XdgPopupSurface *self = data;
    (void)_xdg_surface;

    xdg_surface_ack_configure (self->xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

static void
xdg_popup_handle_configure (void *data,
                            struct xdg_popup *_xdg_popup,
                            int32_t _x,
                            int32_t _y,
                            int32_t width,
                            int32_t height)
{
    XdgPopupSurface *self = data;
    (void)_xdg_popup;
    (void)_x;
    (void)_y;

    g_return_if_fail(width >= 0 && height >= 0); // Protocol error

    // Technically this should not be applied until we get a xdg_surface.configure
    GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
    gtk_window_resize (gtk_window, width, height);
    GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (gtk_window));
    g_return_if_fail (gdk_window);
    // calculating the correct values is hard, but we're not required to provide them
    g_signal_emit_by_name (gdk_window, "moved-to-rect", NULL, NULL, FALSE, FALSE);
}

static void
xdg_popup_handle_popup_done (void *data,
                             struct xdg_popup *_xdg_popup)
{
    (void)_xdg_popup;

    XdgPopupSurface *self = data;
    GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
    gtk_widget_unmap (GTK_WIDGET (gtk_window));
}

static const struct xdg_popup_listener xdg_popup_listener = {
    .configure = xdg_popup_handle_configure,
    .popup_done = xdg_popup_handle_popup_done,
};

static void
xdg_popup_surface_get_anchor_rect (XdgPopupSurface *self, GdkRectangle *rect)
{
    // The anchor rect is given relative to the actual top-left of the parent GDK window surface
    // We need it realative to the logical geometry of the transient-for window, which may be sevel layers up
    *rect = self->position.rect;
    // It is a protocol error for size to be <= 0
    rect->width = MAX (rect->width, 1);
    rect->height = MAX (rect->height, 1);
    GdkWindow *parent_window = self->position.transient_for_gdk_window;
    CustomShellSurface *transient_for_shell_surface = self->position.transient_for_shell_surface;
    GtkWidget *transient_for_widget = GTK_WIDGET (custom_shell_surface_get_gtk_window (transient_for_shell_surface));
    GdkWindow *transient_for_window = gtk_widget_get_window (transient_for_widget);
    g_return_if_fail (parent_window);
    g_return_if_fail (transient_for_window);
    // Traverse up to the transient-for window adding each window's position relative to it's parent along the way
    while (parent_window && parent_window != transient_for_window) {
        gint x, y;
        gdk_window_get_position (parent_window, &x, &y);
        rect->x += x;
        rect->y += y;
        parent_window = gdk_window_get_effective_parent (parent_window);
    }
    if (parent_window != transient_for_window) {
        g_warning ("Could not find position of child window %p relative to parent window %p",
                   (void *)self->position.transient_for_gdk_window,
                   (void *)transient_for_window);
    }
    // Subtract the transient-for window's logical top-left
    GdkRectangle transient_for_geom =
        transient_for_shell_surface->virtual->get_logical_geom (transient_for_shell_surface);
    rect->x -= transient_for_geom.x;
    rect->y -= transient_for_geom.y;
}

static void
xdg_popup_surface_map (CustomShellSurface *super, struct wl_surface *wl_surface)
{
    XdgPopupSurface *self = (XdgPopupSurface *)super;

    g_return_if_fail (!self->xdg_popup);
    g_return_if_fail (!self->xdg_surface);

    GtkWindow *gtk_window = custom_shell_surface_get_gtk_window (super);
    GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (gtk_window));
    g_return_if_fail (gdk_window);
    GdkRectangle rect;
    xdg_popup_surface_get_anchor_rect (self, &rect);
    struct xdg_wm_base *xdg_wm_base_global = gtk_wayland_get_xdg_wm_base_global ();
    g_return_if_fail (xdg_wm_base_global);
    struct xdg_positioner *positioner = xdg_wm_base_create_positioner (xdg_wm_base_global);
    self->geom = gtk_wayland_get_logical_geom (gtk_window);
    enum xdg_positioner_anchor anchor = gdk_gravity_get_xdg_positioner_anchor(self->position.rect_anchor);
    enum xdg_positioner_gravity gravity = gdk_gravity_get_xdg_positioner_gravity(self->position.window_anchor);
    enum xdg_positioner_constraint_adjustment constraint_adjustment =
        gdk_anchor_hints_get_xdg_positioner_constraint_adjustment (self->position.anchor_hints);
    xdg_positioner_set_size (positioner, self->geom.width, self->geom.height);
    xdg_positioner_set_anchor_rect (positioner, rect.x, rect.y, rect.width, rect.height);
    xdg_positioner_set_offset (positioner, self->position.rect_anchor_d.x, self->position.rect_anchor_d.y);
    xdg_positioner_set_anchor (positioner, anchor);
    xdg_positioner_set_gravity (positioner, gravity);
    xdg_positioner_set_constraint_adjustment (positioner, constraint_adjustment);

    self->xdg_surface = xdg_wm_base_get_xdg_surface (xdg_wm_base_global, wl_surface);
    g_return_if_fail (self->xdg_surface);
    xdg_surface_add_listener (self->xdg_surface, &xdg_surface_listener, self);

    CustomShellSurface *transient_for_shell_surface = self->position.transient_for_shell_surface;
    self->xdg_popup = transient_for_shell_surface->virtual->get_popup (transient_for_shell_surface,
                                                                       self->xdg_surface,
                                                                       positioner);
    g_return_if_fail (self->xdg_popup);
    xdg_popup_add_listener (self->xdg_popup, &xdg_popup_listener, self);

    xdg_positioner_destroy (positioner);

    xdg_surface_set_window_geometry (self->xdg_surface,
                                     self->geom.x,
                                     self->geom.y,
                                     self->geom.width,
                                     self->geom.height);
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

static void
xdg_popup_surface_finalize (CustomShellSurface *super)
{
    xdg_popup_surface_unmap (super);
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

static GdkRectangle
xdg_popup_surface_get_logical_geom (CustomShellSurface *super)
{
    XdgPopupSurface *self = (XdgPopupSurface *)super;
    return self->geom;
}

static const CustomShellSurfaceVirtual xdg_popup_surface_virtual = {
    .map = xdg_popup_surface_map,
    .unmap = xdg_popup_surface_unmap,
    .finalize = xdg_popup_surface_finalize,
    .get_popup = xdg_popup_surface_get_popup,
    .get_logical_geom = xdg_popup_surface_get_logical_geom,
};

static void
xdg_popup_surface_on_size_allocate (GtkWidget *_widget,
                                    GdkRectangle *allocation,
                                    XdgPopupSurface *self)
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

XdgPopupSurface *
xdg_popup_surface_new (GtkWindow *gtk_window, XdgPopupPosition const* position)
{
    XdgPopupSurface *self = g_new0 (XdgPopupSurface, 1);
    g_assert (gtk_window);
    g_assert (position);
    self->super.virtual = &xdg_popup_surface_virtual;
    custom_shell_surface_init ((CustomShellSurface *)self, gtk_window);

    self->position = *position;
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

void
xdg_popup_surface_update_position (XdgPopupSurface *self, XdgPopupPosition const* position)
{
    self->position = *position;
    // Don't bother trying to remap. It's not needed and breaks shit
}

XdgPopupSurface *
custom_shell_surface_get_xdg_popup (CustomShellSurface *shell_surface)
{
    if (shell_surface && shell_surface->virtual == &xdg_popup_surface_virtual)
        return (XdgPopupSurface *)shell_surface;
    else
        return NULL;
}
