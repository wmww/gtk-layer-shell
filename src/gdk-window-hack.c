/* This entire file is licensed under MIT
 *
 * Copyright 2020 William Wold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "gdk-window-hack.h"
#include "gtk-wayland.h"
#include "xdg-popup-surface.h"

#include "wayland-client.h"

typedef enum _PositionMethod
{
  POSITION_METHOD_ENUM
} PositionMethod;
typedef void *EGLSurface;
typedef void *GdkWaylandWindowExported;

#include "gdk_window_impl_priv.h"
#include "gdk_window_priv.h"
#include "gdk_window_impl_wayland_priv.h"
#include "gdk_window_impl_class_priv.h"

#include <glib-2.0/glib.h>

// The type of the function pointer of GdkWindowImpl's move_to_rect method (gdkwindowimpl.h:78)'
typedef void (*MoveToRectFunc) (GdkWindow *window,
                                const GdkRectangle *rect,
                                GdkGravity rect_anchor,
                                GdkGravity window_anchor,
                                GdkAnchorHints anchor_hints,
                                int rect_anchor_dx,
                                int rect_anchor_dy);

static MoveToRectFunc gdk_window_move_to_rect_real = NULL;

static GdkWindow *
gdk_window_hack_get_transient_for (GdkWindow *gdk_window)
{
    GdkWindow *window_transient_for = gdk_window_priv_get_transient_for (gdk_window);
    GdkWindowImplWayland *window_impl = (GdkWindowImplWayland *)gdk_window_priv_get_impl (gdk_window);
    GdkWindow *wayland_transient_for = gdk_window_impl_wayland_priv_get_transient_for (window_impl);
    if (window_transient_for != wayland_transient_for) {
        g_warning ("Wayland transient_for (%p) != generic transient_for (%p)",
                   wayland_transient_for,
                   window_transient_for);
        if (!window_transient_for)
            return wayland_transient_for;
    }
    return window_transient_for;
}

static void
gdk_window_move_to_rect_impl_override (GdkWindow *window,
                                       const GdkRectangle *rect,
                                       GdkGravity rect_anchor,
                                       GdkGravity window_anchor,
                                       GdkAnchorHints anchor_hints,
                                       int rect_anchor_dx,
                                       int rect_anchor_dy)
{
    g_assert (gdk_window_move_to_rect_real);
    gdk_window_move_to_rect_real (window,
                                  rect,
                                  rect_anchor,
                                  window_anchor,
                                  anchor_hints,
                                  rect_anchor_dx,
                                  rect_anchor_dy);

    GdkWindow *transient_for_gdk_window = gdk_window_hack_get_transient_for (window);
    CustomShellSurface *transient_for_shell_surface = NULL;
    GdkWindow *toplevel_gdk_window = transient_for_gdk_window;
    while (toplevel_gdk_window) {
        toplevel_gdk_window = gdk_window_get_toplevel (toplevel_gdk_window);
        GtkWindow *toplevel_gtk_window = gtk_wayland_gdk_to_gtk_window (toplevel_gdk_window);
        transient_for_shell_surface = gtk_window_get_custom_shell_surface (toplevel_gtk_window);
        if (transient_for_shell_surface)
            break;
        toplevel_gdk_window = gdk_window_hack_get_transient_for (toplevel_gdk_window);
    }
    if (transient_for_shell_surface) {
        g_return_if_fail (rect);
        XdgPopupPosition position = {
            .transient_for_shell_surface = transient_for_shell_surface,
            .transient_for_gdk_window = transient_for_gdk_window,
            .rect = *rect,
            .rect_anchor = rect_anchor,
            .window_anchor = window_anchor,
            .anchor_hints = anchor_hints,
            .rect_anchor_d = {
                .x = rect_anchor_dx,
                .y = rect_anchor_dy,
            },
        };
        gtk_wayland_setup_window_as_custom_popup (window, &position);
    }
}

void
gdk_window_hack_init (GdkWindow *gdk_window)
{
    // Don't do anything once this has run successfully once
    if (gdk_window_move_to_rect_real)
        return;

    GdkWindowImplWayland *window_impl = (GdkWindowImplWayland *)gdk_window_priv_get_impl (gdk_window);
    GdkWindowImplClass *window_class = (GdkWindowImplClass *)G_OBJECT_GET_CLASS(window_impl);

    // If we have not already done the override, set the window's function to be the override and our "real" fp to the one that was there before
    if (gdk_window_impl_class_priv_get_move_to_rect (window_class) != gdk_window_move_to_rect_impl_override) {
        gdk_window_move_to_rect_real = gdk_window_impl_class_priv_get_move_to_rect (window_class);
        gdk_window_impl_class_priv_set_move_to_rect (window_class, gdk_window_move_to_rect_impl_override);
    }
}
