#include "gtk-layer-shell.h"
#include "gdk-window-hack.h"
#include "protocol/xdg-shell-client.h"
#include "protocol/wlr-layer-shell-unstable-v1-client.h"

struct xdg_wm_base *xdg_wm_base_global = NULL;
struct zwlr_layer_shell_v1 *layer_shell_global = NULL;

static gboolean has_initialized = FALSE;

// The last widget to get the query-tooltip callback
// Used as the parent of new tooltips
static GtkWidget *last_query_tooltip_widget = NULL;

const char *wayland_shell_surface_key = "wayland_shell_surface";

static void (*wayland_shell_surface_popup_callback)(WaylandShellSurface *self) = NULL;

gboolean
is_using_wayland ()
{
    return GDK_IS_WAYLAND_DISPLAY (gdk_display_get_default ());
}

struct zwlr_layer_shell_v1 *
get_layer_shell_global ()
{
    return layer_shell_global;
}

struct xdg_wm_base *
get_xdg_wm_base_global ()
{
    return xdg_wm_base_global;
}

gboolean gtk_layer_shell_has_initialized ()
{
    return has_initialized;
}

// Gets the upper left and size of the portion of the window that is actually used (not shadows and whatnot)
// It does this by walking down the gdk_window tree, as long as there is exactly one child
static void
wayland_widget_get_logical_geom (GtkWidget *widget, GdkRectangle *geom)
{
    GdkWindow *window;
    GList *list;

    window = gtk_widget_get_window (widget);
    list = gdk_window_get_children (window);
    if (list && !list->next) // If there is exactly one child window
        window = list->data;
    gdk_window_get_geometry (window, &geom->x, &geom->y, &geom->width, &geom->height);
}

WaylandShellSurface *
gtk_widget_get_wayland_shell_surface (GtkWidget *widget)
{
    GdkWindow *gdk_window;

    g_return_val_if_fail (widget, NULL);
    gdk_window = gtk_widget_get_window (gtk_widget_get_toplevel (widget));
    if (!gdk_window)
        return NULL;
    return g_object_get_data (G_OBJECT (gdk_window), wayland_shell_surface_key);
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
    // who cares
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = wl_registry_handle_global,
    .global_remove = wl_registry_handle_global_remove,
};


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

struct zwlr_layer_surface_v1_listener layer_surface_listener = {
    .configure = layer_surface_handle_configure,
    .closed = layer_surface_handle_closed,
};

static void
xdg_surface_handle_configure (void *wayland_shell_surface,
                  struct xdg_surface *xdg_surface,
                  uint32_t serial)

{
    // WaylandShellSurface *self = wayland_shell_surface;
    xdg_surface_ack_configure (xdg_surface, serial);
}

struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

static void
xdg_popup_handle_configure (void *wayland_shell_surface,
                struct xdg_popup *xdg_popup,
                int32_t x,
                int32_t y,
                int32_t width,
                int32_t height)
{
    if (width > 0 && height > 0) {
        WaylandShellSurface *self = wayland_shell_surface;
        gtk_window_resize (self->gtk_window, width, height);
    }
}

static void
xdg_popup_handle_popup_done (void *wayland_shell_surface,
                 struct xdg_popup *xdg_popup)
{
    WaylandShellSurface *self = wayland_shell_surface;
    gtk_widget_unmap (GTK_WIDGET (self->gtk_window));
}

static const struct xdg_popup_listener xdg_popup_listener = {
    .configure = xdg_popup_handle_configure,
    .popup_done = xdg_popup_handle_popup_done,
};

void
wayland_shell_surface_unmap (WaylandShellSurface *self)
{
    // This function must be called before the wl_surface can be safely destroyed

    self->width = 0;
    self->height = 0;

    if (self->xdg_popup) {
        xdg_popup_destroy (self->xdg_popup);
        self->xdg_popup = NULL;
    }
    if (self->xdg_toplevel) {
        xdg_toplevel_destroy (self->xdg_toplevel);
        self->xdg_toplevel = NULL;
    }
    if (self->xdg_surface) {
        // Important that XDG surfaces are destroyed after their role (popup or toplevel)
        xdg_surface_destroy (self->xdg_surface);
        self->xdg_surface = NULL;
    }

    if (self->layer_surface) {
        zwlr_layer_surface_v1_destroy (self->layer_surface);
        self->layer_surface = NULL;
    }
    if (self->layer_surface_info) {
        g_free (self->layer_surface_info);
        self->layer_surface_info = NULL;
    }
}

