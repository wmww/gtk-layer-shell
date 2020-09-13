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

struct wl_display* display = NULL;

void* alloc_zeroed(size_t size)
{
    void* data = malloc(size);
    memset(data, 0, size);
    return data;
}

static const char* get_display_name()
{
    const char* result = getenv("WAYLAND_DISPLAY");
    if (!result)
    {
        FATAL("WAYLAND_DISPLAY not set");
    }
    return result;
}

int ignore_or_destroy_dispatcher(const void* data, void* resource, uint32_t opcode, const struct wl_message* message, union wl_argument* args)
{
    if (strcmp(message[opcode].name, "destroy") == 0)
    {
        wl_resource_destroy(resource);
    }
    return 0;
}

void free_data_destroy_func(struct wl_resource *resource)
{
    void* data = wl_resource_get_user_data(resource);
    free(data);
}

static void client_disconnect(struct wl_listener *listener, void *data)
{
    wl_display_terminate(display);
}

static struct wl_listener client_disconnect_listener = {
    .notify = client_disconnect,
};

static void client_connect(struct wl_listener *listener, void *data)
{
    struct wl_client* client = (struct wl_client*)data;
    wl_client_add_destroy_listener(client, &client_disconnect_listener);
}

static struct wl_listener client_connect_listener = {
    .notify = client_connect,
};

static void wl_output_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    wl_resource_create(client, &wl_output_interface, version, id);
};

int main(int argc, const char** argv)
{
    display = wl_display_create();
    if (wl_display_add_socket(display, get_display_name()) != 0)
    {
        FATAL_FMT("server failed to connect to Wayland display %s", get_display_name());
    }

    wl_display_add_client_created_listener(display, &client_connect_listener);

    wl_global_create(display, &wl_compositor_interface, 4, NULL, wl_compositor_bind);
    wl_global_create(display, &wl_shm_interface, 1, NULL, wl_shm_bind);
    wl_global_create(display, &wl_seat_interface, 6, NULL, wl_seat_bind);
    wl_global_create(display, &wl_output_interface, 2, NULL, wl_output_bind);
    wl_global_create(display, &xdg_wm_base_interface, 2, NULL, xdg_wm_base_bind);

    wl_display_run(display);

    wl_display_destroy(display);

    return 0;
}
