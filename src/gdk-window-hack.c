#include "gdk-window-hack.h"

#include <glib-2.0/glib.h>

static const char *popup_data_key = "hack_popup_position";

// The type of the function pointer of GdkWindowImpl's move_to_rect method (gdkwindowimpl.h:78)'
typedef void (*MoveToRectFunc) (GdkWindow *window,
                                const GdkRectangle *rect,
                                GdkGravity rect_anchor,
                                GdkGravity window_anchor,
                                GdkAnchorHints anchor_hints,
                                int rect_anchor_dx,
                                int rect_anchor_dy);

static MoveToRectFunc gdk_window_move_to_rect_real = NULL;

static void
gdk_window_move_to_rect_impl_override (GdkWindow *window,
                                       const GdkRectangle *rect,
                                       GdkGravity rect_anchor,
                                       GdkGravity window_anchor,
                                       GdkAnchorHints anchor_hints,
                                       int rect_anchor_dx,
                                       int rect_anchor_dy)
{
    GdkWinowHackPosition* position_data = gtk_window_hack_get_position (window);
    if (!position_data) {
        position_data = g_new0 (GdkWinowHackPosition, 1);
        g_object_set_data_full (G_OBJECT (window), popup_data_key, position_data, g_free);
    }

    if (rect) {
        position_data->rect = *rect;
    } else {
        g_warning ("No rect sent to gdk_window_move_to_rect_impl_override ()");
    }
    position_data->rect_anchor = rect_anchor;
    position_data->window_anchor = window_anchor;
    position_data->anchor_hints = anchor_hints;
    position_data->rect_anchor_dx = rect_anchor_dx;
    position_data->rect_anchor_dy = rect_anchor_dy;

    g_assert (gdk_window_move_to_rect_real);
    gdk_window_move_to_rect_real (window, rect, rect_anchor, window_anchor, anchor_hints, rect_anchor_dx, rect_anchor_dy);
}

void
gdk_window_hack_init (GdkWindow *gdk_window)
{
    // Don't do anything once this has run successfully once
    if (gdk_window_move_to_rect_real)
        return;

    // Assume a GdkWindowImpl* is the first thing in a GdkWindow after the parent GObject (gdkinternals.h:203)
    void *gdk_window_impl = *(void **)((void *)gdk_window + sizeof(GObject));

    // Assume a GdkWindowImplClass* is the first thing in a GdkWindowImpl (a class pointer is the first thing in a GObject)
    void *gdk_window_impl_class = *(void **)gdk_window_impl;

    // Assume there is a GObjectClass and 10 function pointers in GdkWindowImplClass before move_to_rect (gdkwindowimpl.h:78)
    MoveToRectFunc *move_to_rect_func_ptr_ptr = gdk_window_impl_class + sizeof(GObjectClass) + 10 * sizeof(void *);

    // If we have not already done the override, set the window's function to be the override and our "real" fp to the one that was there before
    if (*move_to_rect_func_ptr_ptr != gdk_window_move_to_rect_impl_override) {
        gdk_window_move_to_rect_real = *move_to_rect_func_ptr_ptr;
        *move_to_rect_func_ptr_ptr = gdk_window_move_to_rect_impl_override;
    }
}

// Returned memory is valid for the lifetime of the window
GdkWinowHackPosition* gtk_window_hack_get_position (GdkWindow *gdk_window)
{
    return g_object_get_data (G_OBJECT (gdk_window), popup_data_key);
}