// Should only be used in wayland_shell_surface_new as the destroy listener
// Other usages may cause a double free
static void
wayland_shell_surface_destroy_cb (WaylandShellSurface *self) {
    wayland_shell_surface_unmap (self);
    free (self);
}

static void
wayland_shell_surface_set_size (WaylandShellSurface *self, gint width, gint height)
{
    //if (self->width != width || self->height != height) {
        self->width  = width;
        self->height = height;
        if (self->layer_surface)
            zwlr_layer_surface_v1_set_size (self->layer_surface, self->width, self->height);
        if (self->xdg_surface) {
            GdkRectangle geom;
            wayland_widget_get_logical_geom (GTK_WIDGET (self->gtk_window), &geom);
            xdg_surface_set_window_geometry (self->xdg_surface, geom.x, geom.y, geom.width, geom.height);
        }
    //}
}

static void
wayland_widget_size_allocate_cb (GtkWidget           *widget,
                 GdkRectangle        *allocation,
                 WaylandShellSurface *shell_surface)
{
    wayland_shell_surface_set_size (shell_surface,
                    allocation->width,
                    allocation->height);
}

static WaylandShellSurface *
wayland_shell_surface_new (GtkWindow *gtk_window)
{
    WaylandShellSurface *self;
    GdkWindow *gdk_window;

    if (!has_initialized)
        wayland_shell_surface_global_init ();

    g_return_val_if_fail (has_initialized, NULL);
    g_return_val_if_fail (gtk_window, NULL);

    // Make sure there isn't already a Wayland shell surface for this windows
    g_return_val_if_fail (!gtk_widget_get_wayland_shell_surface (GTK_WIDGET (gtk_window)), NULL);

    gdk_window = gtk_widget_get_window (gtk_widget_get_toplevel (GTK_WIDGET (gtk_window)));
    g_return_val_if_fail (gdk_window, NULL);

    self = g_new0 (WaylandShellSurface, 1);
    self->gtk_window = gtk_window;

    gdk_wayland_window_set_use_custom_surface (gdk_window);
    g_object_set_data_full (G_OBJECT (gdk_window),
                wayland_shell_surface_key,
                self,
                (GDestroyNotify) wayland_shell_surface_destroy_cb);
    g_signal_connect (gtk_window, "size-allocate", G_CALLBACK (wayland_widget_size_allocate_cb), self);

    return self;
}

WaylandShellSurface *
wayland_shell_surface_new_layer_surface (GtkWindow *gtk_window, struct wl_output *output, uint32_t layer, const char *name_space)
{
    WaylandShellSurface *self;
    GdkWindow *gdk_window;
    struct wl_surface *wl_surface;

    gtk_window_set_decorated (gtk_window, FALSE);
    self = wayland_shell_surface_new (gtk_window);
    g_return_val_if_fail (self, NULL);

    gdk_window = gtk_widget_get_window (GTK_WIDGET (gtk_window));
    g_return_val_if_fail (gdk_window, NULL);

    wl_surface = gdk_wayland_window_get_wl_surface (gdk_window);
    g_return_val_if_fail (wl_surface, NULL);

    if (layer_shell_global) {
        self->layer_surface = zwlr_layer_shell_v1_get_layer_surface (layer_shell_global,
                                         wl_surface,
                                         output,
                                         layer,
                                         name_space);
        g_return_val_if_fail (self->layer_surface, NULL);
        zwlr_layer_surface_v1_set_keyboard_interactivity (self->layer_surface, FALSE);
        zwlr_layer_surface_v1_add_listener (self->layer_surface, &layer_surface_listener, self);
    } else if (xdg_wm_base_global) {
        g_warning ("Layer Shell Wayland protocol not supported, panel will not be placed correctly");
        self->xdg_surface = xdg_wm_base_get_xdg_surface (xdg_wm_base_global, wl_surface);
        self->xdg_toplevel = xdg_surface_get_toplevel (self->xdg_surface);
        xdg_surface_add_listener (self->xdg_surface, &xdg_surface_listener, self);
    } else {
        g_warning ("Neither Layer Shell or XDG shell stable Wayland protocols detected, panel can not be drawn");
        return NULL;
    }
    wayland_shell_surface_set_size (self,
                    gtk_widget_get_allocated_width (GTK_WIDGET (gtk_window)),
                    gtk_widget_get_allocated_height (GTK_WIDGET (gtk_window)));
    wl_surface_commit (wl_surface);
    wl_display_roundtrip (gdk_wayland_display_get_wl_display (gdk_window_get_display (gdk_window)));
    return self;
}

