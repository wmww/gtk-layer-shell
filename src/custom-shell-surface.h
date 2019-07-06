#ifndef CUSTOM_SHELL_SURFACE_H
#define CUSTOM_SHELL_SURFACE_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>

struct wl_surface;
struct xdg_surface;
struct xdg_positioner;

typedef struct _CustomShellSurface CustomShellSurface;
typedef struct _CustomShellSurfacePrivate CustomShellSurfacePrivate;
typedef struct _CustomShellSurfaceVirtual CustomShellSurfaceVirtual;

struct _CustomShellSurfaceVirtual
{
    // Called during the window's gtk signal of the same name
    // Should create the wayland objects needed to map the surface
    void (*map) (CustomShellSurface *super, struct wl_surface *wl_surface);

    // Must be called before the associated GtkWindow is unmapped
    void (*unmap) (CustomShellSurface *super);

    // Will usually call unmap; can be the same function if no other resources need to be freed
    void (*finalize) (CustomShellSurface *super);

    struct xdg_popup *(*get_popup) (CustomShellSurface *super,
                                    struct xdg_surface *popup_xdg_surface,
                                    struct xdg_positioner *positioner);

    // Returns the logical geometry of the window (excludes shadows and such)
    GdkRectangle (*get_logical_geom) (CustomShellSurface *super);
};

struct _CustomShellSurface
{
    CustomShellSurfaceVirtual const *virtual;
    CustomShellSurfacePrivate *private;
};

// Usually called by the subclass constructors
// Does not map the surface yet
void custom_shell_surface_init (CustomShellSurface *self, GtkWindow *gtk_window);

// If the window has a shell surface, return it; else return NULL
// NULL input is handled gracefully
CustomShellSurface *gtk_window_get_custom_shell_surface (GtkWindow *gtk_window);

GtkWindow *custom_shell_surface_get_gtk_window (CustomShellSurface *self);

// In theory this could commit once on next event loop, but for now it will just commit every time it is called
// Does nothing is the shell surface does not currently have a GdkWindow with a wl_surface
void custom_shell_surface_needs_commit (CustomShellSurface *self);

// Unmap and remap a currently mapped shell surface
void custom_shell_surface_remap (CustomShellSurface *self);

// Destruction is taken care of automatically when the associated window is destroyed

#endif // CUSTOM_SHELL_SURFACE_H
