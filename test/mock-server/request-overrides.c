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
#include "linux/input.h"

typedef struct
{
    struct wl_resource* surface;
    struct wl_resource* pending_frame;
    struct wl_resource* xdg_toplevel;
    struct wl_resource* xdg_popup;
    struct wl_resource* xdg_surface;
    struct wl_resource* layer_surface;
    char layer_send_configure;
    int layer_set_w;
    int layer_set_h;
    uint32_t layer_anchor;
} SurfaceData;

static struct wl_resource* seat_global = NULL;
static struct wl_resource* pointer_global = NULL;
static uint32_t click_serial = 0;

static void surface_data_assert_has_one_role(SurfaceData* data)
{
    int role_count = 0;
    char roles[200];
    roles[0] = 0;
    if (data->xdg_surface && data->xdg_toplevel)
    {
        role_count++;
        strcat(roles, "xdg_toplevel ");
    }
    if (data->xdg_surface && data->xdg_popup)
    {
        role_count++;
        strcat(roles, "xdg_popup");
    }
    if (data->layer_surface)
    {
        role_count++;
        strcat(roles, "layer_surface");
    }
    if (role_count != 1)
    {
        FATAL_FMT("wl_surface@%d has %d roles: %s", wl_resource_get_id(data->surface), role_count, roles);
    }
}

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
    if (data->layer_surface && data->layer_send_configure)
    {
        char horiz = (
            (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT) &&
            (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT));
        char vert = (
            (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP) &&
            (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM));
        int width = data->layer_set_w;
        int height = data->layer_set_h;
        if (horiz)
            width = OUTPUT_WIDTH;
        if (vert)
            height = OUTPUT_HEIGHT;
        zwlr_layer_surface_v1_send_configure(data->layer_surface, wl_display_next_serial(display), width, height);
        data->layer_send_configure = 0;

        // Move the pointer onto the surface and click
        // This is needed to trigger a tooltip or popup menu to open for the popup tests
        ASSERT(pointer_global);
        wl_pointer_send_enter(
            pointer_global,
            wl_display_next_serial(display),
            data->surface,
            wl_fixed_from_double(5.0), wl_fixed_from_double(5.0));
        wl_pointer_send_frame(pointer_global);
        click_serial = wl_display_next_serial(display);
        wl_pointer_send_button(
            pointer_global,
            click_serial, 0,
            BTN_LEFT, WL_POINTER_BUTTON_STATE_PRESSED);
        wl_pointer_send_frame(pointer_global);
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
    data->surface = surface;
    use_default_impl(surface);
    wl_resource_set_user_data(surface, data);
    wl_resource_set_destructor(surface, free_data_destroy_func);
}

void wl_seat_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    seat_global = wl_resource_create(client, &wl_seat_interface, version, id);
    use_default_impl(seat_global);
    wl_seat_send_capabilities(seat_global, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
};

static void wl_seat_get_pointer(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    NEW_ID_ARG(id, 0);
    ASSERT(!pointer_global);
    pointer_global = wl_resource_create(
        wl_resource_get_client(resource),
        &wl_pointer_interface,
        wl_resource_get_version(resource),
        id);
    use_default_impl(pointer_global);
}

static void xdg_wm_base_get_xdg_surface(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    NEW_ID_ARG(id, 0);
    RESOURCE_ARG(wl_surface, surface, 1);
    struct wl_resource* xdg_surface = wl_resource_create(
        wl_resource_get_client(resource),
        &xdg_surface_interface,
        wl_resource_get_version(resource),
        id);
    use_default_impl(xdg_surface);
    SurfaceData* data = wl_resource_get_user_data(surface);
    data->xdg_surface = xdg_surface;
    wl_resource_set_user_data(xdg_surface, data);
}

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
    SurfaceData* data = wl_resource_get_user_data(resource);
    wl_resource_set_user_data(toplevel, data);
    data->xdg_toplevel = toplevel;
    surface_data_assert_has_one_role(data);
}

static void xdg_popup_grab(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    RESOURCE_ARG(wl_seat, seat, 0);
    UINT_ARG(serial, 1);
    ASSERT_EQ(seat, seat_global, "%p");
    ASSERT_EQ(serial, click_serial, "%u");
}

static void zwlr_layer_surface_v1_set_anchor(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    UINT_ARG(anchor, 0);
    SurfaceData* data = wl_resource_get_user_data(resource);
    data->layer_send_configure = 1;
    data->layer_anchor = anchor;
}

static void zwlr_layer_surface_v1_set_size(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    UINT_ARG(width, 0);
    UINT_ARG(height, 1);
    SurfaceData* data = wl_resource_get_user_data(resource);
    data->layer_send_configure = 1;
    data->layer_set_w = width;
    data->layer_set_h = height;
}

static void zwlr_layer_shell_v1_get_layer_surface(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    NEW_ID_ARG(id, 0);
    RESOURCE_ARG(wl_surface, surface, 1);
    struct wl_resource* layer_surface = wl_resource_create(
        wl_resource_get_client(resource),
        &zwlr_layer_surface_v1_interface,
        wl_resource_get_version(resource),
        id);
    use_default_impl(layer_surface);
    SurfaceData* data = wl_resource_get_user_data(surface);
    wl_resource_set_user_data(layer_surface, data);
    data->layer_send_configure = 1;
    data->layer_surface = layer_surface;
    surface_data_assert_has_one_role(data);
}

void install_overrides()
{
    OVERRIDE_REQUEST(wl_surface, commit);
    OVERRIDE_REQUEST(wl_surface, frame);
    OVERRIDE_REQUEST(wl_compositor, create_surface);
    OVERRIDE_REQUEST(wl_seat, get_pointer);
    OVERRIDE_REQUEST(xdg_wm_base, get_xdg_surface);
    OVERRIDE_REQUEST(xdg_surface, get_toplevel);
    OVERRIDE_REQUEST(xdg_popup, grab);
    OVERRIDE_REQUEST(zwlr_layer_shell_v1, get_layer_surface);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, set_anchor);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, set_size);
}