void
wayland_shell_surface_set_layer_surface_info (WaylandShellSurface *self, uint32_t anchor, int exclusive_zone)
{
    struct _LayerSurfaceInfo *info;
    gboolean initial_setup = FALSE;
    gboolean needs_commit = FALSE;

    g_return_if_fail (self);
    g_return_if_fail (self->layer_surface);

    if (!self->layer_surface_info) {
        self->layer_surface_info = g_new0 (struct _LayerSurfaceInfo, 1);
        initial_setup = TRUE;
    }

    info = self->layer_surface_info;

    if (initial_setup || info->anchor != anchor) {
        zwlr_layer_surface_v1_set_anchor (self->layer_surface, anchor);
        info->anchor = anchor;
        needs_commit = TRUE;
    }

    if (initial_setup || info->exclusive_zone != exclusive_zone) {
        zwlr_layer_surface_v1_set_exclusive_zone (self->layer_surface, exclusive_zone);
        info->exclusive_zone = exclusive_zone;
        needs_commit = TRUE;
    }

    if (needs_commit) {
        wl_surface_commit (gdk_wayland_window_get_wl_surface (gtk_widget_get_window (GTK_WIDGET (self->gtk_window))));
    }
}

static struct xdg_popup *
wayland_shell_surface_make_child_xdg_popup (WaylandShellSurface *self,
                        struct xdg_surface *popup_xdg_surface,
                        struct xdg_positioner *positioner)
{
    if (self->layer_surface) {
        struct xdg_popup *xdg_popup;
        xdg_popup = xdg_surface_get_popup (popup_xdg_surface, NULL, positioner);
        zwlr_layer_surface_v1_get_popup (self->layer_surface, xdg_popup);
        return xdg_popup;
    } else if (self->xdg_surface) {
        return xdg_surface_get_popup (popup_xdg_surface, self->xdg_surface, positioner);
    } else {
        g_warning ("Wayland shell surface %p has no layer or xdg shell surface wayland objects", self);
        return NULL;
    }
}

static enum xdg_positioner_gravity
gdk_gravity_get_xdg_positioner_gravity(GdkGravity gravity)
{
    switch (gravity)
    {
    case GDK_GRAVITY_NORTH_WEST: return XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT;
    case GDK_GRAVITY_NORTH: return XDG_POSITIONER_GRAVITY_BOTTOM;
    case GDK_GRAVITY_NORTH_EAST: return XDG_POSITIONER_GRAVITY_BOTTOM_LEFT;
    case GDK_GRAVITY_WEST: return XDG_POSITIONER_GRAVITY_RIGHT;
    case GDK_GRAVITY_CENTER: return XDG_POSITIONER_GRAVITY_NONE;
    case GDK_GRAVITY_EAST: return XDG_POSITIONER_GRAVITY_LEFT;
    case GDK_GRAVITY_SOUTH_WEST: return XDG_POSITIONER_GRAVITY_TOP_RIGHT;
    case GDK_GRAVITY_SOUTH: return XDG_POSITIONER_GRAVITY_TOP;
    case GDK_GRAVITY_SOUTH_EAST: return XDG_POSITIONER_GRAVITY_TOP_LEFT;
    case GDK_GRAVITY_STATIC: return XDG_POSITIONER_GRAVITY_NONE;
    default: return XDG_POSITIONER_GRAVITY_NONE;
    }
}

