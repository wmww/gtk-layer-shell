#include "layer-surface.h"

#include "custom-shell-surface.h"
#include "gtk-wayland.h"

#include "protocol/wlr-layer-shell-unstable-v1-client.h"
#include "protocol/xdg-shell-client.h"

#include <gtk/gtk.h>

struct _LayerSurface
{
    CustomShellSurface super;

    // Can be set at any time
    uint32_t anchor;
    int exclusive_zone;
    GtkRequisition current_allocation; // Last size allocation, or (-1, -1) if there hasn't been one
    GtkRequisition cached_layer_size; // Last size sent to zwlr_layer_surface_v1_set_size, or (-1, -1) if never called

    // Need the surface to be recreated to change
    struct wl_output *output;
    enum zwlr_layer_shell_v1_layer layer;

    // The actual layer surface Wayland object (can be NULL)
    struct zwlr_layer_surface_v1 *layer_surface;
};

static GtkRequisition
layer_surface_get_gtk_window_size (LayerSurface *self)
{
    if (self->current_allocation.width >= 0 && self->current_allocation.height >= 0) {
        return self->current_allocation;
    } else {
        GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
        GtkRequisition natural_size;
        gtk_widget_get_preferred_size (GTK_WIDGET (gtk_window), NULL, &natural_size);
        return natural_size;
    }
}

static void
layer_surface_handle_configure (void *wayland_shell_surface,
                                struct zwlr_layer_surface_v1 *surface,
                                uint32_t serial,
                                uint32_t w,
                                uint32_t h)
{
    if (w > 0 || h > 0) {
        GtkRequisition requested = {
            .width = w,
            .height = h,
        };
        LayerSurface *self = wayland_shell_surface;
        GtkRequisition current_size = layer_surface_get_gtk_window_size (self);
        if (requested.width == 0)
            requested.width = current_size.width;
        if (requested.height == 0)
            requested.height = current_size.height;
        GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
        gtk_window_resize (gtk_window, requested.width, requested.height);
    }
    zwlr_layer_surface_v1_ack_configure (surface, serial);
}

static void
layer_surface_handle_closed (void *wayland_shell_surface,
                             struct zwlr_layer_surface_v1 *surface)
{
    // WaylandShellSurface *self = wayland_shell_surface;
    // TODO: close the GTK window and destroy the layer shell surface object
}

static const struct zwlr_layer_surface_v1_listener layer_surface_listener = {
    .configure = layer_surface_handle_configure,
    .closed = layer_surface_handle_closed,
};

static void
layer_surface_map (CustomShellSurface *super, struct wl_surface *wl_surface)
{
    LayerSurface *self = (LayerSurface *)super;

    g_return_if_fail (!self->layer_surface);

    struct zwlr_layer_shell_v1 *layer_shell_global = gtk_wayland_get_layer_shell_global ();
    g_return_if_fail (layer_shell_global);

    // name is either static or managed by the window widget
    const char *name = gtk_window_get_title (custom_shell_surface_get_gtk_window (super));
    if (name == NULL)
        name = "gtk-layer-shell";

    self->layer_surface = zwlr_layer_shell_v1_get_layer_surface (layer_shell_global,
                                                                 wl_surface,
                                                                 self->output,
                                                                 self->layer,
                                                                 name);
    g_return_if_fail (self->layer_surface);

    zwlr_layer_surface_v1_set_keyboard_interactivity (self->layer_surface, FALSE);
    zwlr_layer_surface_v1_set_anchor (self->layer_surface, self->anchor);
    zwlr_layer_surface_v1_set_exclusive_zone (self->layer_surface, self->exclusive_zone);
    if (self->cached_layer_size.width >= 0 && self->cached_layer_size.height >= 0) {
        zwlr_layer_surface_v1_set_size (self->layer_surface,
                                        self->cached_layer_size.width,
                                        self->cached_layer_size.height);
    }
    zwlr_layer_surface_v1_add_listener (self->layer_surface, &layer_surface_listener, self);
}

static void
layer_surface_unmap (CustomShellSurface *super)
{
    LayerSurface *self = (LayerSurface *)super;

    if (self->layer_surface) {
        zwlr_layer_surface_v1_destroy (self->layer_surface);
        self->layer_surface = NULL;
    }
}

static struct xdg_popup *
layer_surface_get_popup (CustomShellSurface *super,
                         struct xdg_surface *popup_xdg_surface,
                         struct xdg_positioner *positioner)
{
    LayerSurface *self = (LayerSurface *)super;

