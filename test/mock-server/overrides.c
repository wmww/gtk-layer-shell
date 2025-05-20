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
#include "linux/input.h"

typedef enum
{
    SURFACE_ROLE_NONE = 0,
    SURFACE_ROLE_XDG_TOPLEVEL,
    SURFACE_ROLE_XDG_POPUP,
    SURFACE_ROLE_LAYER,
} SurfaceRole;

typedef struct SurfaceData SurfaceData;

struct SurfaceData
{
    SurfaceRole role;
    struct wl_resource* surface;
    struct wl_resource* pending_frame;
    struct wl_resource* pending_buffer; // The attached but not committed buffer
    char buffer_cleared; // If the buffer has been explicitly cleared since the last commit
    char pending_window_geom; // If the window geom has been set since last commit
    struct wl_resource* xdg_toplevel;
    struct wl_resource* xdg_popup;
    struct wl_resource* xdg_surface;
    struct wl_resource* layer_surface;
    char has_committed_buffer; // This surface has a non-null committed buffer
    char initial_commit_for_role; // Set to 1 when a role is created for a surface, and cleared after the first commit
    char layer_send_configure; // If to send a layer surface configure on the next commit
    uint32_t layer_initial_configure_serial; // The serial of the first configure event
    char layer_initial_configure_acked; // If the first configure event has been acked
    char click_on_surface; // If to click on the surface next commit
    int layer_set_w; // The width to configure the layer surface with
    int layer_set_h; // The height to configure the layer surface with
    uint32_t layer_anchor; // The layer surface's anchor
    uint32_t click_serial; // The most recent serial that was used to click on this surface
    SurfaceData* most_recent_popup; // Start of the popup linked list
    SurfaceData* previous_popup_sibling; // Forms a linked list of popups
    SurfaceData* popup_parent;
};

static struct wl_resource* seat_global = NULL;
static struct wl_resource* pointer_global = NULL;
static struct wl_resource* output_global = NULL;
char configure_delay_enabled = 0;

// Needs to be called before any role objects are assigned
static void surface_data_set_role(SurfaceData* data, SurfaceRole role)
{
    ASSERT_EQ(data->role, SURFACE_ROLE_NONE, "%u");
    char is_xdg_role = (role == SURFACE_ROLE_XDG_TOPLEVEL || role == SURFACE_ROLE_XDG_POPUP);
    ASSERT_EQ(data->xdg_surface != NULL, is_xdg_role, "%d");
    ASSERT(!data->xdg_toplevel);
    ASSERT(!data->xdg_popup);
    ASSERT(!data->layer_surface);
    ASSERT(!data->has_committed_buffer);
    data->role = role;
    data->initial_commit_for_role = 1;
}

static void surface_data_unmap(SurfaceData* data) {
    SurfaceData* popup = data->most_recent_popup;
    while (popup) {
        // Popups must be unmapped before their parents
        ASSERT(!popup->layer_surface);
        ASSERT(!popup->xdg_popup);
        ASSERT(!popup->xdg_toplevel);
        ASSERT(!popup->xdg_surface);
        popup = popup->previous_popup_sibling;
    }
}

static void surface_data_add_pupup(SurfaceData* parent, SurfaceData* popup) {
    ASSERT(!popup->previous_popup_sibling);
    popup->previous_popup_sibling = parent->most_recent_popup;
    parent->most_recent_popup = popup;
    popup->popup_parent = parent;
}

static void surface_data_configure_layer_surface(SurfaceData* data) {
    if (!data->layer_surface || !data->layer_send_configure) return;

    char horiz = (
        (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT) &&
        (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT));
    char vert = (
        (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP) &&
        (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM));
    int width = data->layer_set_w;
    int height = data->layer_set_h;
    if (width == 0 && !horiz)
        FATAL("not horizontally stretched and no width given");
    if (height == 0 && !vert)
        FATAL("not horizontally stretched and no width given");
    if (horiz)
        width = DEFAULT_OUTPUT_WIDTH;
    if (vert)
        height = DEFAULT_OUTPUT_HEIGHT;
    uint32_t serial = wl_display_next_serial(display);
    if (!data->layer_initial_configure_serial)
    {
        data->layer_initial_configure_serial = serial;
    }
    zwlr_layer_surface_v1_send_configure(data->layer_surface, serial, width, height);
    data->layer_send_configure = 0;
}

