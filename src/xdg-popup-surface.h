#ifndef XDG_POPUP_SURFACE_H
#define XDG_POPUP_SURFACE_H

#include "custom-shell-surface.h"

// an XdgPopupSurface * can be safely cast to a CustomShellSurface *
typedef struct _XdgPopupSurface XdgPopupSurface;

typedef struct
{
    CustomShellSurface *transient_for_shell_surface;
    GdkWindow *transient_for_gdk_window;
    GdkRectangle rect;
    GdkGravity rect_anchor,  window_anchor;
    GdkAnchorHints anchor_hints;
    GdkPoint rect_anchor_d;
} XdgPopupPosition;

// Copies position, does not take ownership
XdgPopupSurface *xdg_popup_surface_new (GtkWindow *gtk_window, XdgPopupPosition const* position);

// Copies position, does not take ownership
void xdg_popup_surface_update_position (XdgPopupSurface *self, XdgPopupPosition const* position);

// Safe cast, returns NULL if wrong type sent
XdgPopupSurface *custom_shell_surface_get_xdg_popup (CustomShellSurface *shell_surface);

#endif // XDG_POPUP_SURFACE_H
