#ifndef LAYER_SHELL_SURFACE_H
#define LAYER_SHELL_SURFACE_H

#include "custom-shell-surface.h"
#include "wlr-layer-shell-unstable-v1-client.h"
#include "gtk-layer-shell.h"
#include <gtk/gtk.h>

// a LayerSurface * can be safely cast to a CustomShellSurface *
typedef struct _LayerSurface LayerSurface;

LayerSurface *layer_surface_new (GtkWindow *gtk_window);

// Safe cast, returns NULL if wrong type sent
LayerSurface *custom_shell_surface_get_layer_surface (CustomShellSurface *shell_surface);

// Surface is remapped in order to set
void layer_surface_set_layer (LayerSurface *self, enum zwlr_layer_shell_v1_layer layer);
void layer_surface_set_monitor (LayerSurface *self, GdkMonitor *monitor); // Can be null for default
void layer_surface_set_name_space (LayerSurface *self, char const* name_space); // Makes a copy of the string, can be null

// Can be set without remapping the surface
void layer_surface_set_anchor (LayerSurface *self, GtkLayerShellEdge edge, gboolean anchor_to_edge);
void layer_surface_set_margin (LayerSurface *self, GtkLayerShellEdge edge, int margin_size);
void layer_surface_set_exclusive_zone (LayerSurface *self, int exclusive_zone);
void layer_surface_auto_exclusive_zone_enable (LayerSurface *self);

void layer_surface_set_keyboard_interactivity (LayerSurface *self, gboolean interactivity);

#endif // LAYER_SHELL_SURFACE_H
