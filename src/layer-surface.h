#ifndef LAYER_SHELL_SURFACE_H
#define LAYER_SHELL_SURFACE_H

#include "custom-shell-surface.h"
#include <gtk/gtk.h>

// a LayerSurface * can be safely cast to a CustomShellSurface *
typedef struct _LayerSurface LayerSurface;

LayerSurface *layer_surface_new (GtkWindow *gtk_window);

// Safe cast, returns NULL if wrong type sent
LayerSurface *custom_shell_surface_get_layer_surface (CustomShellSurface *shell_surface);

void layer_surface_set_anchor (LayerSurface *self, gboolean left, gboolean right, gboolean top, gboolean bottom);
void layer_surface_set_exclusive_zone (LayerSurface *self, int exclusive_zone);

#endif // LAYER_SHELL_SURFACE_H
