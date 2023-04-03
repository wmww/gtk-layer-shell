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

#ifndef LAYER_SHELL_SURFACE_H
#define LAYER_SHELL_SURFACE_H

#include "custom-shell-surface.h"
#include "wlr-layer-shell-unstable-v1-client.h"
#include "gtk-layer-shell.h"
#include <gtk/gtk.h>

// A LayerSurface * can be safely cast to a CustomShellSurface *
typedef struct _LayerSurface LayerSurface;

// Functions that mutate this structure should all be in layer-surface.c to make the logic easier to understand
// Struct is declared in this header to prevent the need for excess getters
struct _LayerSurface
{
    CustomShellSurface super;

    // Can be set at any time
    gboolean anchors[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER]; // The current anchor
    int margins[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER]; // The current margins
    int exclusive_zone; // The current exclusive zone (set either explicitly or automatically)
    gboolean auto_exclusive_zone; // If to automatically change the exclusive zone to match the window size
    GtkLayerShellKeyboardMode keyboard_mode; // Type of keyboard interactivity enabled for this surface
    GtkLayerShellLayer layer; // The current layer, needs surface recreation on old layer shell versions

    // Need the surface to be recreated to change
    GdkMonitor *monitor; // Can be null
    const char *name_space; // Can be null, freed on destruction

    // Not set by user requests
    struct zwlr_layer_surface_v1 *layer_surface; // The actual layer surface Wayland object (can be NULL)
    GtkRequisition current_allocation; // Last size allocation, or (0, 0) if there hasn't been one
    GtkRequisition cached_layer_size; // Last size sent to zwlr_layer_surface_v1_set_size (starts as 0, 0)
    GtkRequisition last_configure_size; // Last size received from a configure event
};

LayerSurface *layer_surface_new (GtkWindow *gtk_window);

// Safe cast, returns NULL if wrong type sent
LayerSurface *custom_shell_surface_get_layer_surface (CustomShellSurface *shell_surface);

// Surface is remapped in order to set
void layer_surface_set_monitor (LayerSurface *self, GdkMonitor *monitor); // Can be null for default
void layer_surface_set_name_space (LayerSurface *self, char const* name_space); // Makes a copy of the string, can be null

// Can be set without remapping the surface
void layer_surface_set_layer (LayerSurface *self, GtkLayerShellLayer layer); // Remaps surface on old layer shell versions
void layer_surface_set_anchor (LayerSurface *self, GtkLayerShellEdge edge, gboolean anchor_to_edge);
void layer_surface_set_margin (LayerSurface *self, GtkLayerShellEdge edge, int margin_size);
void layer_surface_set_exclusive_zone (LayerSurface *self, int exclusive_zone);
void layer_surface_auto_exclusive_zone_enable (LayerSurface *self);
void layer_surface_set_keyboard_mode (LayerSurface *self, GtkLayerShellKeyboardMode mode);

// Returns the effective namespace (default if unset). Does not return ownership. Never returns NULL. Handles null self.
const char* layer_surface_get_namespace (LayerSurface *self);

#endif // LAYER_SHELL_SURFACE_H
