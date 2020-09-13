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

static void wl_surface_destroy(struct wl_client *client, struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static void wl_surface_attach(struct wl_client *client, struct wl_resource *resource, struct wl_resource *buffer, int32_t x, int32_t y)
{
    // Ignore
}

static void wl_surface_damage(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height)
{
    // Ignore
}

static void wl_surface_frame(struct wl_client *client, struct wl_resource *resource, uint32_t callback)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    ASSERT(!data->pending_frame);
    data->pending_frame = wl_resource_create(
        client,
        &wl_callback_interface,
        wl_resource_get_version(resource),
        callback);
}

static void wl_surface_set_opaque_region(struct wl_client *client, struct wl_resource *resource, struct wl_resource *region)
{
    // Ignore
}

static void wl_surface_set_input_region(struct wl_client *client, struct wl_resource *resource, struct wl_resource *region)
{
    // Ignore
}

static void wl_surface_commit(struct wl_client *client, struct wl_resource *resource)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    if (data->pending_frame)
    {
        wl_callback_send_done(data->pending_frame, 0);
        wl_resource_destroy(data->pending_frame);
        data->pending_frame = NULL;
    }
}

static void wl_surface_set_buffer_transform(struct wl_client *client, struct wl_resource *resource, int32_t transform)
{
    // Ignore
}

static void wl_surface_set_buffer_scale(struct wl_client *client, struct wl_resource *resource, int32_t scale)
{
    // Ignore
}

static void wl_surface_damage_buffer(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height)
{
    // Ignore
}

static const struct wl_surface_interface wl_surface_impl = {
    .destroy = wl_surface_destroy,
    .attach = wl_surface_attach,
    .damage = wl_surface_damage,
    .frame = wl_surface_frame,
    .set_opaque_region = wl_surface_set_opaque_region,
    .set_input_region = wl_surface_set_input_region,
    .commit = wl_surface_commit,
    .set_buffer_transform = wl_surface_set_buffer_transform,
    .set_buffer_scale = wl_surface_set_buffer_scale,
    .damage_buffer = wl_surface_damage_buffer,
};

static void wl_compositor_create_surface(struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
    struct wl_resource* surface = wl_resource_create(
        client,
        &wl_surface_interface,
        wl_resource_get_version(resource),
        id);
    SurfaceData* data = alloc_struct(SurfaceData);
    wl_resource_set_implementation(surface, &wl_surface_impl, data, free_data_destroy_func);
}

static void wl_compositor_create_region(struct wl_client * client, struct wl_resource * resource, uint32_t id)
{
    struct wl_resource* region = wl_resource_create(
        client,
        &wl_region_interface,
        wl_resource_get_version(resource),
        id);
    wl_resource_set_dispatcher(region, ignore_or_destroy_dispatcher, NULL, NULL, NULL);
}

static const struct wl_compositor_interface wl_compositor_impl = {
    .create_surface = wl_compositor_create_surface,
    .create_region = wl_compositor_create_region,
};

void wl_compositor_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    struct wl_resource* compositor = wl_resource_create(client, &wl_compositor_interface, version, id);
    wl_resource_set_implementation(compositor, &wl_compositor_impl, NULL, NULL);
}
