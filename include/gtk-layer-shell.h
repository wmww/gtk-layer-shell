#ifndef GTK_LAYER_SHELL_H
#define GTK_LAYER_SHELL_H

#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>
#include <gtk/gtk.h>

// Types defined in Wayland protocol headers
struct xdg_wm_base;
enum xdg_positioner_anchor;
enum xdg_positioner_gravity;
struct zwlr_layer_shell_v1;

// Both always attached to GDK windows (not GTK ones)
extern const char *wayland_shell_surface_key;

// If we are currently running Wayland
gboolean is_using_wayland (void);
struct zwlr_layer_shell_v1 *get_layer_shell_global (void);
struct xdg_wm_base *get_xdg_wm_base_global (void);
gboolean gtk_layer_shell_has_initialized (void);

// Current state of a Layer surface (not used for layer surface popups)
struct _LayerSurfaceInfo {
    uint32_t anchor;
    int exclusive_zone;
};

typedef struct _LayerSurfaceInfo LayerSurfaceInfo;

// The Wayland variables for some sort of custom Wayland shell surface (can be a Layer surface, XDG stable toplevel or XDG popup)
struct _WaylandShellSurface {
    GtkWindow *gtk_window;
    GtkWidget *transient_for_widget;
    gint width, height;

    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
    struct xdg_popup *xdg_popup;
    gboolean is_tooltip;

    struct zwlr_layer_surface_v1 *layer_surface;
    LayerSurfaceInfo *layer_surface_info;
};

typedef struct _WaylandShellSurface WaylandShellSurface;

// Set up global variables and callbacks needed for Wayland shell surfaces
void wayland_shell_surface_global_init (void(*map_popup_callback)(WaylandShellSurface *self));

// If the widget or a parent has a WaylandShellSurface, return it, else return NULL
WaylandShellSurface *gtk_widget_get_wayland_shell_surface (GtkWidget *widget);

// Create a new Wayland shell surface backed by a Layer Surface
// Should be called in the widget's realize signal'
WaylandShellSurface *wayland_shell_surface_new_layer_surface (GtkWindow *gtk_window,
                                                              struct wl_output *output,
                                                              uint32_t layer,
                                                              const char *name_space);

// Should only be called on Wayland shell surfaces with a layer surface, sets the anchor and exclusive zone
void wayland_shell_surface_set_layer_surface_info (WaylandShellSurface *self, uint32_t anchor, int exclusive_zone);

// With an already existing (but not mapped) WaylandShellSurface, map it as a popup, usually called from the popup callback
void wayland_shell_surface_map_popup (WaylandShellSurface *self,
                                      enum xdg_positioner_anchor anchor,
                                      enum xdg_positioner_gravity gravity,
                                      GdkPoint offset);

// If this shell surface is transient for another Wayland shell surface, return that. Else return NULL
WaylandShellSurface *wayland_shell_surface_get_parent (WaylandShellSurface *self);

// Keep getting parents until we hit the top, then return the last one
WaylandShellSurface *wayland_shell_surface_get_toplevel (WaylandShellSurface *shell_surface);

// Idempotent; deletes the Wayland objects for a shell surface
void wayland_shell_surface_unmap (WaylandShellSurface *self);

#endif // GTK_LAYER_SHELL_H
