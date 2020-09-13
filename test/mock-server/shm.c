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

static void wl_shm_pool_create_buffer(struct wl_client *client, struct wl_resource *resource, uint32_t id, int32_t offset, int32_t width, int32_t height, int32_t stride, uint32_t format)
{
    struct wl_resource* buffer = wl_resource_create(client, &wl_buffer_interface, wl_resource_get_version(resource), id);
    wl_resource_set_dispatcher(buffer, ignore_or_destroy_dispatcher, NULL, NULL, NULL);
}

static void wl_shm_pool_destroy(struct wl_client *client, struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static void wl_shm_pool_resize(struct wl_client *client, struct wl_resource *resource, int32_t size)
{
    // Ignore
}

static const struct wl_shm_pool_interface wl_shm_pool_impl = {
    .create_buffer = wl_shm_pool_create_buffer,
    .destroy = wl_shm_pool_destroy,
    .resize = wl_shm_pool_resize,
};

static void wl_shm_create_pool(struct wl_client *client, struct wl_resource *resource, uint32_t id, int32_t fd, int32_t size)
{
    struct wl_resource* shm_pool = wl_resource_create(
        client,
        &wl_shm_pool_interface,
        wl_resource_get_version(resource),
        id);
    wl_resource_set_implementation(shm_pool, &wl_shm_pool_impl, NULL, NULL);
}

static const struct wl_shm_interface wl_shm_impl = {
    .create_pool = wl_shm_create_pool,
};

void wl_shm_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    struct wl_resource* shm = wl_resource_create(client, &wl_shm_interface, version, id);
    wl_resource_set_implementation(shm, &wl_shm_impl, NULL, NULL);
}
