#ifndef XDG_POPUP_SURFACE_H
#define XDG_POPUP_SURFACE_H

#include "custom-shell-surface.h"

typedef struct _GtkWidget GtkWidget;

// a LayerSurface * can be safely cast to a CustomShellSurface *
typedef struct _XdgPopupSurface XdgPopupSurface;

XdgPopupSurface *xdg_popup_surface_new (GtkWindow *gtk_window);

// Safe cast, returns NULL if wrong type sent
XdgPopupSurface *custom_shell_surface_get_xdg_popup (CustomShellSurface *shell_surface);

void xdg_popup_surface_set_parent (XdgPopupSurface *self,
                                   CustomShellSurface *parent_shell_surface,
                                   GtkWidget *parent_widget);

#endif // XDG_POPUP_SURFACE_H
