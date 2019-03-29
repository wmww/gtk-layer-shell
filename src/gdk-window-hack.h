#ifndef GDK_WINDOW_HACK_H
#define GDK_WINDOW_HACK_H

#include <gdk/gdk.h>

// This only has an effect the first time it's called'
void gdk_window_hack_init (GdkWindow *gdk_window);

GdkWindow *gdk_window_hack_get_transient_for (GdkWindow *gdk_window);

#endif // GDK_WINDOW_HACK_H
