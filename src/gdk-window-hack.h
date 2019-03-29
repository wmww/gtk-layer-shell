#ifndef GDK_WINDOW_HACK_H
#define GDK_WINDOW_HACK_H

#include <gdk/gdk.h>

// This only has an effect the first time it's called
// It enables gtk_window_hack_get_position () working later
void gdk_window_hack_init (GdkWindow *gdk_window);

typedef struct _GdkWinowHackPosition GdkWinowHackPosition;
struct _GdkWinowHackPosition
{
    GdkRectangle rect;
    GdkGravity rect_anchor,  window_anchor;
    GdkAnchorHints anchor_hints;
    int rect_anchor_dx, rect_anchor_dy;
};

// Returned memory is valid for the lifetime of the window
GdkWinowHackPosition* gtk_window_hack_get_position (GdkWindow *gdk_window);

GdkWindow *gdk_window_hack_get_transient_for (GdkWindow *gdk_window);

#endif // GDK_WINDOW_HACK_H
