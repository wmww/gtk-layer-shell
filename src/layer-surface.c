#include "layer-surface.h"

#include "custom-shell-surface.h"
#include "gtk-wayland.h"

#include "protocol/wlr-layer-shell-unstable-v1-client.h"
#include "protocol/xdg-shell-client.h"

#include <gtk/gtk.h>

struct _LayerSurface
{
    CustomShellSurface parent;

    // Set by GTK
    int cached_width, cached_height;

    // Can be set at any time
    uint32_t anchor;
    int exclusive_zone;

    // Need the surface to be recreated to change
    struct wl_output *output;
    enum zwlr_layer_shell_v1_layer layer;
    const char *_namespace;

    // The actual layer surface Wayland object (can be NULL)
    struct zwlr_layer_surface_v1 *layer_surface;
};

typedef struct _LayerSurfaceInfo LayerSurfaceInfo;

static void
layer_surface_handle_configure (void *wayland_shell_surface,
                struct zwlr_layer_surface_v1 *surface,
                uint32_t serial,
                uint32_t w,
                uint32_t h)
{
    // WaylandShellSurface *self = wayland_shell_surface;
    // TODO: resize the GTK window
    // gtk_window_set_default_size (GTK_WINDOW (window), width, height);
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
layer_shell_surface_realize (CustomShellSurface *super, struct wl_surface *wl_surface)
{
    LayerSurface *self = (LayerSurface *)super;

    struct zwlr_layer_shell_v1 *layer_shell_global = gtk_wayland_get_layer_shell_global ();
    g_return_if_fail (layer_shell_global);

    self->layer_surface = zwlr_layer_shell_v1_get_layer_surface (layer_shell_global,
                                                                 wl_surface,
                                                                 self->output,
                                                                 self->layer,
                                                                 self->_namespace);
    g_return_if_fail (self->layer_surface);

    zwlr_layer_surface_v1_set_keyboard_interactivity (self->layer_surface, FALSE);
    zwlr_layer_surface_v1_set_anchor (self->layer_surface, self->anchor);
    zwlr_layer_surface_v1_add_listener (self->layer_surface, &layer_surface_listener, self);
    if (self->cached_width > -1 && self->cached_height > -1)
        zwlr_layer_surface_v1_set_size (self->layer_surface, self->cached_width, self->cached_height);
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
}

static const CustomShellSurfaceVirtual layer_surface_virtual = {
    .realize = layer_shell_surface_realize,
    .unmap = layer_surface_unmap,
    .finalize = layer_surface_unmap, // nothing but unmapping is needed to finalize
    .get_popup = layer_surface_get_popup,
};

static void
layer_surface_on_size_allocate (GtkWidget *gtk_window,
                                GdkRectangle *allocation,
                                LayerSurface *self)
{
    if (self->layer_surface && (self->cached_width != allocation->width ||
                                self->cached_height != allocation->height)) {
        zwlr_layer_surface_v1_set_size (self->layer_surface, allocation->width, allocation->height);
    }

    self->cached_width = allocation->width;
    self->cached_height = allocation->height;
}

LayerSurface *
layer_surface_new (GtkWindow *gtk_window)
{
    LayerSurface *self = g_new0 (LayerSurface, 1);
    self->parent.virtual = &layer_surface_virtual;
    custom_shell_surface_init ((CustomShellSurface*)self, gtk_window);

    self->cached_width = -1;
    self->cached_height = -1;
    self->output = NULL;
    self->layer = ZWLR_LAYER_SHELL_V1_LAYER_TOP;
    self->_namespace = "todo-chage-me";
    self->anchor = ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT;
    self->exclusive_zone = 0;
    self->layer_surface = NULL;

    g_signal_connect (gtk_window, "size-allocate", G_CALLBACK (layer_surface_on_size_allocate), self);

    return self;
}
