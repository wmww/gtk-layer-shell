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

#include "layer-surface.h"

#include "gtk-layer-shell.h"
#include "wayland-utils.h"
#include "libwayland-wrappers.h"

#include "wlr-layer-shell-unstable-v1-client.h"
#include "xdg-shell-client.h"

#include <gtk/gtk.h>
#include <gdk/wayland/gdkwayland.h>

LayerSurface *pending_layer_surface = NULL;


static const char *layer_surface_key = "wayland_layer_surface";

LayerSurface *
gtk_window_get_layer_surface (GtkWindow *gtk_window)
{
    if (!gtk_window)
        return NULL;

    return g_object_get_data (G_OBJECT (gtk_window), layer_surface_key);
}

void
layer_surface_needs_commit (LayerSurface *self)
{
    if (!self->gtk_window)
        return;

    // GdkSurface *gdk_surface = gtk_native_get_surface (GTK_NATIVE (self->gtk_window));

    // if (!gdk_surface)
    //    return;

    // Hopefully this will trigger a commit
    // Don't commit directly, as that screws up GTK's internal state
    // (see https://github.com/wmww/gtk-layer-shell/issues/51)
    // TODO
    // gdk_window_invalidate_rect (gdk_window, NULL, FALSE);

    gtk_widget_queue_draw (GTK_WIDGET (self->gtk_window));
}

void
layer_surface_remap (LayerSurface *self)
{
    GtkWidget *window_widget = GTK_WIDGET (self->gtk_window);
    g_return_if_fail (window_widget);
    gtk_widget_set_visible (window_widget, FALSE);
    gtk_widget_set_visible (window_widget, TRUE);
}

/*
 * Sends the .set_size request if the current allocation differs from the last size sent
 * Needs to be called whenever current_allocation or anchors are changed
 * If .set_size is sent, it should trigger the compositor to send a .configure event
 */
