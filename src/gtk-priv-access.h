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

#ifndef GDK_WINDOW_HACK_H
#define GDK_WINDOW_HACK_H

#include <gdk/gdk.h>
#include <stdint.h>

// This only has an effect the first time it's called
// It enables gtk_window_hack_get_position () working later
void gtk_priv_access_init (GdkWindow *gdk_window);

// Returns the laster serial from a user input event
// Can be used for popups grabs and such
uint32_t gdk_window_get_priv_latest_serial (GdkSeat *seat);

// Returns the GdkSeat that can be used for popup grabs
GdkSeat *gdk_window_get_priv_grab_seat (GdkWindow *gdk_window);

// Sets the window as mapped (mapped is set to false automatically in gdk_wayland_window_hide_surface ())
// If window is not set to mapped, some subsurfaces fail (see https://github.com/wmww/gtk-layer-shell/issues/38)
void gdk_window_set_priv_mapped (GdkWindow *gdk_window);

#endif // GDK_WINDOW_HACK_H