    if (!self->layer_surface) {
        g_critical ("layer_surface_get_popup () called when the layer surface wayland object has not yet been created");
        return NULL;
    }

    struct xdg_popup *xdg_popup = xdg_surface_get_popup (popup_xdg_surface, NULL, positioner);
    zwlr_layer_surface_v1_get_popup (self->layer_surface, xdg_popup);
    return xdg_popup;
}

static const CustomShellSurfaceVirtual layer_surface_virtual = {
    .map = layer_surface_map,
    .unmap = layer_surface_unmap,
    .finalize = layer_surface_unmap, // nothing but unmapping is needed to finalize
    .get_popup = layer_surface_get_popup,
};

static void
layer_surface_update_size (LayerSurface *self)
{
    GtkWindow *gtk_window = custom_shell_surface_get_gtk_window ((CustomShellSurface *)self);
    GtkRequisition request_size;
    gtk_widget_get_preferred_size (GTK_WIDGET (gtk_window), NULL, &request_size);

    if ((self->anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT) &&
        (self->anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT)) {

        request_size.width = 0;
    }
    if ((self->anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP) &&
        (self->anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM)) {

        request_size.height = 0;
    }

    GtkRequisition window_default_size;
    gtk_window_get_default_size (gtk_window,
                                 &window_default_size.width,
                                 &window_default_size.height);
    if (window_default_size.width >= 0)
        request_size.width = window_default_size.width;
    if (window_default_size.height >= 0)
        request_size.height = window_default_size.height;

    if (request_size.width != self->cached_layer_size.width ||
        request_size.height != self->cached_layer_size.height) {

        self->cached_layer_size = request_size;
        if (self->layer_surface) {
            zwlr_layer_surface_v1_set_size (self->layer_surface,
                                            self->cached_layer_size.width,
                                            self->cached_layer_size.height);
        }
    }
}

static void
layer_surface_on_size_allocate (GtkWidget *gtk_window,
                                GdkRectangle *allocation,
                                LayerSurface *self)
{
    if (self->current_allocation.width != allocation->width ||
        self->current_allocation.height != allocation->height) {

        self->current_allocation.width = allocation->width;
        self->current_allocation.height = allocation->height;

        layer_surface_update_size (self);
    }
}

LayerSurface *
layer_surface_new (GtkWindow *gtk_window)
{
    LayerSurface *self = g_new0 (LayerSurface, 1);
    self->super.virtual = &layer_surface_virtual;
    custom_shell_surface_init ((CustomShellSurface *)self, gtk_window);

    self->current_allocation = (GtkRequisition) {
        .width = -1,
        .height = -1,
    };
    self->cached_layer_size = self->current_allocation;
    self->output = NULL;
    self->layer = ZWLR_LAYER_SHELL_V1_LAYER_TOP;
    self->anchor = 0;
    self->exclusive_zone = 0;
    self->layer_surface = NULL;

    gtk_window_set_decorated (gtk_window, FALSE);
    g_signal_connect (gtk_window, "size-allocate", G_CALLBACK (layer_surface_on_size_allocate), self);

    return self;
}

LayerSurface *
custom_shell_surface_get_layer_surface (CustomShellSurface *shell_surface)
{
    if (shell_surface && shell_surface->virtual == &layer_surface_virtual)
        return (LayerSurface *)shell_surface;
    else
        return NULL;
}

void
layer_surface_set_layer (LayerSurface *self, enum zwlr_layer_shell_v1_layer layer)
{
    if (self->layer != layer) {
        self->layer = layer;
        if (self->layer_surface) {
            custom_shell_surface_remap ((CustomShellSurface *)self);
        }
    }
}

uint32_t
layer_surface_get_anchor (LayerSurface *self)
{
    return self->anchor;
}

void
layer_surface_set_anchor (LayerSurface *self, uint32_t anchor)
{
    if (self->anchor != anchor) {
        self->anchor = anchor;
        if (self->layer_surface) {
            zwlr_layer_surface_v1_set_anchor (self->layer_surface, self->anchor);
            layer_surface_update_size (self);
            custom_shell_surface_needs_commit ((CustomShellSurface *)self);
        }
    }
}

void
layer_surface_set_exclusive_zone (LayerSurface *self, int exclusive_zone)
{
    if (self->exclusive_zone != exclusive_zone) {
        self->exclusive_zone = exclusive_zone;
        if (self->layer_surface) {
            zwlr_layer_surface_v1_set_exclusive_zone (self->layer_surface, self->exclusive_zone);
            custom_shell_surface_needs_commit ((CustomShellSurface *)self);
        }
    }
}
