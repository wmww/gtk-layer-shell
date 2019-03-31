#ifndef WAYLAND_GLOBALS_H
#define WAYLAND_GLOBALS_H

#include <glib.h> // gboolean

gboolean gtk_wayland_get_has_initialized (void);
void gtk_wayland_init_if_needed (void);

struct xdg_wm_base *gtk_wayland_get_xdg_wm_base_global (void);
struct zwlr_layer_shell_v1 *gtk_wayland_get_layer_shell_global (void);

#endif // WAYLAND_GLOBALS_H
