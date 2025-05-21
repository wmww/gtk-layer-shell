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

#include "custom-shell-surface.h"
#include "gtk-wayland.h"
#include "gtk-priv-access.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>

static const char *custom_shell_surface_key = "wayland_custom_shell_surface";

struct _CustomShellSurfacePrivate
{
    GtkWindow *gtk_window;
    CustomShellSurface *popup_parent;
    GList *popup_children;
};

static void
custom_shell_surface_on_window_destroy (CustomShellSurface *self)
{
    self->virtual->finalize (self);

    if (self->private->popup_parent) {
        g_warning ("Shell surface has popup parent on finalize (should have been cleared by unmap)");
        struct _CustomShellSurfacePrivate *parent_private = self->private->popup_parent->private;
        parent_private->popup_children = g_list_remove(parent_private->popup_children, self);
    }

    while (self->private->popup_children) {
        g_warning ("Shell surface has popup children on finalize (should have been cleared by unmap)");
        CustomShellSurface *child_private = self->private->popup_children->data;
        child_private->private->popup_parent = NULL;
        self->private->popup_children = g_list_remove_link (self->private->popup_children,
                                                            self->private->popup_children);
    }

    g_free (self->private);
    g_free (self);
}

static void
custom_shell_surface_on_window_realize (GtkWidget *widget, CustomShellSurface *self)
{
    g_return_if_fail (GTK_WIDGET (self->private->gtk_window) == widget);

    GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (self->private->gtk_window));
    g_return_if_fail (gdk_window);

    gtk_priv_access_init (gdk_window);
    gdk_wayland_window_set_use_custom_surface (gdk_window);
}

static void
custom_shell_surface_on_window_map (GtkWidget *widget, CustomShellSurface *self)
{
    g_return_if_fail (GTK_WIDGET (self->private->gtk_window) == widget);

    GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (self->private->gtk_window));
    g_return_if_fail (gdk_window);

    struct wl_surface *wl_surface = gdk_wayland_window_get_wl_surface (gdk_window);
    g_return_if_fail (wl_surface);

    // In some cases (observed when a mate panel has an image background) GDK will attach a buffer just after creating
    // the surface (see the implementation of gdk_wayland_window_show() for details). Giving the surface a role with a
    // buffer attached is a protocol violation, so we attach a null buffer. GDK hasn't committed the buffer it may have
    // attached, so we don't need to commit. If this is removed, test-window-with-initially-attached-buffer should fail.
    wl_surface_attach (wl_surface, NULL, 0, 0);

    self->configured = FALSE;
    self->virtual->map (self, wl_surface);
    gdk_window_set_priv_mapped (gdk_window);

    wl_surface_commit (wl_surface);

    struct wl_display *display = gdk_wayland_display_get_wl_display (gdk_display_get_default ());
    while (!self->configured) {
        wl_display_roundtrip (display);
    }
}

void
custom_shell_surface_init (CustomShellSurface *self, GtkWindow *gtk_window)
{
    g_assert (self->virtual); // Subclass should have set this up first

    self->private = g_new0 (CustomShellSurfacePrivate, 1);
    self->private->gtk_window = gtk_window;

    g_return_if_fail (gtk_window);
    g_return_if_fail (!gtk_widget_get_mapped (GTK_WIDGET (gtk_window)));
    g_object_set_data_full (G_OBJECT (gtk_window),
                            custom_shell_surface_key,
                            self,
                            (GDestroyNotify) custom_shell_surface_on_window_destroy);
    g_signal_connect (gtk_window, "realize", G_CALLBACK (custom_shell_surface_on_window_realize), self);
    g_signal_connect (gtk_window, "map", G_CALLBACK (custom_shell_surface_on_window_map), self);

    if (gtk_widget_get_realized (GTK_WIDGET (gtk_window))) {
        // We must be in the process of realizing now
        custom_shell_surface_on_window_realize (GTK_WIDGET (gtk_window), self);
    }
}

CustomShellSurface *
gtk_window_get_custom_shell_surface (GtkWindow *gtk_window)
{
    if (!gtk_window)
        return NULL;

    return g_object_get_data (G_OBJECT (gtk_window), custom_shell_surface_key);
}

GtkWindow *
custom_shell_surface_get_gtk_window (CustomShellSurface *self)
{
    g_return_val_if_fail (self, NULL);
    return self->private->gtk_window;
}

void
custom_shell_surface_needs_commit (CustomShellSurface *self)
{
    if (!self->private->gtk_window)
        return;

    GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (self->private->gtk_window));

    if (!gdk_window)
        return;

    // Hopefully this will trigger a commit
    // Don't commit directly, as that screws up GTK's internal state
    // (see https://github.com/wmww/gtk-layer-shell/issues/51)
    gdk_window_invalidate_rect (gdk_window, NULL, FALSE);
}

void
custom_shell_surface_force_commit (CustomShellSurface *self)
{
    if (!self->private->gtk_window)
        return;

    GdkWindow *gdk_window = gtk_widget_get_window (GTK_WIDGET (self->private->gtk_window));

    if (!gdk_window || gdk_window_get_priv_pending_commit (gdk_window))
        return;

    struct wl_surface *wl_surface = gdk_wayland_window_get_wl_surface (gdk_window);

    if (!wl_surface)
        return;

    wl_surface_commit (wl_surface);
}

void
custom_shell_surface_remap (CustomShellSurface *self)
{
    GdkDisplay *gdk_display = gdk_display_get_default ();
    if (gdk_display_get_n_monitors (gdk_display) == 0) {
        // GTK will exit if you try to map a window while there are no monitors, so don't do that
        return;
    }
    GtkWidget *window_widget = GTK_WIDGET (self->private->gtk_window);
    g_return_if_fail (window_widget);
    gtk_widget_hide (window_widget);
    gtk_widget_show (window_widget);
}

// Calls virtual->get_popup and adds the surface to the list of popups
struct xdg_popup* custom_shell_surface_add_popup (CustomShellSurface *self,
                                                  CustomShellSurface *popup,
                                                  struct xdg_surface *popup_xdg_surface,
                                                  struct xdg_positioner *positioner)
{
    if (g_list_find (self->private->popup_children, popup)) {
        g_warning ("Popup added to shell surface multiple times");
    } else {
        self->private->popup_children = g_list_append (self->private->popup_children, popup);
    }
    popup->private->popup_parent = self;
    return self->virtual->get_popup (self, popup_xdg_surface, positioner);
}

// Unamps all popups and then calls virtual->unmap()
void custom_shell_surface_unmap (CustomShellSurface *self)
{
    // Since we have to unmap *before* the default GTK handler (so we delete our objects before the wl_surface gets
    // deleted), we also unmap before the GTK logic kicks in that unmaps children before parents. That means we have to
    // handle that ourselves.
    GList* local_children = self->private->popup_children;
    self->private->popup_children = NULL;
    while (local_children) {
        custom_shell_surface_unmap (local_children->data);
        local_children = g_list_remove_link (local_children, local_children);
    }
    if (self->private->popup_parent) {
        struct _CustomShellSurfacePrivate *parent_private = self->private->popup_parent->private;
        parent_private->popup_children = g_list_remove (parent_private->popup_children, self);
        self->private->popup_parent = NULL;
    }
    self->virtual->unmap (self);
}
