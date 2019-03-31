#include "custom-shell-surface.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>

static const char *custom_shell_surface_key = "wayland_custom_shell_surface";

struct _CustomShellSurfacePrivate
{
    GtkWindow *gtk_window;
};

static void
custom_shell_surface_on_window_destroy (CustomShellSurface *self)
{
    self->virtual->finalize (self);
    g_free (self->private);
    g_free (self);
}

static void
custom_shell_surface_on_window_realize (GtkWindow *gtk_window, CustomShellSurface *self)
{
    GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (gtk_window));
    g_return_if_fail (gdk_window);

    struct wl_surface *wl_surface = gdk_wayland_window_get_wl_surface (gdk_window);
    g_return_if_fail (wl_surface);

    gdk_wayland_window_set_use_custom_surface (gdk_window);

    self->virtual->realize (self, wl_surface);

    /*
    if (layer_shell_global) {
        self->layer_surface = zwlr_layer_shell_v1_get_layer_surface (layer_shell_global,
                                         wl_surface,
                                         output,
                                         layer,
                                         name_space);
        g_return_val_if_fail (self->layer_surface, NULL);
        zwlr_layer_surface_v1_set_keyboard_interactivity (self->layer_surface, FALSE);
        zwlr_layer_surface_v1_set_anchor (self->layer_surface, ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT);
        zwlr_layer_surface_v1_add_listener (self->layer_surface, &layer_surface_listener, self);
    } else if (xdg_wm_base_global) {
        g_warning ("Layer Shell Wayland protocol not supported, panel will not be placed correctly");
        self->xdg_surface = xdg_wm_base_get_xdg_surface (xdg_wm_base_global, wl_surface);
        self->xdg_toplevel = xdg_surface_get_toplevel (self->xdg_surface);
        xdg_surface_add_listener (self->xdg_surface, &xdg_surface_listener, self);
    } else {
        g_warning ("Neither Layer Shell or XDG shell stable Wayland protocols detected, panel can not be drawn");
        return NULL;
    }
    wayland_shell_surface_set_size (self,
                    gtk_widget_get_allocated_width (GTK_WIDGET (gtk_window)),
                    gtk_widget_get_allocated_height (GTK_WIDGET (gtk_window)));
    */
    wl_surface_commit (wl_surface);
    wl_display_roundtrip (gdk_wayland_display_get_wl_display (gdk_window_get_display (gdk_window)));
}

void
custom_shell_surface_init (CustomShellSurface *self, GtkWindow *gtk_window)
{
    g_assert (self->virtual); // Subclass should have set this up first

    self->private = g_new0 (CustomShellSurfacePrivate, 1);
    self->private->gtk_window = gtk_window;

    g_return_if_fail (gtk_window);
    g_return_if_fail (!gtk_widget_get_realized (GTK_WIDGET (gtk_window)));
    gtk_window_set_decorated (gtk_window, FALSE);
    g_object_set_data_full (G_OBJECT (gtk_window),
                            custom_shell_surface_key,
                            self,
                            (GDestroyNotify) custom_shell_surface_on_window_destroy);
    g_signal_connect (gtk_window, "realize", G_CALLBACK (custom_shell_surface_on_window_realize), self);
}

CustomShellSurface *
gtk_window_get_custom_shell_surface (GtkWindow *gtk_window)
{
    return g_object_get_data (G_OBJECT (gtk_window), custom_shell_surface_key);
}
