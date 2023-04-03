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

typedef struct
{
    const struct wl_message* message;
    RequestOverrideFunction function;
    struct wl_list link;
} RequestOverride;

struct wl_list request_overrides;

void install_request_override(const struct wl_interface* interface, const char* name, RequestOverrideFunction function)
{
    for (int i = 0; i < interface->method_count; i++)
    {
        if (strcmp(name, interface->methods[i].name) == 0)
        {
            RequestOverride* override = ALLOC_STRUCT(RequestOverride);
            override->message = &interface->methods[i];
            override->function = function;
            wl_list_insert(&request_overrides, &override->link);
            return;
        }
    }
    FATAL_FMT("Interface %s does not have a request named %s", interface->name, name);
}

static int default_dispatcher(const void* data, void* resource, uint32_t opcode, const struct wl_message* message, union wl_argument* args)
{
    // First, check if there is an override
    RequestOverride* override;
    wl_list_for_each(override, &request_overrides, link)
    {
        if (override->message == message)
        {
            override->function(resource, message, args);
            return 0;
        }
    }

    // If there are any new-id type arguments, resources need to be created for them
    // See https://wayland.freedesktop.org/docs/html/apb.html#Client-structwl__message
    int arg = 0;
    for (const char* c = message->signature; *c; c++)
    {
        if (*c == 'n' && args[arg].n != 0)
        {
            struct wl_resource* new_resource = wl_resource_create(
                wl_resource_get_client(resource),
                message->types[arg],
                wl_resource_get_version(resource),
                args[arg].n);
            wl_resource_set_dispatcher(new_resource, default_dispatcher, NULL, NULL, NULL);
        }
        if (*c >= 'a' && *c <= 'z')
            arg++;
    }
    if (strcmp(message->name, "destroy") == 0)
    {
        wl_resource_destroy(resource);
    }
    return 0;
}

void use_default_impl(struct wl_resource* resource)
{
    wl_resource_set_dispatcher(resource, default_dispatcher, NULL, NULL, NULL);
}

static void default_global_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    struct wl_interface* interface = data;
    struct wl_resource* resource = wl_resource_create(client, interface, version, id);
    use_default_impl(resource);
};

void default_global_create(struct wl_display* display, const struct wl_interface* interface, int version)
{
    wl_global_create(display, interface, version, (void*)interface, default_global_bind);
}

char type_code_at_index(const struct wl_message* message, int index)
{
    int i = 0;
    for (const char* c = message->signature; *c; c++)
    {
        if (*c >= 'a' && *c <= 'z')
        {
            if (i == index)
                return *c;
            else
                i++;
        }
    }
    FATAL_FMT(".%s does not have an argument %d", message->name, index);
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

int main(int argc, const char** argv)
{
    wl_list_init(&request_overrides);

    display = wl_display_create();
    if (wl_display_add_socket(display, get_display_name()) != 0)
    {
        FATAL_FMT("server failed to connect to Wayland display %s", get_display_name());
    }

    wl_display_add_client_created_listener(display, &client_connect_listener);

    init();

    wl_display_run(display);
    wl_display_destroy(display);

    return 0;
}
