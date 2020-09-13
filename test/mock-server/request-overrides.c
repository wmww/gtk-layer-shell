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

static void wl_surface_frame(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    NEW_ID_ARG(callback, 0);
    SurfaceData* data = wl_resource_get_user_data(resource);
    ASSERT(!data->pending_frame);
    data->pending_frame = wl_resource_create(
        wl_resource_get_client(resource),
        &wl_callback_interface,
        wl_resource_get_version(resource),
        callback);
}

static void wl_surface_commit(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    if (data->pending_frame)
    {
        wl_callback_send_done(data->pending_frame, 0);
        wl_resource_destroy(data->pending_frame);
        data->pending_frame = NULL;
    }
}

static void wl_compositor_create_surface(struct wl_resource* resource, const struct wl_message* message, union wl_argument* args)
{
    NEW_ID_ARG(id, 0);
    struct wl_resource* surface = wl_resource_create(
        wl_resource_get_client(resource),
        &wl_surface_interface,
        wl_resource_get_version(resource),
        id);
    SurfaceData* data = ALLOC_STRUCT(SurfaceData);
    use_default_impl(surface);
    wl_resource_set_user_data(surface, data);
    wl_resource_set_destructor(surface, free_data_destroy_func);
}

void wl_seat_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    struct wl_resource* seat = wl_resource_create(client, &wl_seat_interface, version, id);
    use_default_impl(seat);
    wl_seat_send_capabilities(seat, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
};

static void xdg_surface_get_toplevel(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    NEW_ID_ARG(id, 0);
    struct wl_resource* toplevel = wl_resource_create(
        wl_resource_get_client(resource),
        &xdg_toplevel_interface,
        wl_resource_get_version(resource),
        id);
    use_default_impl(toplevel);
    struct wl_array states;
    wl_array_init(&states);
    xdg_toplevel_send_configure(toplevel, 0, 0, &states);
    wl_array_release(&states);
    xdg_surface_send_configure(resource, wl_display_next_serial(display));
}

void install_overrides()
{
    OVERRIDE_REQUEST(wl_surface, commit);
    OVERRIDE_REQUEST(wl_surface, frame);
    OVERRIDE_REQUEST(wl_compositor, create_surface);
    OVERRIDE_REQUEST(xdg_surface, get_toplevel);
}