static enum xdg_positioner_anchor
gdk_gravity_get_xdg_positioner_anchor(GdkGravity anchor)
{
    switch (anchor)
    {
    case GDK_GRAVITY_NORTH_WEST: return XDG_POSITIONER_ANCHOR_TOP_LEFT;
    case GDK_GRAVITY_NORTH: return XDG_POSITIONER_ANCHOR_TOP;
    case GDK_GRAVITY_NORTH_EAST: return XDG_POSITIONER_ANCHOR_TOP_RIGHT;
    case GDK_GRAVITY_WEST: return XDG_POSITIONER_ANCHOR_LEFT;
    case GDK_GRAVITY_CENTER: return XDG_POSITIONER_ANCHOR_NONE;
    case GDK_GRAVITY_EAST: return XDG_POSITIONER_ANCHOR_RIGHT;
    case GDK_GRAVITY_SOUTH_WEST: return XDG_POSITIONER_ANCHOR_BOTTOM_LEFT;
    case GDK_GRAVITY_SOUTH: return XDG_POSITIONER_ANCHOR_BOTTOM;
    case GDK_GRAVITY_SOUTH_EAST: return XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT;
    case GDK_GRAVITY_STATIC: return XDG_POSITIONER_ANCHOR_NONE;
    default: return XDG_POSITIONER_ANCHOR_NONE;
    }
}

static struct xdg_positioner *
wayland_shell_surface_get_xdg_positioner (WaylandShellSurface *self,
                      GdkGravity anchor,
                      GdkGravity gravity,
                      GdkPoint offset)
{
    GdkRectangle popup_geom; // Rectangle on the wayland surface which makes up the "logical" window (cuts off boarders and shadows)
    struct xdg_positioner *positioner; // Wayland object we're building
    GdkPoint attach_widget_on_window; // Location of the transient for widget on its parent window
    GtkAllocation attach_widget_allocation; // Size of the transient for widget
    GdkWindow *popup_window;
    gint popup_width, popup_height; // Size of the Wayland surface
    double popup_anchor_x = 0, popup_anchor_y = 0; // From 0.0 to 1.0, relative to popup surface size, the point on the popup that will be attached
    GdkPoint positioner_offset; // The final calculated offset to be sent to the positioner

    g_return_val_if_fail (has_initialized, NULL);
    g_return_val_if_fail (xdg_wm_base_global, NULL);
    g_return_val_if_fail (self->transient_for_widget, NULL);

    gtk_widget_translate_coordinates (self->transient_for_widget, gtk_widget_get_toplevel (self->transient_for_widget),
                      0, 0, &attach_widget_on_window.x, &attach_widget_on_window.y);
    gtk_widget_get_allocated_size (self->transient_for_widget, &attach_widget_allocation, NULL);
    wayland_widget_get_logical_geom (GTK_WIDGET (self->gtk_window), &popup_geom);
    popup_window = gtk_widget_get_window (GTK_WIDGET (self->gtk_window));
    popup_width = gdk_window_get_width (popup_window);
    popup_height = gdk_window_get_height (popup_window);

    switch (gravity) {
    case GDK_GRAVITY_WEST:
    case GDK_GRAVITY_SOUTH_WEST:
    case GDK_GRAVITY_NORTH_WEST:
        popup_anchor_x = 1;
        break;

    case GDK_GRAVITY_CENTER:
    case GDK_GRAVITY_SOUTH:
    case GDK_GRAVITY_NORTH:
        popup_anchor_x = 0.5;
        break;

    case GDK_GRAVITY_EAST:
    case GDK_GRAVITY_SOUTH_EAST:
    case GDK_GRAVITY_NORTH_EAST:
        popup_anchor_x = 0;
        break;
    }
    switch (gravity) {
    case GDK_GRAVITY_NORTH:
    case GDK_GRAVITY_NORTH_WEST:
    case GDK_GRAVITY_NORTH_EAST:
        popup_anchor_y = 1;
        break;

    case GDK_GRAVITY_CENTER:
    case GDK_GRAVITY_WEST:
    case GDK_GRAVITY_EAST:
        popup_anchor_y = 0.5;
        break;

    case GDK_GRAVITY_SOUTH:
    case GDK_GRAVITY_SOUTH_EAST:
    case GDK_GRAVITY_SOUTH_WEST:
        popup_anchor_y = 0;
        break;
    }
    positioner_offset.x = (popup_width  - popup_geom.width)  * popup_anchor_x - popup_geom.x + offset.x;
    positioner_offset.y = (popup_height - popup_geom.height) * popup_anchor_y - popup_geom.y + offset.y;

    positioner = xdg_wm_base_create_positioner (xdg_wm_base_global);

    xdg_positioner_set_anchor_rect (positioner,
                    MAX (attach_widget_on_window.x, 0), MAX (attach_widget_on_window.y, 0),
                    MAX (attach_widget_allocation.width, 1), MAX (attach_widget_allocation.height, 1));
    xdg_positioner_set_offset (positioner, positioner_offset.x, positioner_offset.y);
    xdg_positioner_set_anchor (positioner, gdk_gravity_get_xdg_positioner_anchor(anchor));
    xdg_positioner_set_gravity (positioner, gdk_gravity_get_xdg_positioner_gravity(gravity));
    xdg_positioner_set_constraint_adjustment (positioner,
                          XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X
                          | XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y);

    return positioner;
}