static int surface_data_configure_layer_surface_timer_callback(void *data) {
    surface_data_configure_layer_surface(data);
    return 0;
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

static void wl_surface_attach(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    RESOURCE_ARG(wl_buffer, buffer, 0);
    SurfaceData* data = wl_resource_get_user_data(resource);
    data->pending_buffer = buffer;
    data->buffer_cleared = buffer == NULL;
    data->click_on_surface = 1;
}

static void wl_surface_commit(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    SurfaceData* data = wl_resource_get_user_data(resource);

    if (data->buffer_cleared)
    {
        data->has_committed_buffer = 0;
        data->buffer_cleared = 0;
    }
    else if (data->pending_buffer)
    {
        if (data->layer_surface)
        {
            ASSERT(data->layer_initial_configure_acked);
        }
        data->has_committed_buffer = 1;
    }

    if (data->pending_buffer)
    {
        wl_buffer_send_release(data->pending_buffer);
        data->pending_buffer = NULL;
    }

    if (data->pending_window_geom) {
        ASSERT(data->has_committed_buffer);
        data->pending_window_geom = 0;
    }

    if (data->pending_frame)
    {
        wl_callback_send_done(data->pending_frame, 0);
        wl_resource_destroy(data->pending_frame);
        data->pending_frame = NULL;
    }

    if (data->initial_commit_for_role)
    {
        ASSERT(!data->has_committed_buffer);
        data->initial_commit_for_role = 0;
    }

    if (data->layer_surface && data->layer_send_configure)
    {
        if (configure_delay_enabled)
        {
            struct wl_event_source* source = wl_event_loop_add_timer(
                wl_display_get_event_loop(display),
                surface_data_configure_layer_surface_timer_callback,
                data
            );
            wl_event_source_timer_update(source, 100);
        }
        else
        {
            surface_data_configure_layer_surface(data);
        }
    }

    if (data->click_on_surface) {
        // Move the pointer onto the surface and click
        // This is needed to trigger a tooltip or popup menu to open for the popup tests
        ASSERT(pointer_global);
        wl_pointer_send_enter(
            pointer_global,
            wl_display_next_serial(display),
            data->surface,
            wl_fixed_from_double(50), wl_fixed_from_double(50));
        wl_pointer_send_frame(pointer_global);
        data->click_serial = wl_display_next_serial(display);
        wl_pointer_send_button(
            pointer_global,
            data->click_serial, 0,
            BTN_LEFT, WL_POINTER_BUTTON_STATE_PRESSED);
        wl_pointer_send_frame(pointer_global);
        data->click_on_surface = 0;
    }
}

static void wl_surface_destroy(struct wl_resource* resource, const struct wl_message* message, union wl_argument* args)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    ASSERT(!data->xdg_popup);
    ASSERT(!data->xdg_toplevel);
    ASSERT(!data->xdg_surface);
    ASSERT(!data->layer_surface);
    data->surface = NULL;
    // Don't free surfaces to guarantee traversing popups is always safe
    // We're employing the missile memory management pattern here https://x.com/pomeranian99/status/858856994438094848
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
}

void wl_seat_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    ASSERT(!seat_global);
    seat_global = wl_resource_create(client, &wl_seat_interface, version, id);
    use_default_impl(seat_global);
    wl_seat_send_capabilities(seat_global, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
};

void wl_output_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    ASSERT(!output_global);
    output_global = wl_resource_create(client, &wl_output_interface, version, id);
    use_default_impl(output_global);
    wl_output_send_done(output_global);
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
    wl_resource_set_user_data(xdg_surface, data);
    data->xdg_surface = xdg_surface;
}

static void xdg_surface_destroy(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    ASSERT(!data->xdg_toplevel);
    ASSERT(!data->xdg_popup);
    data->xdg_surface = NULL;
}

static void xdg_surface_set_window_geometry(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    //INT_ARG(x, 0);
    //INT_ARG(y, 1);
    INT_ARG(width, 2);
    INT_ARG(height, 3);
    SurfaceData* data = wl_resource_get_user_data(resource);
    ASSERT(width > 0);
    ASSERT(height > 0);
    data->pending_window_geom = 1;
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
    surface_data_set_role(data, SURFACE_ROLE_XDG_TOPLEVEL);
    wl_resource_set_user_data(toplevel, data);
    data->xdg_toplevel = toplevel;
}

static void xdg_toplevel_destroy(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    ASSERT(data->xdg_surface);
    data->xdg_toplevel = NULL;
    surface_data_unmap(data);
}

