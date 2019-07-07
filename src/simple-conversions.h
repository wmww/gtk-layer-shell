#ifndef SIMPLE_CONVERSIONS_H
#define SIMPLE_CONVERSIONS_H

#include "xdg-shell-client.h"
#include "wlr-layer-shell-unstable-v1-client.h"
#include "gtk-layer-shell.h"
#include <gdk/gdk.h>

enum zwlr_layer_shell_v1_layer gtk_layer_shell_layer_get_zwlr_layer_shell_v1_layer (GtkLayerShellLayer layer);
uint32_t gtk_layer_shell_edge_array_get_zwlr_layer_shell_v1_anchor (gboolean edges[GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER]);
enum xdg_positioner_gravity gdk_gravity_get_xdg_positioner_gravity (GdkGravity gravity);
enum xdg_positioner_anchor gdk_gravity_get_xdg_positioner_anchor (GdkGravity anchor);
enum xdg_positioner_constraint_adjustment gdk_anchor_hints_get_xdg_positioner_constraint_adjustment (GdkAnchorHints hints);

#endif // SIMPLE_CONVERSIONS_H
