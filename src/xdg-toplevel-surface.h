#ifndef XDG_TOPLEVEL_SURFACE_H
#define XDG_TOPLEVEL_SURFACE_H

#include "custom-shell-surface.h"

// an XdgToplevelSurface * can be safely cast to a CustomShellSurface *
typedef struct _XdgToplevelSurface XdgToplevelSurface;

XdgToplevelSurface *xdg_toplevel_surface_new (GtkWindow *gtk_window);

// Safe cast, returns NULL if wrong type sent
XdgToplevelSurface *custom_shell_surface_get_xdg_toplevel (CustomShellSurface *shell_surface);

#endif // XDG_TOPLEVEL_SURFACE_H