WaylandShellSurface *
wayland_shell_surface_get_parent (WaylandShellSurface *self)
{
    if (self->transient_for_widget) {
        return gtk_widget_get_wayland_shell_surface (self->transient_for_widget);
    } else {
        return NULL;
    }
}

WaylandShellSurface *
wayland_shell_surface_get_toplevel (WaylandShellSurface *shell_surface)
{
    WaylandShellSurface *next_shell_surface;
    int wayland_shell_surface_parent_counter = 0;

    next_shell_surface = shell_surface;

    do {
        g_return_val_if_fail (wayland_shell_surface_parent_counter++ < 1000, NULL);
        shell_surface = next_shell_surface;
        next_shell_surface = wayland_shell_surface_get_parent (shell_surface);
    } while (next_shell_surface);

    g_assert (shell_surface);

    return shell_surface;
}

static void
wayland_shell_surface_map_as_popup (WaylandShellSurface *self,
                    struct xdg_positioner *positioner)
{
    GtkRequisition popup_size;
    GdkWindow *popup_window;
    struct wl_surface *popup_wl_surface;
    WaylandShellSurface *transient_for_wayland_shell_surface;

    wayland_shell_surface_unmap (self);

    g_return_if_fail (self->transient_for_widget);

    gtk_widget_get_preferred_size (GTK_WIDGET (self->gtk_window), NULL, &popup_size);
    xdg_positioner_set_size (positioner, popup_size.width, popup_size.height);

    popup_window = gtk_widget_get_window (GTK_WIDGET (self->gtk_window));
    g_return_if_fail (popup_window);

    popup_wl_surface = gdk_wayland_window_get_wl_surface (popup_window);
    g_return_if_fail (popup_wl_surface);
    self->xdg_surface = xdg_wm_base_get_xdg_surface (xdg_wm_base_global, popup_wl_surface);
    xdg_surface_add_listener (self->xdg_surface, &xdg_surface_listener, self);

    transient_for_wayland_shell_surface = gtk_widget_get_wayland_shell_surface (self->transient_for_widget);
    g_return_if_fail (transient_for_wayland_shell_surface);
    self->xdg_popup = wayland_shell_surface_make_child_xdg_popup (transient_for_wayland_shell_surface,
                                      self->xdg_surface,
                                      positioner);
    g_return_if_fail (self->xdg_popup);
    xdg_popup_add_listener (self->xdg_popup, &xdg_popup_listener, self);

    wayland_shell_surface_set_size (self,
                    gtk_widget_get_allocated_width (GTK_WIDGET (self->gtk_window)),
                    gtk_widget_get_allocated_height (GTK_WIDGET (self->gtk_window)));

    wl_surface_commit (popup_wl_surface);
    wl_display_roundtrip (gdk_wayland_display_get_wl_display (gdk_window_get_display (popup_window)));
}

void
wayland_shell_surface_map_popup (WaylandShellSurface *self,
                 GdkGravity anchor,
                 GdkGravity gravity,
                 GdkPoint offset)
{
    struct xdg_positioner *positioner = wayland_shell_surface_get_xdg_positioner (self, anchor, gravity, offset);
    wayland_shell_surface_map_as_popup (self, positioner);
    xdg_positioner_destroy (positioner);
}

