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

static struct wl_resource* pointer = NULL;

void wl_seat_get_pointer(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    ASSERT(!pointer);
    pointer = wl_resource_create(
        client,
        &wl_pointer_interface,
        wl_resource_get_version(resource),
        id);
}

void wl_seat_get_keyboard(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    FATAL_NOT_IMPL;
}

void wl_seat_get_touch(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    FATAL_NOT_IMPL;
}

void wl_seat_release(struct wl_client *client, struct wl_resource *resource)
{
    FATAL_NOT_IMPL;
}

static const struct wl_seat_interface wl_seat_impl = {
    .get_pointer = wl_seat_get_pointer,
    .get_keyboard = wl_seat_get_keyboard,
    .get_touch = wl_seat_get_touch,
    .release = wl_seat_release,
};

void wl_seat_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    struct wl_resource* seat = wl_resource_create(client, &wl_seat_interface, version, id);
    wl_resource_set_implementation(seat, &wl_seat_impl, NULL, NULL);
    //wl_seat_send_capabilities(seat, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
    wl_seat_send_capabilities(seat, 0);
};


void wl_data_device_manager_create_data_source(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    FATAL_NOT_IMPL;
}

void wl_data_device_manager_get_data_device(
    struct wl_client *client,
    struct wl_resource *resource,
    uint32_t id,
    struct wl_resource *seat)
{
    struct wl_resource* data_device = wl_resource_create(
        client,
        &wl_data_device_interface,
        wl_resource_get_version(resource),
        id);
    wl_resource_set_dispatcher(data_device, ignore_or_destroy_dispatcher, NULL, NULL, NULL);
}

static const struct wl_data_device_manager_interface data_device_manager_interface = {
    .create_data_source = wl_data_device_manager_create_data_source,
    .get_data_device = wl_data_device_manager_get_data_device,
};

void wl_data_device_manager_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    struct wl_resource* data_device_manager = wl_resource_create(client, &wl_data_device_manager_interface, version, id);
    wl_resource_set_implementation(data_device_manager, &data_device_manager_interface, NULL, NULL);
};
