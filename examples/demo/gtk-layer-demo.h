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

#ifndef GTK_LAYER_DEMO_H
#define GTK_LAYER_DEMO_H

#include "gtk-layer-shell.h"
#include <gtk/gtk.h>

typedef enum {
    WINDOW_ORIENTATION_NONE = 0,
    WINDOW_ORIENTATION_HORIZONTAL,
    WINDOW_ORIENTATION_VERTICAL,
} WindowOrientation;

typedef struct {
    gboolean edges[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER];
    WindowOrientation orientation;
    GtkWidget *toplevel_box;
    GtkWidget *first_box;
    GtkWidget *second_box;
} ToplevelData;

extern const char *anchor_edges_key;
extern const int fixed_size_width;
extern const int fixed_size_height;

void
layer_window_update_orientation (GtkWindow *layer_window);

GtkWidget *
layer_selection_new (GtkWindow *layer_window, GtkLayerShellLayer default_layer);

GtkWidget *
monitor_selection_new (GtkWindow *layer_window);

GtkWidget *
anchor_control_new (GtkWindow *layer_window, const gboolean default_anchors[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER]);

GtkWidget *
margin_control_new (GtkWindow *layer_window, const int default_margins[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER]);

GtkWidget *
version_info_new ();

GtkWidget *
mscl_toggles_new (GtkWindow *layer_window,
                  gboolean default_auto_exclusive_zone,
                  gboolean default_fixed_size);

GtkWidget *
menu_bar_new (GtkWindow *layer_window);

GtkWidget *
keyboard_selection_new (GtkWindow *layer_window, GtkLayerShellKeyboardMode default_kb);

#endif // GTK_LAYER_DEMO_H
