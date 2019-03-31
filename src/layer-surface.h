#ifndef LAYER_SHELL_SURFACE_H
#define LAYER_SHELL_SURFACE_H

#include "custom-shell-surface.h"

// a LayerSurface * can be safely cast to a CustomShellSurface *
typedef struct _LayerSurface LayerSurface;

LayerSurface *layer_surface_new (GtkWindow *gtk_window);

#endif // LAYER_SHELL_SURFACE_H
