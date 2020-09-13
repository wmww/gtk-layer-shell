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

static void zwlr_layer_surface_v1_set_size(struct wl_client *client, struct wl_resource *resource, uint32_t width, uint32_t height)
{
    // Ignore
}

static void zwlr_layer_surface_v1_set_anchor(struct wl_client *client, struct wl_resource *resource, uint32_t anchor)
{
    // Ignore
}

static void zwlr_layer_surface_v1_set_exclusive_zone(struct wl_client *client, struct wl_resource *resource, int32_t zone)
{
    // Ignore
}

static void zwlr_layer_surface_v1_set_margin(struct wl_client *client, struct wl_resource *resource, int32_t top, int32_t right, int32_t bottom, int32_t left)
{
    // Ignore
}

static void zwlr_layer_surface_v1_set_keyboard_interactivity(struct wl_client *client, struct wl_resource *resource, uint32_t keyboard_interactivity)
{
    // Ignore
}

static void zwlr_layer_surface_v1_get_popup(struct wl_client *client, struct wl_resource *resource, struct wl_resource *popup)
{
    // Ignore
}

static void zwlr_layer_surface_v1_ack_configure(struct wl_client *client, struct wl_resource *resource, uint32_t serial)
{
    // Ignore
}

static void zwlr_layer_surface_v1_destroy(struct wl_client *client, struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static void zwlr_layer_surface_v1_set_layer(struct wl_client *client, struct wl_resource *resource, uint32_t layer)
{
    // Ignore
}

static const struct zwlr_layer_surface_v1_interface zwlr_layer_surface_v1_impl = {
    .set_size = zwlr_layer_surface_v1_set_size,
    .set_anchor = zwlr_layer_surface_v1_set_anchor,
    .set_exclusive_zone = zwlr_layer_surface_v1_set_exclusive_zone,
    .set_margin = zwlr_layer_surface_v1_set_margin,
    .set_keyboard_interactivity = zwlr_layer_surface_v1_set_keyboard_interactivity,
    .get_popup = zwlr_layer_surface_v1_get_popup,
    .ack_configure = zwlr_layer_surface_v1_ack_configure,
    .destroy = zwlr_layer_surface_v1_destroy,
    .set_layer = zwlr_layer_surface_v1_set_layer,
};

static void zwlr_layer_shell_v1_get_layer_surface(struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface, struct wl_resource *output, uint32_t layer, const char *namespace)
{
    struct wl_resource* layer_surface = wl_resource_create(
        client,
        &zwlr_layer_surface_v1_interface,
        wl_resource_get_version(resource),
        id);
    wl_resource_set_implementation(layer_surface, &zwlr_layer_surface_v1_impl, NULL, NULL);
}

static void zwlr_layer_shell_v1_destroy(struct wl_client *client, struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static const struct zwlr_layer_shell_v1_interface zwlr_layer_shell_v1_impl = {
    .get_layer_surface = zwlr_layer_shell_v1_get_layer_surface,
    .destroy = zwlr_layer_shell_v1_destroy,
};

void zwlr_layer_shell_v1_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    struct wl_resource* layer_shell = wl_resource_create(client, &zwlr_layer_shell_v1_interface, version, id);
    wl_resource_set_implementation(layer_shell, &zwlr_layer_shell_v1_impl, NULL, NULL);
};
