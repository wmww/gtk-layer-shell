/* This entire file is licensed under MIT
 *
 * Copyright 2020 William Wold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mock-server.h"

static void xdg_surface_destroy(struct wl_client *client, struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static void xdg_surface_get_toplevel(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    struct wl_resource* toplevel = wl_resource_create(client, &xdg_toplevel_interface, wl_resource_get_version(resource), id);
    use_default_impl(toplevel);
    struct wl_array states;
    wl_array_init(&states);
    xdg_toplevel_send_configure(toplevel, 0, 0, &states);
    wl_array_release(&states);
    xdg_surface_send_configure(resource, wl_display_next_serial(display));
}

static void xdg_surface_get_popup(struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *parent, struct wl_resource *positioner)
{
    FATAL_NOT_IMPL;
}

static void xdg_surface_set_window_geometry(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height)
{
    // Ignore
}

static void xdg_surface_ack_configure(struct wl_client *client, struct wl_resource *resource, uint32_t serial)
{
    // Ignore
}

static const struct xdg_surface_interface xdg_surface_impl = {
    .destroy = xdg_surface_destroy,
    .get_toplevel = xdg_surface_get_toplevel,
    .get_popup = xdg_surface_get_popup,
    .set_window_geometry = xdg_surface_set_window_geometry,
    .ack_configure = xdg_surface_ack_configure,
};

static void xdg_wm_base_destroy(struct wl_client *client, struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static void xdg_wm_base_create_positioner(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    FATAL_NOT_IMPL;
}

static void xdg_wm_base_get_xdg_surface(struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface)
{
    struct wl_resource* xdg_surface = wl_resource_create(
        client,
        &xdg_surface_interface,
        wl_resource_get_version(resource),
        id);
    wl_resource_set_implementation(xdg_surface, &xdg_surface_impl, NULL, NULL);
}

static void xdg_wm_base_pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial)
{
    // ignore
}

static const struct xdg_wm_base_interface xdg_wm_base_impl = {
    .destroy = xdg_wm_base_destroy,
    .create_positioner = xdg_wm_base_create_positioner,
    .get_xdg_surface = xdg_wm_base_get_xdg_surface,
    .pong = xdg_wm_base_pong,
};

void xdg_wm_base_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    struct wl_resource* xdg_wm_base = wl_resource_create(client, &xdg_wm_base_interface, version, id);
    wl_resource_set_implementation(xdg_wm_base, &xdg_wm_base_impl, NULL, NULL);
};