static void xdg_surface_get_popup(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    NEW_ID_ARG(id, 0);
    RESOURCE_ARG(xdg_surface, parent, 1);
    struct wl_resource* popup = wl_resource_create(
        wl_resource_get_client(resource),
        &xdg_popup_interface,
        wl_resource_get_version(resource),
        id);
    use_default_impl(popup);
    xdg_popup_send_configure(popup, 0, 0, 100, 100);
    xdg_surface_send_configure(resource, wl_display_next_serial(display));
    SurfaceData* data = wl_resource_get_user_data(resource);
    surface_data_set_role(data, SURFACE_ROLE_XDG_POPUP);
    wl_resource_set_user_data(popup, data);
    data->xdg_popup = popup;
    if (parent) {
        SurfaceData* parent_data = wl_resource_get_user_data(parent);
        surface_data_add_pupup(parent_data, data);
    }
}

static void xdg_popup_grab(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    RESOURCE_ARG(wl_seat, seat, 0);
    UINT_ARG(serial, 1);
    ASSERT_EQ(seat, seat_global, "%p");
    ASSERT(data->popup_parent);
    ASSERT(data->popup_parent->click_serial);
    ASSERT_EQ(serial, data->popup_parent->click_serial, "%u");
}

static void xdg_popup_destroy(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    ASSERT(data->xdg_surface);
    data->xdg_popup = NULL;
    surface_data_unmap(data);
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

static void zwlr_layer_surface_v1_get_popup(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    RESOURCE_ARG(xdg_popup, popup, 0);
    SurfaceData* data = wl_resource_get_user_data(resource);
    SurfaceData* popup_data = wl_resource_get_user_data(popup);
    ASSERT(!popup_data->popup_parent);
    surface_data_add_pupup(data, popup_data);
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
    surface_data_set_role(data, SURFACE_ROLE_LAYER);
    wl_resource_set_user_data(layer_surface, data);
    data->layer_send_configure = 1;
    data->layer_surface = layer_surface;
}

static void zwlr_layer_surface_v1_ack_configure(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    UINT_ARG(serial, 0);
    if (serial && serial == data->layer_initial_configure_serial)
    {
        data->layer_initial_configure_acked = 1;
    }
}

static void zwlr_layer_surface_v1_destroy(struct wl_resource *resource, const struct wl_message* message, union wl_argument* args)
{
    SurfaceData* data = wl_resource_get_user_data(resource);
    data->layer_surface = NULL;
    surface_data_unmap(data);
}

void init()
{
    OVERRIDE_REQUEST(wl_surface, commit);
    OVERRIDE_REQUEST(wl_surface, frame);
    OVERRIDE_REQUEST(wl_surface, attach);
    OVERRIDE_REQUEST(wl_surface, destroy);
    OVERRIDE_REQUEST(wl_compositor, create_surface);
    OVERRIDE_REQUEST(wl_seat, get_pointer);
    OVERRIDE_REQUEST(xdg_wm_base, get_xdg_surface);
    OVERRIDE_REQUEST(xdg_surface, destroy);
    OVERRIDE_REQUEST(xdg_surface, set_window_geometry);
    OVERRIDE_REQUEST(xdg_surface, get_toplevel);
    OVERRIDE_REQUEST(xdg_toplevel, destroy);
    OVERRIDE_REQUEST(xdg_surface, get_popup);
    OVERRIDE_REQUEST(xdg_popup, grab);
    OVERRIDE_REQUEST(xdg_popup, destroy);
    OVERRIDE_REQUEST(zwlr_layer_shell_v1, get_layer_surface);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, set_anchor);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, set_size);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, get_popup);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, ack_configure);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, destroy);

    wl_global_create(display, &wl_seat_interface, 6, NULL, wl_seat_bind);
    wl_global_create(display, &wl_output_interface, 2, NULL, wl_output_bind);
    default_global_create(display, &wl_shm_interface, 1);
    default_global_create(display, &wl_data_device_manager_interface, 2);
    default_global_create(display, &wl_compositor_interface, 4);
    default_global_create(display, &wl_subcompositor_interface, 1);
    default_global_create(display, &xdg_wm_base_interface, 2);
    default_global_create(display, &zwlr_layer_shell_v1_interface, 4);
}

const char* handle_command(const char* command)
{
    fprintf(stderr, "got command: %s\n", command);
    if (strcmp(command, "enable_configure_delay") == 0) {
        configure_delay_enabled = 1;
        return "configure_delay_enabled";
    } else {
        FATAL_FMT("unkown command: %s", command);
    }
}
