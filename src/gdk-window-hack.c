#include "gdk-window-hack.h"
#include "gtk-wayland.h"
#include "xdg-popup-surface.h"

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
    // Assume the transient_for GdkWindow* is the 3rd pointer after the GObject in GdkWindow (gdkinternals.h:206)
    void **transient_for = (void**)((char *)gdk_window + sizeof(GObject) + 2 * sizeof(void *));
    return GDK_WINDOW (*transient_for);
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

    // Assume a GdkWindowImpl* is the first thing in a GdkWindow after the parent GObject (gdkinternals.h:203)
    void *gdk_window_impl = *(void **)((char *)gdk_window + sizeof(GObject));

    // Assume a GdkWindowImplClass* is the first thing in a GdkWindowImpl (a class pointer is the first thing in a GObject)
    void *gdk_window_impl_class = *(void **)gdk_window_impl;

    // Assume there is a GObjectClass and 10 function pointers in GdkWindowImplClass before move_to_rect (gdkwindowimpl.h:78)
    MoveToRectFunc *move_to_rect_func_ptr_ptr = (MoveToRectFunc *)((char *)gdk_window_impl_class + sizeof(GObjectClass) + 10 * sizeof(void *));

    // If we have not already done the override, set the window's function to be the override and our "real" fp to the one that was there before
    if (*move_to_rect_func_ptr_ptr != gdk_window_move_to_rect_impl_override) {
        gdk_window_move_to_rect_real = *move_to_rect_func_ptr_ptr;
        *move_to_rect_func_ptr_ptr = gdk_window_move_to_rect_impl_override;
    }
}
