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

// This is an implementation of a mock Wayland compositor for testing
// It does not show anything on the screen, and is only as conforment as is required by GTK

#include <wayland-server.h>
#include "xdg-shell-server.h"
#include "wlr-layer-shell-unstable-v1-server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static struct wl_display* display = NULL;
static struct wl_resource* compositor = NULL;
static struct wl_resource* seat = NULL;
static struct wl_resource* pointer = NULL;
static struct wl_resource* output = NULL;

#define FATAL_FMT(format, ...) fprintf(stderr, "Fatal error: " format "\n", ##__VA_ARGS__); exit(1)
#define FATAL(message) FATAL_FMT(message"%s", "")
#define FATAL_NOT_IMPL FATAL_FMT("%s() not implemented", __func__); exit(1)

static const char* get_display_name()
{
    const char* result = getenv("WAYLAND_DISPLAY");
    if (!result)
    {
        FATAL("WAYLAND_DISPLAY not set");
    }
    return result;
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

static void compositor_create_surface(struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
    if (!output)
    {
        fprintf(stderr, "Fatal: client should not have created surface without binding to output"); exit(1);
    }
    struct wl_resource* surface = wl_resource_create(
        client,
        &wl_surface_interface,
        wl_resource_get_version(resource),
        id);
    wl_surface_send_enter(surface, output);
}

static void compositor_create_region(struct wl_client * client, struct wl_resource * resource, uint32_t id)
{
    FATAL_NOT_IMPL;
}

static const struct wl_compositor_interface compositor_interface = {
    .create_surface = compositor_create_surface,
    .create_region = compositor_create_region,
};

static void compositor_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    compositor = wl_resource_create(client, &wl_compositor_interface, version, id);
    wl_resource_set_implementation(compositor, &compositor_interface, NULL, NULL);
};

void seat_get_pointer(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    if (pointer)
    {
        FATAL("multiple pointers created");
    }
    pointer = wl_resource_create(
        client,
        &wl_pointer_interface,
        wl_resource_get_version(resource),
        id);
}

void seat_get_keyboard(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    FATAL_NOT_IMPL;
}

void seat_get_touch(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    FATAL_NOT_IMPL;
}

void seat_release(struct wl_client *client, struct wl_resource *resource)
{
    FATAL_NOT_IMPL;
}

static const struct wl_seat_interface seat_interface = {
    .get_pointer = seat_get_pointer,
    .get_keyboard = seat_get_keyboard,
    .get_touch = seat_get_touch,
    .release = seat_release,
};

static void seat_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    seat = wl_resource_create(client, &wl_seat_interface, version, id);
    wl_resource_set_implementation(seat, &seat_interface, NULL, NULL);
    wl_seat_send_capabilities(seat, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
};

static void output_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    output = wl_resource_create(client, &wl_output_interface, version, id);
};

int main(int argc, const char** argv)
{
    display = wl_display_create();
    if (wl_display_add_socket(display, get_display_name()) != 0)
    {
        FATAL_FMT("server failed to connect to Wayland display %s", get_display_name());
    }

    wl_display_add_client_created_listener(display, &client_connect_listener);

    wl_global_create(display, &wl_compositor_interface, 4, NULL, compositor_bind);
    wl_global_create(display, &wl_seat_interface, 6, NULL, seat_bind);
    wl_global_create(display, &wl_output_interface, 1, NULL, output_bind);

    wl_display_run(display);

    wl_display_destroy(display);

    return 0;
}
