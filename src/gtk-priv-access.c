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

#include "gtk-priv-access.h"
#include "gtk-wayland.h"
#include "xdg-popup-surface.h"

#include "wayland-client.h"

typedef enum _PositionMethod
{
  POSITION_METHOD_ENUM
} PositionMethod;
typedef void *EGLSurface;
typedef void *GdkWaylandWindowExported;
typedef void *GdkWaylandTabletToolData;

#include "gdk_window_impl_priv.h"
#include "gdk_window_priv.h"
#include "gdk_window_impl_wayland_priv.h"
#include "gdk_window_impl_class_priv.h"
#include "gdk_wayland_pointer_frame_data_priv.h"
#include "gdk_wayland_pointer_data_priv.h"
#include "gdk_wayland_seat_priv.h"
#include "gdk_wayland_touch_data_priv.h"
#include "gdk_wayland_tablet_data_priv.h"

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
gdk_window_get_priv_transient_for (GdkWindow *gdk_window)
{
    GdkWindow *window_transient_for = gdk_window_priv_get_transient_for (gdk_window);
    GdkWindowImplWayland *window_impl = (GdkWindowImplWayland *)gdk_window_priv_get_impl (gdk_window);
    GdkWindow *wayland_transient_for = gdk_window_impl_wayland_priv_get_transient_for (window_impl);
    if (wayland_transient_for)
        return wayland_transient_for;
    else
        return window_transient_for;
}

uint32_t
gdk_window_get_priv_latest_serial (GdkSeat *seat)
{
    uint32_t serial = 0;
    GdkWaylandSeat *wayland_seat = (GdkWaylandSeat *)seat;

    serial = MAX(serial, gdk_wayland_seat_priv_get_keyboard_key_serial (wayland_seat));

    GdkWaylandPointerData* pointer_data = gdk_wayland_seat_priv_get_pointer_info_ptr (wayland_seat);
    serial = MAX(serial, gdk_wayland_pointer_data_priv_get_press_serial (pointer_data));

    GHashTableIter i;
    GdkWaylandTouchData *touch;
    g_hash_table_iter_init (&i, gdk_wayland_seat_priv_get_touches (wayland_seat));
    while (g_hash_table_iter_next (&i, NULL, (gpointer *)&touch))
        serial = MAX(serial, gdk_wayland_touch_data_priv_get_touch_down_serial (touch));

    for (GList *l = gdk_wayland_seat_priv_get_tablets (wayland_seat); l; l = l->next) {
        GdkWaylandTabletData *tablet_data = l->data;
        GdkWaylandPointerData *pointer_data = gdk_wayland_tablet_data_priv_get_pointer_info_ptr (tablet_data);
        serial = MAX(serial, gdk_wayland_pointer_data_priv_get_press_serial (pointer_data));
    }

    return serial;
}

static GdkSeat *
gdk_window_get_priv_grab_seat_for_single_window (GdkWindow *gdk_window)
{
    if (!gdk_window)
        return NULL;

    GdkWindowImplWayland *window_impl = (GdkWindowImplWayland *)gdk_window_priv_get_impl (gdk_window);
    return gdk_window_impl_wayland_priv_get_grab_input_seat (window_impl);
}

GdkSeat *
gdk_window_get_priv_grab_seat (GdkWindow *gdk_window)
{
    GdkSeat *seat = NULL;

    seat = gdk_window_get_priv_grab_seat_for_single_window (gdk_window);
    if (seat)
        return seat;

    // see the comment in find_grab_input_seat ()
    GdkWindow* grab_window = g_object_get_data (G_OBJECT (gdk_window), "gdk-attached-grab-window");
    seat = gdk_window_get_priv_grab_seat_for_single_window (grab_window);
    if (seat)
        return seat;

    while (gdk_window)
    {
        gdk_window = gdk_window_get_priv_transient_for (gdk_window);

        seat = gdk_window_get_priv_grab_seat_for_single_window (gdk_window);
        if (seat)
            return seat;
    }

    return NULL;
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

    GdkWindow *transient_for_gdk_window = gdk_window_get_priv_transient_for (window);
    CustomShellSurface *transient_for_shell_surface = NULL;
    GdkWindow *toplevel_gdk_window = transient_for_gdk_window;
    while (toplevel_gdk_window) {
        toplevel_gdk_window = gdk_window_get_toplevel (toplevel_gdk_window);
        GtkWindow *toplevel_gtk_window = gtk_wayland_gdk_to_gtk_window (toplevel_gdk_window);
        transient_for_shell_surface = gtk_window_get_custom_shell_surface (toplevel_gtk_window);
        if (transient_for_shell_surface)
            break;
        toplevel_gdk_window = gdk_window_get_priv_transient_for (toplevel_gdk_window);
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
gdk_window_set_priv_mapped (GdkWindow *gdk_window)
{
    GdkWindowImplWayland *window_impl = (GdkWindowImplWayland *)gdk_window_priv_get_impl (gdk_window);
    gdk_window_impl_wayland_priv_set_mapped (window_impl, TRUE);
}

void
gtk_priv_access_init (GdkWindow *gdk_window)
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