static void
wayland_shell_surface_map_popup_auto (WaylandShellSurface *self)
{
    GdkWindow *popup_window = gtk_widget_get_window (GTK_WIDGET (self->gtk_window));
    GdkWinowHackPosition *position = gtk_window_hack_get_position (popup_window);
    GdkRectangle popup_geom;

    struct xdg_positioner *positioner = xdg_wm_base_create_positioner (xdg_wm_base_global);
    wayland_widget_get_logical_geom (GTK_WIDGET (self->gtk_window), &popup_geom);
    xdg_positioner_set_size (positioner,
                             popup_geom.width, popup_geom.height);
    xdg_positioner_set_anchor_rect (positioner,
                                    position->rect.x, position->rect.y,
                                    position->rect.width, position->rect.height);
    xdg_positioner_set_offset (positioner,
                               position->rect_anchor_dx,
                               position->rect_anchor_dy);
    xdg_positioner_set_anchor (positioner,
                               gdk_gravity_get_xdg_positioner_anchor(position->rect_anchor));
    xdg_positioner_set_gravity (positioner,
                                gdk_gravity_get_xdg_positioner_gravity(position->window_anchor));
    xdg_positioner_set_constraint_adjustment (positioner,
                          XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X
                          | XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y);// TODO

    struct wl_surface *popup_wl_surface = gdk_wayland_window_get_wl_surface (popup_window);
    g_return_if_fail (popup_wl_surface);
    self->xdg_surface = xdg_wm_base_get_xdg_surface (xdg_wm_base_global, popup_wl_surface);
    xdg_surface_add_listener (self->xdg_surface, &xdg_surface_listener, self);

    GdkWindow *transient_for_gdk_window = gdk_window_hack_get_transient_for (popup_window);
    g_return_if_fail (transient_for_gdk_window);
    WaylandShellSurface *transient_for_wayland_shell_surface = g_object_get_data (G_OBJECT (transient_for_gdk_window), wayland_shell_surface_key);
    g_return_if_fail (transient_for_wayland_shell_surface);
    self->xdg_popup = wayland_shell_surface_make_child_xdg_popup (transient_for_wayland_shell_surface,
                                                                  self->xdg_surface,
                                                                  positioner);
    g_return_if_fail (self->xdg_popup);
    xdg_positioner_destroy (positioner);
    xdg_popup_add_listener (self->xdg_popup, &xdg_popup_listener, self);

    wayland_shell_surface_set_size (self,
                    gtk_widget_get_allocated_width (GTK_WIDGET (self->gtk_window)),
                    gtk_widget_get_allocated_height (GTK_WIDGET (self->gtk_window)));

    wl_surface_commit (popup_wl_surface);
    wl_display_roundtrip (gdk_wayland_display_get_wl_display (gdk_window_get_display (popup_window)));
}

static gboolean
wayland_popup_map_event_cb (GtkWidget *popup_widget, GdkEvent *event, WaylandShellSurface *shell_surface)
{
    g_return_val_if_fail (shell_surface, FALSE);
    g_warn_if_fail (popup_widget == GTK_WIDGET (shell_surface->gtk_window));

    if (shell_surface->is_tooltip) {
        g_warn_if_fail (last_query_tooltip_widget);
        shell_surface->transient_for_widget = last_query_tooltip_widget;
    }

    /*
    if (wayland_shell_surface_popup_callback) {
        wayland_shell_surface_popup_callback (shell_surface);
    }
    */
    wayland_shell_surface_map_popup_auto (shell_surface);

    return TRUE;
}