static void
layer_surface_send_set_size (LayerSurface *self)
{
    GtkRequisition request_size = self->current_allocation;

    if ((self->anchors[GTK_LAYER_SHELL_EDGE_LEFT]) &&
        (self->anchors[GTK_LAYER_SHELL_EDGE_RIGHT])) {

        request_size.width = 0;
    }

    if ((self->anchors[GTK_LAYER_SHELL_EDGE_TOP]) &&
        (self->anchors[GTK_LAYER_SHELL_EDGE_BOTTOM])) {

        request_size.height = 0;
    }

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

/*
 * Sets the window's geometry hints (used to force the window to be a specific size)
 * Needs to be called whenever last_configure_size or anchors are changed
 * Lets windows decide their own size along any axis the surface is not stretched along
 * Forces window (by setting the max and min hints) to be of configured size along axes they are stretched along
 */
static void
layer_surface_update_size (LayerSurface *self)
{
    gint width = -1;
    gint height = -1;

    if ((self->anchors[GTK_LAYER_SHELL_EDGE_LEFT]) &&
        (self->anchors[GTK_LAYER_SHELL_EDGE_RIGHT])) {

        width = self->last_configure_size.width;
    }
    if ((self->anchors[GTK_LAYER_SHELL_EDGE_TOP]) &&
        (self->anchors[GTK_LAYER_SHELL_EDGE_BOTTOM])) {

        height = self->last_configure_size.height;
    }

    /*
    GdkGeometry hints;
    hints.min_width = width;
    hints.max_width = width;
    hints.min_height = height;
    hints.max_height = height;

    gtk_window_set_geometry_hints (gtk_window,
                                   NULL,
                                   &hints,
                                   GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE);
    */

    // This will usually get called in a moment by the layer_surface_on_size_allocate () triggered by the above
    // gtk_window_set_geometry_hints (). However in some cases (such as a streatching a window after a size request has
    // been set), an allocate will not be triggered but the set size does need to change. For this reason we make the
    // call here as well and let the later call clean up any mistakes this one makes. This makes the flicker problem
    // worse, but I think it's more important that the end result is correct.
    layer_surface_send_set_size (self);
}

static void
layer_surface_handle_configure (void *data,
                                struct zwlr_layer_surface_v1 *surface,
                                uint32_t serial,
                                uint32_t w,
                                uint32_t h)
{
    LayerSurface *self = data;

    if (self->client_facing_xdg_surface && self->client_facing_xdg_toplevel) {
        struct wl_array states;
        wl_array_init(&states);
        {
            uint32_t *state = wl_array_add(&states, sizeof(uint32_t));
            g_assert(state);
            *state = XDG_TOPLEVEL_STATE_ACTIVATED;
        }
        {
            uint32_t *state = wl_array_add(&states, sizeof(uint32_t));
            g_assert(state);
            *state = XDG_TOPLEVEL_STATE_MAXIMIZED;
        }
        DISPATCH_CLIENT_FACING_EVENT(
            xdg_toplevel_listener,
            self->client_facing_xdg_toplevel,
            configure,
            self->client_facing_xdg_toplevel,
            w, h,
            &states);
        wl_array_release(&states);

        DISPATCH_CLIENT_FACING_EVENT(
            xdg_surface_listener,
            self->client_facing_xdg_surface,
            configure,
            self->client_facing_xdg_surface,
            serial);
    } else {
        g_warning(MESSAGE_PREFIX "no XDG surface to configure");
    }

    zwlr_layer_surface_v1_ack_configure (surface, serial);

    self->last_configure_size = (GtkRequisition) {
        .width = (gint)w,
        .height = (gint)h,
    };

    layer_surface_update_size (self);
}

static void
layer_surface_handle_closed (void *data,
                             struct zwlr_layer_surface_v1 *_surface)
{
    LayerSurface *self = data;
    (void)_surface;

    gtk_window_close (self->gtk_window);
}

static const struct zwlr_layer_surface_v1_listener layer_surface_listener = {
    .configure = layer_surface_handle_configure,
    .closed = layer_surface_handle_closed,
};

static void
layer_surface_send_set_anchor (LayerSurface *self)
{
    if (self->layer_surface) {
        uint32_t wlr_anchor = gtk_layer_shell_edge_array_get_zwlr_layer_shell_v1_anchor (self->anchors);
        zwlr_layer_surface_v1_set_anchor (self->layer_surface, wlr_anchor);
    }
}

static void
layer_surface_send_set_margin (LayerSurface *self)
{
    if (self->layer_surface) {
        zwlr_layer_surface_v1_set_margin (self->layer_surface,
                                          self->margins[GTK_LAYER_SHELL_EDGE_TOP],
                                          self->margins[GTK_LAYER_SHELL_EDGE_RIGHT],
                                          self->margins[GTK_LAYER_SHELL_EDGE_BOTTOM],
                                          self->margins[GTK_LAYER_SHELL_EDGE_LEFT]);
    }
}

static void
layer_surface_on_window_realize (GtkWidget *widget, LayerSurface *self)
{
    g_return_if_fail (GTK_WIDGET (self->gtk_window) == widget);
    g_return_if_fail (!self->layer_surface);

    GdkSurface *gdk_surface = gtk_native_get_surface (GTK_NATIVE (self->gtk_window));
    g_return_if_fail (gdk_surface);

    self->wl_surface = gdk_wayland_surface_get_wl_surface (gdk_surface);
    g_return_if_fail(self->wl_surface);

    pending_layer_surface = self;
}

static void
layer_surface_create_surface_object (LayerSurface *self)
{
    pending_layer_surface = NULL;

    struct zwlr_layer_shell_v1 *layer_shell_global = gtk_wayland_get_layer_shell_global ();
    g_return_if_fail (layer_shell_global);

    const char *name_space = layer_surface_get_namespace(self);

    struct wl_output *output = NULL;
    if (self->monitor) {
        output = gdk_wayland_monitor_get_wl_output (self->monitor);
    }

    enum zwlr_layer_shell_v1_layer layer = gtk_layer_shell_layer_get_zwlr_layer_shell_v1_layer(self->layer);
    self->layer_surface = zwlr_layer_shell_v1_get_layer_surface (layer_shell_global,
                                                                 self->wl_surface,
                                                                 output,
                                                                 layer,
                                                                 name_space);
    g_return_if_fail (self->layer_surface);

    zwlr_layer_surface_v1_set_keyboard_interactivity (self->layer_surface, self->keyboard_mode);
    zwlr_layer_surface_v1_set_exclusive_zone (self->layer_surface, self->exclusive_zone);
    layer_surface_send_set_anchor (self);
    layer_surface_send_set_margin (self);
    zwlr_layer_surface_v1_set_size (self->layer_surface, 300, 300);
    /*
    if (self->cached_layer_size.width >= 0 && self->cached_layer_size.height >= 0) {
        zwlr_layer_surface_v1_set_size (self->layer_surface,
                                        self->cached_layer_size.width,
                                        self->cached_layer_size.height);
    }
    */
    zwlr_layer_surface_v1_add_listener (self->layer_surface, &layer_surface_listener, self);
}

static void
layer_surface_unmap (LayerSurface *super)
{
    LayerSurface *self = (LayerSurface *)super;

    if (self->layer_surface) {
        zwlr_layer_surface_v1_destroy (self->layer_surface);
        self->layer_surface = NULL;
    }

    clear_client_facing_proxy_data((struct wl_proxy *)self->client_facing_xdg_surface);
    clear_client_facing_proxy_data((struct wl_proxy *)self->client_facing_xdg_toplevel);
}

static void
layer_surface_destroy (LayerSurface *self)
{
    layer_surface_unmap (self);
    g_free ((gpointer)self->name_space);
    g_free (self);

}

static void
layer_surface_update_auto_exclusive_zone (LayerSurface *self)
{
    if (!self->auto_exclusive_zone)
        return;

    gboolean horiz = (self->anchors[GTK_LAYER_SHELL_EDGE_LEFT] ==
                      self->anchors[GTK_LAYER_SHELL_EDGE_RIGHT]);
    gboolean vert = (self->anchors[GTK_LAYER_SHELL_EDGE_TOP] ==
                     self->anchors[GTK_LAYER_SHELL_EDGE_BOTTOM]);
    int new_exclusive_zone = -1;

    if (horiz && !vert) {
        new_exclusive_zone = self->current_allocation.height;
        if (!self->anchors[GTK_LAYER_SHELL_EDGE_TOP])
            new_exclusive_zone += self->margins[GTK_LAYER_SHELL_EDGE_TOP];
        if (!self->anchors[GTK_LAYER_SHELL_EDGE_BOTTOM])
            new_exclusive_zone += self->margins[GTK_LAYER_SHELL_EDGE_BOTTOM];
    } else if (vert && !horiz) {
        new_exclusive_zone = self->current_allocation.width;
        if (!self->anchors[GTK_LAYER_SHELL_EDGE_LEFT])
            new_exclusive_zone += self->margins[GTK_LAYER_SHELL_EDGE_LEFT];
        if (!self->anchors[GTK_LAYER_SHELL_EDGE_RIGHT])
            new_exclusive_zone += self->margins[GTK_LAYER_SHELL_EDGE_RIGHT];
    }

    if (new_exclusive_zone >= 0 && self->exclusive_zone != new_exclusive_zone) {
        self->exclusive_zone = new_exclusive_zone;
        if (self->layer_surface) {
            zwlr_layer_surface_v1_set_exclusive_zone (self->layer_surface, self->exclusive_zone);
        }
    }
}

static void
layer_surface_on_size_allocate (GtkWidget *_gtk_window,
                                GdkRectangle *allocation,
                                LayerSurface *self)
{
    (void)_gtk_window;

    if (self->current_allocation.width != allocation->width ||
        self->current_allocation.height != allocation->height) {

        self->current_allocation = (GtkRequisition) {
            .width = allocation->width,
            .height = allocation->height,
        };

        layer_surface_send_set_size (self);
        layer_surface_update_auto_exclusive_zone (self);
    }
}

LayerSurface *
layer_surface_new (GtkWindow *gtk_window)
{
    g_return_val_if_fail (gtk_wayland_get_layer_shell_global (), NULL);
    g_return_val_if_fail (gtk_window, NULL);
    g_return_val_if_fail (!gtk_widget_get_mapped (GTK_WIDGET (gtk_window)), NULL);

    LayerSurface *self = g_new0 (LayerSurface, 1);

    self->gtk_window = gtk_window;

    g_object_set_data_full (G_OBJECT (gtk_window),
                            layer_surface_key,
                            self,
                            (GDestroyNotify) layer_surface_destroy);
    g_signal_connect (gtk_window, "realize", G_CALLBACK (layer_surface_on_window_realize), self);

    if (gtk_widget_get_realized (GTK_WIDGET (gtk_window))) {
        // We must be in the process of realizing now
        layer_surface_on_window_realize (GTK_WIDGET (gtk_window), self);
    }

    self->current_allocation = (GtkRequisition) {
        .width = 0,
        .height = 0,
    };
    self->cached_layer_size = self->current_allocation;
    self->last_configure_size = self->current_allocation;
    self->monitor = NULL;
    self->layer = GTK_LAYER_SHELL_LAYER_TOP;
    self->name_space = NULL;
    self->exclusive_zone = 0;
    self->auto_exclusive_zone = FALSE;
    self->keyboard_mode = GTK_LAYER_SHELL_KEYBOARD_MODE_NONE;
    self->layer_surface = NULL;

    gtk_window_set_decorated (gtk_window, FALSE);
    g_signal_connect (gtk_window, "size-allocate", G_CALLBACK (layer_surface_on_size_allocate), self);

    return self;
}

void
layer_surface_set_monitor (LayerSurface *self, GdkMonitor *monitor)
{
    if (monitor) g_return_if_fail (GDK_IS_WAYLAND_MONITOR (monitor));
    if (monitor != self->monitor) {
        self->monitor = monitor;
        if (self->layer_surface) {
            layer_surface_remap (self);
        }
    }
}

void
layer_surface_set_name_space (LayerSurface *self, char const* name_space)
{
    if (g_strcmp0(self->name_space, name_space) != 0) {
        g_free ((gpointer)self->name_space);
        self->name_space = g_strdup (name_space);
        if (self->layer_surface) {
            layer_surface_remap (self);
        }
    }
}

void
layer_surface_set_layer (LayerSurface *self, GtkLayerShellLayer layer)
{
    if (self->layer != layer) {
        self->layer = layer;
        if (self->layer_surface) {
            uint32_t version = zwlr_layer_surface_v1_get_version (self->layer_surface);
            if (version >= ZWLR_LAYER_SURFACE_V1_SET_LAYER_SINCE_VERSION) {
                enum zwlr_layer_shell_v1_layer wlr_layer = gtk_layer_shell_layer_get_zwlr_layer_shell_v1_layer(layer);
                zwlr_layer_surface_v1_set_layer (self->layer_surface, wlr_layer);
                layer_surface_needs_commit (self);
            } else {
                layer_surface_remap (self);
            }
        }
    }
}

void
layer_surface_set_anchor (LayerSurface *self, GtkLayerShellEdge edge, gboolean anchor_to_edge)
{
    g_return_if_fail (edge >= 0 && edge < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER);
    anchor_to_edge = (anchor_to_edge != FALSE);
    if (anchor_to_edge != self->anchors[edge]) {
        self->anchors[edge] = anchor_to_edge;
        if (self->layer_surface) {
            layer_surface_send_set_anchor (self);
            layer_surface_update_size (self);
            layer_surface_update_auto_exclusive_zone (self);
            layer_surface_needs_commit (self);
        }
    }
}

void
layer_surface_set_margin (LayerSurface *self, GtkLayerShellEdge edge, int margin_size)
{
    g_return_if_fail (edge >= 0 && edge < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER);
    if (margin_size != self->margins[edge]) {
        self->margins[edge] = margin_size;
        layer_surface_send_set_margin (self);
        layer_surface_update_auto_exclusive_zone (self);
        layer_surface_needs_commit (self);
    }
}

void
layer_surface_set_exclusive_zone (LayerSurface *self, int exclusive_zone)
{
    self->auto_exclusive_zone = FALSE;
    if (exclusive_zone < -1)
        exclusive_zone = -1;
    if (self->exclusive_zone != exclusive_zone) {
        self->exclusive_zone = exclusive_zone;
        if (self->layer_surface) {
            zwlr_layer_surface_v1_set_exclusive_zone (self->layer_surface, self->exclusive_zone);
            layer_surface_needs_commit (self);
        }
    }
}

void
layer_surface_auto_exclusive_zone_enable (LayerSurface *self)
{
    if (!self->auto_exclusive_zone) {
        self->auto_exclusive_zone = TRUE;
        layer_surface_update_auto_exclusive_zone (self);
    }
}

void
layer_surface_set_keyboard_mode (LayerSurface *self, GtkLayerShellKeyboardMode mode)
{
    if (mode == GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND) {
        uint32_t version = gtk_layer_get_protocol_version();
        if (version <= 3) {
            g_warning (
                "Compositor uses layer shell version %d, which does not support on-demand keyboard interactivity",
                version);
            mode = GTK_LAYER_SHELL_KEYBOARD_MODE_NONE;
        }
    }
    if (self->keyboard_mode != mode) {
        self->keyboard_mode = mode;
        if (self->layer_surface) {
            zwlr_layer_surface_v1_set_keyboard_interactivity (self->layer_surface, self->keyboard_mode);
            layer_surface_needs_commit (self);
        }
    }
}

const char*
layer_surface_get_namespace (LayerSurface *self)
{
    if (self && self->name_space)
        return self->name_space;
    else
        return "gtk-layer-shell";
}

static struct wl_proxy *
stubbed_xdg_toplevel_handle_request (
    void* data,
    struct wl_proxy *proxy,
    uint32_t opcode,
    const struct wl_interface *interface,
    uint32_t version,
    uint32_t flags,
    union wl_argument *args)
{
    // TODO
    return NULL;
}

static struct wl_proxy *
stubbed_xdg_surface_handle_request (
    void* data,
    struct wl_proxy *proxy,
    uint32_t opcode,
    const struct wl_interface *interface,
    uint32_t version,
    uint32_t flags,
    union wl_argument *args)
{
    LayerSurface *self = (LayerSurface *)data;
    if (opcode == XDG_SURFACE_GET_TOPLEVEL) {
        struct wl_proxy *toplevel = create_client_facing_proxy (
            proxy,
            &xdg_toplevel_interface,
            version,
            stubbed_xdg_toplevel_handle_request,
            NULL,
            data);
        self->client_facing_xdg_toplevel = (struct xdg_toplevel *)toplevel;
        return toplevel;
    } else if (opcode == XDG_SURFACE_GET_POPUP) {
        g_error(MESSAGE_PREFIX "internal error: XDG surface intercepted, but is now being used as popup");
        return create_client_facing_proxy (proxy, &xdg_popup_interface, version, NULL, NULL, NULL);
    } else {
        return NULL;
    }
}

static void
stubbed_xdg_surface_handle_destroy (void* data, struct wl_proxy *proxy)
{
    LayerSurface *self = (LayerSurface *)data;
    layer_surface_unmap(self);
}

struct wl_proxy *
layer_surface_handle_request (
    struct wl_proxy *proxy,
    uint32_t opcode,
    const struct wl_interface *interface,
    uint32_t version,
    uint32_t flags,
    union wl_argument *args)
{
    const char* type = proxy->object.interface->name;
    if (strcmp(type, xdg_wm_base_interface.name) == 0) {
        if (opcode == XDG_WM_BASE_GET_XDG_SURFACE) {
            if (pending_layer_surface && pending_layer_surface->wl_surface == (struct wl_surface *)args[1].o) {
                struct wl_proxy *xdg_surface = create_client_facing_proxy (
                    proxy,
                    &xdg_surface_interface,
                    version,
                    stubbed_xdg_surface_handle_request,
                    stubbed_xdg_surface_handle_destroy,
                    pending_layer_surface);
                pending_layer_surface->client_facing_xdg_surface = (struct xdg_surface *)xdg_surface;
                layer_surface_create_surface_object(pending_layer_surface);
                return xdg_surface;
            }
        }
    } else if (strcmp(type, xdg_surface_interface.name) == 0) {
        if (opcode == XDG_SURFACE_GET_POPUP) {
            LayerSurface *self = get_client_facing_proxy_data ((struct wl_proxy *)args[1].o, stubbed_xdg_surface_handle_request);
            if (self) {
                if (self->layer_surface) {
                    struct xdg_popup *xdg_popup = xdg_surface_get_popup (
                        (struct xdg_surface *)proxy,
                        NULL,
                        (struct xdg_positioner *)args[2].o);
                    zwlr_layer_surface_v1_get_popup (self->layer_surface, xdg_popup);
                    return (struct wl_proxy *)xdg_popup;
                } else {
                    g_error (MESSAGE_PREFIX "tried to create popup before layer shell surface");
                    return create_client_facing_proxy (proxy, &xdg_popup_interface, version, NULL, NULL, NULL);
                }
            }
        }
    }
    return real_wl_proxy_marshal_array_flags (proxy, opcode, interface, version, flags, args);
}
