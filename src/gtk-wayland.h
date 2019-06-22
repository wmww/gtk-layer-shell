#ifndef WAYLAND_GLOBALS_H
#define WAYLAND_GLOBALS_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "xdg-popup-surface.h"

gboolean gtk_wayland_get_has_initialized (void);
struct xdg_wm_base *gtk_wayland_get_xdg_wm_base_global (void);
struct zwlr_layer_shell_v1 *gtk_wayland_get_layer_shell_global (void);

void gtk_wayland_init_if_needed (void);

GtkWindow *gtk_wayland_gdk_to_gtk_window (GdkWindow *gdk_window);

// Does not take ownership of position
void gtk_wayland_setup_window_as_custom_popup (GdkWindow *gdk_window, XdgPopupPosition const *position);

GdkRectangle gtk_wayland_get_logical_geom (GtkWindow *widget);

#endif // WAYLAND_GLOBALS_H