// This callback only does anything for popups of Wayland surfaces
static void
wayland_window_realize_override_cb (GtkWindow *gtk_window, void *_data)
{
    GtkWidget *window_widget, *transient_for_widget;
    gboolean is_tooltip = FALSE;

    // Call the default realize handler
    GValue args[1] = { G_VALUE_INIT };
    g_value_init_from_instance (&args[0], gtk_window);
    g_signal_chain_from_overridden (args, NULL);
    g_value_unset (&args[0]);

    window_widget = GTK_WIDGET (gtk_window);

    if (gtk_window_get_type_hint (gtk_window) == GDK_WINDOW_TYPE_HINT_TOOLTIP) {
        is_tooltip = TRUE;
    }

    transient_for_widget = gtk_window_get_attached_to (gtk_window);
    if (!transient_for_widget)
        transient_for_widget = GTK_WIDGET (gtk_window_get_transient_for (gtk_window));

    if (transient_for_widget && gtk_widget_get_wayland_shell_surface (transient_for_widget)) {
        WaylandShellSurface *shell_surface;

        gdk_window_hack_init (gtk_widget_get_window (window_widget));
        shell_surface = gtk_widget_get_wayland_shell_surface (window_widget);

        if (shell_surface) {
            wayland_shell_surface_unmap (shell_surface);
        } else {
            shell_surface = wayland_shell_surface_new (gtk_window);
            g_signal_connect (gtk_window, "map-event", G_CALLBACK (wayland_popup_map_event_cb), shell_surface);
            // unmap needs to be handled by a type-level override
        }

        shell_surface->is_tooltip = is_tooltip;
        shell_surface->transient_for_widget = transient_for_widget;
    }
}

// This callback must override the default unmap handler, so it can run first
// wayland_popup_data_unmap () must be called before GtkWidget's unmap, or Wayland objects are destroyed in the wrong order
static void
wayland_window_unmap_override_cb (GtkWindow *gtk_window, void *_data)
{
    WaylandShellSurface *shell_surface;

    shell_surface = gtk_widget_get_wayland_shell_surface (GTK_WIDGET (gtk_window));
    if (shell_surface)
        wayland_shell_surface_unmap (shell_surface);

    // Call the default unmap handler
    GValue args[1] = { G_VALUE_INIT };
    g_value_init_from_instance (&args[0], gtk_window);
    g_signal_chain_from_overridden (args, NULL);
    g_value_unset (&args[0]);
}

static gboolean
wayland_query_tooltip_emission_hook (GSignalInvocationHint *_ihint,
                     guint n_param_values,
                     const GValue *param_values,
                     gpointer _data)
{
    last_query_tooltip_widget = GTK_WIDGET (g_value_peek_pointer(&param_values[0]));
    return TRUE; // Always stay connected
}

void
wayland_shell_surface_global_init ()
{
    GdkDisplay *gdk_display;
    gint realize_signal_id, unmap_signal_id, query_tooltip_signal_id;
    GClosure *realize_closure, *unmap_closure;

    g_assert_false (has_initialized);

    gdk_display = gdk_display_get_default ();
    g_return_if_fail (gdk_display);
    g_return_if_fail (GDK_IS_WAYLAND_DISPLAY (gdk_display));

    struct wl_display *wl_display = gdk_wayland_display_get_wl_display (gdk_display);
    struct wl_registry *wl_registry = wl_display_get_registry (wl_display);
    wl_registry_add_listener (wl_registry, &wl_registry_listener, NULL);
    wl_display_roundtrip (wl_display);

    if (!layer_shell_global)
        g_warning ("It appears your Wayland compositor does not support the Layer Shell protocol");

    realize_signal_id = g_signal_lookup ("realize", GTK_TYPE_WINDOW);
    realize_closure = g_cclosure_new (G_CALLBACK (wayland_window_realize_override_cb), NULL, NULL);
    g_signal_override_class_closure (realize_signal_id, GTK_TYPE_WINDOW, realize_closure);

    unmap_signal_id = g_signal_lookup ("unmap", GTK_TYPE_WINDOW);
    unmap_closure = g_cclosure_new (G_CALLBACK (wayland_window_unmap_override_cb), NULL, NULL);
    g_signal_override_class_closure (unmap_signal_id, GTK_TYPE_WINDOW, unmap_closure);

    query_tooltip_signal_id = g_signal_lookup ("query-tooltip", GTK_TYPE_WIDGET);
    g_signal_add_emission_hook (query_tooltip_signal_id, 0, wayland_query_tooltip_emission_hook, NULL, NULL);

    has_initialized = TRUE;
}

void
wayland_shell_surface_set_popup_callback(void (*map_popup_callback)(WaylandShellSurface *self))
{
    wayland_shell_surface_popup_callback = map_popup_callback;
}
