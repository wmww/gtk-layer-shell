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

#ifndef XDG_POPUP_SURFACE_H
#define XDG_POPUP_SURFACE_H

#include "custom-shell-surface.h"

// an XdgPopupSurface * can be safely cast to a CustomShellSurface *
typedef struct _XdgPopupSurface XdgPopupSurface;

typedef struct
{
    CustomShellSurface *transient_for_shell_surface;
    GdkWindow *transient_for_gdk_window;
    GdkRectangle rect;
    GdkGravity rect_anchor,  window_anchor;
    GdkAnchorHints anchor_hints;
    GdkPoint rect_anchor_d;
} XdgPopupPosition;

// Copies position, does not take ownership
XdgPopupSurface *xdg_popup_surface_new (GtkWindow *gtk_window, XdgPopupPosition const* position);

// Copies position, does not take ownership
void xdg_popup_surface_update_position (XdgPopupSurface *self, XdgPopupPosition const* position);

// Safe cast, returns NULL if wrong type sent
XdgPopupSurface *custom_shell_surface_get_xdg_popup (CustomShellSurface *shell_surface);

#endif // XDG_POPUP_SURFACE_H
