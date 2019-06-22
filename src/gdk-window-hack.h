#ifndef GDK_WINDOW_HACK_H
#define GDK_WINDOW_HACK_H

#include <gdk/gdk.h>

// This only has an effect the first time it's called
// It enables gtk_window_hack_get_position () working later
void gdk_window_hack_init (GdkWindow *gdk_window);

#endif // GDK_WINDOW_HACK_H
