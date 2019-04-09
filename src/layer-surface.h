#ifndef LAYER_SHELL_SURFACE_H
#define LAYER_SHELL_SURFACE_H

#include "custom-shell-surface.h"
#include "protocol/wlr-layer-shell-unstable-v1-client.h"
#include "gtk-layer-shell.h"
#include <gtk/gtk.h>

// a LayerSurface * can be safely cast to a CustomShellSurface *
typedef struct _LayerSurface LayerSurface;

LayerSurface *layer_surface_new (GtkWindow *gtk_window);

// Safe cast, returns NULL if wrong type sent
LayerSurface *custom_shell_surface_get_layer_surface (CustomShellSurface *shell_surface);

// Surface is remapped in order to set
void layer_surface_set_layer (LayerSurface *self, enum zwlr_layer_shell_v1_layer layer);

// Can be set without remapping the surface
uint32_t layer_surface_get_anchor (LayerSurface *self);
void layer_surface_set_anchor (LayerSurface *self, uint32_t anchor);

void layer_surface_set_exclusive_zone (LayerSurface *self, int exclusive_zone);
void layer_surface_auto_exclusive_zone_enable (LayerSurface *self);

void layer_surface_set_keyboard_interactivity (LayerSurface *self, gboolean *interactivity);

#endif // LAYER_SHELL_SURFACE_H
