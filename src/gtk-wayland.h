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
