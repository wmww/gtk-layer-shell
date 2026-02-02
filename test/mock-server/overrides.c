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

#define OUTPUT_SLOTS 10
struct output_data_t {
    struct wl_global* global;
    int slot;
    int width, height;
};

#define CLIENT_SLOTS 10
struct client_data_t {
    struct wl_client* client;
    int slot;
    struct wl_listener disconnect_listener;
    struct wl_resource* seat;
    struct wl_resource* pointer;
    struct wl_resource* outputs[OUTPUT_SLOTS];
};

enum surface_role_t {
    SURFACE_ROLE_NONE = 0,
    SURFACE_ROLE_XDG_TOPLEVEL,
    SURFACE_ROLE_XDG_POPUP,
    SURFACE_ROLE_LAYER,
    SURFACE_ROLE_SESSION_LOCK,
};

#define SURFACE_SLOTS 20
struct surface_data_t {
    struct client_data_t* client;
    enum surface_role_t role;
    struct wl_resource* surface;
    struct wl_resource* pending_frame;
    struct wl_resource* pending_buffer; // The attached but not committed buffer
    bool buffer_cleared; // If the buffer has been explicitly cleared since the last commit
    bool pending_window_geom; // If the window geom has been set since last commit
    struct wl_resource* xdg_toplevel;
    struct wl_resource* xdg_popup;
    struct wl_resource* xdg_surface;
    struct wl_resource* layer_surface;
    struct wl_resource* lock_surface;
    bool has_committed_buffer; // This surface has a non-null committed buffer
    bool initial_commit_for_role; // Set to 1 when a role is created for a surface, and cleared after the first commit
    bool layer_send_configure; // If to send a layer surface configure on the next commit
    int layer_set_w; // The width to configure the layer surface with
    int layer_set_h; // The height to configure the layer surface with
    uint32_t layer_anchor; // The layer surface's anchor
    struct {
        int top, left, bottom, right;
    } layer_margin; // The layer surface's margin
    uint32_t click_serial; // The most recent serial that was used to click on this surface
    uint32_t configure_serial; // The latest serial used to configure the surface
    bool initial_configure_acked; // If the initial configure event has been acked
    struct surface_data_t* most_recent_popup; // Start of the popup linked list
    struct surface_data_t* previous_popup_sibling; // Forms a linked list of popups
    struct surface_data_t* popup_parent;
    struct output_data_t* explicit_output; // The output requested by the client, or NULL if none
    struct output_data_t* effective_output; // The output this surface is on, or NULL if none
};

int next_output_slot = 0;
static struct output_data_t outputs[OUTPUT_SLOTS] = {0};

static void create_output(int width, int height);
static void destroy_output(int slot);
static struct client_data_t clients[CLIENT_SLOTS] = {0};
static struct surface_data_t surfaces[SURFACE_SLOTS] = {0};
static struct wl_resource* current_session_lock = NULL;
bool configure_delay_enabled = false;
bool destroy_outputs_on_layer_surface_create = false;
int next_surface_slot = 0;
struct surface_data_t* latest_surface = NULL;

static struct output_data_t* find_output(struct wl_resource* resource) {
    for (int client_i = 0; client_i < CLIENT_SLOTS; client_i++)
        for (int output_i = 0; output_i < next_output_slot; output_i++)
            if (clients[client_i].outputs[output_i] == resource)
                return &outputs[output_i];
    return NULL;
}

static struct output_data_t* default_output() {
    for (int i = 0; i < next_output_slot; i++)
        if (outputs[i].global)
            return &outputs[i];
    return NULL;
}

static struct client_data_t* client_from_wl_client(struct wl_client* client) {
    for (int i = 0; i < CLIENT_SLOTS; i++)
        if (clients[i].client == client)
            return &clients[i];
    FATAL_FMT("invalid client %p", (void*)client);
}

static struct client_data_t* client_from_wl_resource(struct wl_resource* resource) {
    return client_from_wl_client(wl_resource_get_client(resource));
}

static void surface_data_assert_no_role(struct surface_data_t* data) {
    ASSERT(!data->xdg_popup);
    ASSERT(!data->xdg_toplevel);
    ASSERT(!data->layer_surface);
    ASSERT(!data->lock_surface);
}

// Needs to be called before any role objects are assigned
static void surface_data_set_role(struct surface_data_t* data, enum surface_role_t role) {
    if (data->role != SURFACE_ROLE_NONE) {
        ASSERT_EQ(data->role, role, "%u");
    }

    if (role == SURFACE_ROLE_XDG_TOPLEVEL || role == SURFACE_ROLE_XDG_POPUP) {
        ASSERT(data->xdg_surface != NULL);
    } else {
        ASSERT(data->xdg_surface == NULL);
    }

    struct wl_resource* xdg_surface = data->xdg_surface;
    data->xdg_surface = NULL; // XDG surfaces are allowed, so hide it from surface_data_assert_no_role()
    surface_data_assert_no_role(data);
    data->xdg_surface = xdg_surface;

    ASSERT(!data->has_committed_buffer);
    data->role = role;
    data->initial_commit_for_role = true;
}

static void surface_data_unmap(struct surface_data_t* data) {
    struct surface_data_t* popup = data->most_recent_popup;
    while (popup) {
        // Popups must be unmapped before their parents
        surface_data_assert_no_role(data);
        popup = popup->previous_popup_sibling;
    }
}

static void surface_data_add_pupup(struct surface_data_t* parent, struct surface_data_t* popup) {
    ASSERT(!popup->previous_popup_sibling);
    popup->previous_popup_sibling = parent->most_recent_popup;
    parent->most_recent_popup = popup;
    popup->popup_parent = parent;
}

static void surface_data_send_configure(struct surface_data_t* data) {
    data->configure_serial = wl_display_next_serial(display);
    switch (data->role) {
        case SURFACE_ROLE_NONE:
            break;

        case SURFACE_ROLE_XDG_TOPLEVEL:
            if (!data->xdg_toplevel || !data->xdg_surface) break;
            struct wl_array states;
            wl_array_init(&states);
            xdg_toplevel_send_configure(data->xdg_toplevel, 0, 0, &states);
            wl_array_release(&states);
            xdg_surface_send_configure(data->xdg_surface, data->configure_serial);
            break;

        case SURFACE_ROLE_XDG_POPUP:
            if (!data->xdg_popup || !data->xdg_surface) break;
            // If the configure size is too small GTK gets upset and unmaps its popup in protest
            // https://gitlab.gnome.org/GNOME/gtk/-/blob/4.16.12/gtk/gtkpopover.c?ref_type=tags#L719
            xdg_popup_send_configure(data->xdg_popup, 0, 0, 500, 500);
            xdg_surface_send_configure(data->xdg_surface, data->configure_serial);
            break;

        case SURFACE_ROLE_LAYER:
            if (!data->layer_send_configure || !data->layer_surface) break;
            bool horiz = (
                (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT) &&
                (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT));
            bool vert = (
                (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP) &&
                (data->layer_anchor & ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM));
            int width = data->layer_set_w;
            int height = data->layer_set_h;
            if (width == 0 && !horiz)
                FATAL("not horizontally stretched and no width given");
            if (height == 0 && !vert)
                FATAL("not horizontally stretched and no width given");
            if (horiz && data->effective_output) width =
                data->effective_output->width - data->layer_margin.left - data->layer_margin.right;
            if (vert  && data->effective_output) height =
                data->effective_output->height - data->layer_margin.top - data->layer_margin.bottom;
            if (width < 0 || height < 0) {
                zwlr_layer_surface_v1_send_closed(data->layer_surface);
            } else {
                zwlr_layer_surface_v1_send_configure(data->layer_surface, data->configure_serial, width, height);
            }
            data->layer_send_configure = false;
            break;

        case SURFACE_ROLE_SESSION_LOCK:
            if (!data->lock_surface) break;
            ASSERT(data->explicit_output);
            ext_session_lock_surface_v1_send_configure(
                data->lock_surface,
                data->configure_serial,
                data->explicit_output->width,
                data->explicit_output->height
            );
            break;
    }
}

static int surface_data_configure_timer_callback(void *data) {
    surface_data_send_configure(data);
    return 0;
}

static void surface_data_queue_configure(struct surface_data_t* data) {
    if (configure_delay_enabled) {
        struct wl_event_source* source = wl_event_loop_add_timer(
            wl_display_get_event_loop(display),
            surface_data_configure_timer_callback,
            data
        );
        wl_event_source_timer_update(source, 100);
    } else {
        surface_data_send_configure(data);
    }
}

REQUEST_OVERRIDE_IMPL(wl_surface, frame) {
    struct surface_data_t* data = wl_resource_get_user_data(wl_surface);
    ASSERT(!data->pending_frame);
    data->pending_frame = new_resource;
}

REQUEST_OVERRIDE_IMPL(wl_surface, attach) {
    RESOURCE_ARG(wl_buffer, buffer, 0);
    struct surface_data_t* data = wl_resource_get_user_data(wl_surface);
    data->pending_buffer = buffer;
    data->buffer_cleared = buffer == NULL;
}

REQUEST_OVERRIDE_IMPL(wl_surface, commit) {
    struct surface_data_t* data = wl_resource_get_user_data(wl_surface);

    if (data->role == SURFACE_ROLE_SESSION_LOCK) {
        if (data->buffer_cleared) {
            FATAL("null buffer committed to session lock surface");
        } else if (!data->pending_buffer && !data->has_committed_buffer) {
            FATAL("no buffer has been attached to committed session lock surface");
        } else if (!data->initial_configure_acked) {
            FATAL("session lock surface committed before initial configure acked");
        }
    }

    if (data->buffer_cleared) {
        data->has_committed_buffer = false;
        data->buffer_cleared = false;
    } else if (data->pending_buffer) {
        data->has_committed_buffer = true;
    }

    if (data->role != SURFACE_ROLE_NONE && data->has_committed_buffer && !data->initial_configure_acked) {
        FATAL("committed buffer before initial configure");
    }

    if (data->pending_buffer) {
        wl_buffer_send_release(data->pending_buffer);
        data->pending_buffer = NULL;
    }

    if (data->pending_window_geom) {
        ASSERT(data->has_committed_buffer);
        data->pending_window_geom = false;
    }

    if (data->pending_frame) {
        wl_callback_send_done(data->pending_frame, 0);
        wl_resource_destroy(data->pending_frame);
        data->pending_frame = NULL;
    }

    if (data->initial_commit_for_role && data->role != SURFACE_ROLE_SESSION_LOCK) {
        ASSERT(!data->has_committed_buffer);
        data->initial_commit_for_role = false;
    }

    if (data->role == SURFACE_ROLE_LAYER && data->layer_send_configure) {
        surface_data_queue_configure(data);
    }
}

REQUEST_OVERRIDE_IMPL(wl_surface, destroy) {
    struct surface_data_t* data = wl_resource_get_user_data(wl_surface);
    surface_data_assert_no_role(data);
    data->surface = NULL;
}

REQUEST_OVERRIDE_IMPL(wl_compositor, create_surface) {
    struct client_data_t* client_data = client_from_wl_resource(wl_compositor);
    ASSERT(next_surface_slot < SURFACE_SLOTS);
    struct surface_data_t* data = &surfaces[next_surface_slot];
    next_surface_slot++;
    wl_resource_set_user_data(new_resource, data);
    data->client = client_data;
    data->surface = new_resource;
    latest_surface = data;
}

void wl_seat_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id) {
    struct client_data_t* client_data = client_from_wl_client(client);
    ASSERT(!client_data->seat);
    client_data->seat = wl_resource_create(client, &wl_seat_interface, version, id);
    use_default_impl(client_data->seat);
    wl_seat_send_capabilities(client_data->seat, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
};

void wl_output_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id) {
    struct output_data_t* output = data;
    struct client_data_t* client_data = client_from_wl_client(client);
    ASSERT(!client_data->outputs[output->slot]);
    struct wl_resource* resource = wl_resource_create(client, &wl_output_interface, version, id);
    use_default_impl(resource);
    client_data->outputs[output->slot] = resource;
    wl_output_send_mode(resource, WL_OUTPUT_MODE_CURRENT, output->width, output->height, 60000);
    wl_output_send_done(resource);
};

REQUEST_OVERRIDE_IMPL(wl_seat, get_pointer) {
    struct client_data_t* client_data = client_from_wl_resource(wl_seat);
    ASSERT(!client_data->pointer);
    client_data->pointer = new_resource;
}

REQUEST_OVERRIDE_IMPL(xdg_wm_base, get_xdg_surface) {
    RESOURCE_ARG(wl_surface, surface, 1);
    struct surface_data_t* data = wl_resource_get_user_data(surface);
    wl_resource_set_user_data(new_resource, data);
    data->xdg_surface = new_resource;
}

REQUEST_OVERRIDE_IMPL(xdg_surface, destroy) {
    struct surface_data_t* data = wl_resource_get_user_data(xdg_surface);
    ASSERT(!data->xdg_toplevel);
    ASSERT(!data->xdg_popup);
    data->xdg_surface = NULL;
    data->popup_parent = NULL;
    data->previous_popup_sibling = NULL;
    data->most_recent_popup = NULL;
}

REQUEST_OVERRIDE_IMPL(xdg_surface, set_window_geometry) {
    //INT_ARG(x, 0);
    //INT_ARG(y, 1);
    INT_ARG(width, 2);
    INT_ARG(height, 3);
    struct surface_data_t* data = wl_resource_get_user_data(xdg_surface);
    ASSERT(width > 0);
    ASSERT(height > 0);
    data->pending_window_geom = true;
}

REQUEST_OVERRIDE_IMPL(xdg_surface, ack_configure) {
    struct surface_data_t* data = wl_resource_get_user_data(xdg_surface);
    UINT_ARG(serial, 0);
    if (serial && serial == data->configure_serial) {
        data->initial_configure_acked = true;
    }
}

REQUEST_OVERRIDE_IMPL(xdg_surface, get_toplevel) {
    struct surface_data_t* data = wl_resource_get_user_data(xdg_surface);
    surface_data_set_role(data, SURFACE_ROLE_XDG_TOPLEVEL);
    wl_resource_set_user_data(new_resource, data);
    data->xdg_toplevel = new_resource;
    surface_data_queue_configure(data);
}

REQUEST_OVERRIDE_IMPL(xdg_toplevel, destroy) {
    struct surface_data_t* data = wl_resource_get_user_data(xdg_toplevel);
    ASSERT(data->xdg_surface);
    data->xdg_toplevel = NULL;
    surface_data_unmap(data);
}

REQUEST_OVERRIDE_IMPL(xdg_surface, get_popup) {
    RESOURCE_ARG(xdg_surface, parent, 1);
    struct surface_data_t* data = wl_resource_get_user_data(xdg_surface);
    surface_data_set_role(data, SURFACE_ROLE_XDG_POPUP);
    wl_resource_set_user_data(new_resource, data);
    ASSERT(!data->xdg_popup);
    data->xdg_popup = new_resource;
    if (parent) {
        struct surface_data_t* parent_data = wl_resource_get_user_data(parent);
        surface_data_add_pupup(parent_data, data);
    }
    surface_data_queue_configure(data);
}

REQUEST_OVERRIDE_IMPL(xdg_popup, grab) {
    struct surface_data_t* data = wl_resource_get_user_data(xdg_popup);
    RESOURCE_ARG(wl_seat, seat, 0);
    UINT_ARG(serial, 1);
    ASSERT_EQ(seat, client_from_wl_resource(xdg_popup)->seat, "%p");
    ASSERT(data->popup_parent);
    if (data->popup_parent->click_serial) {
        ASSERT_EQ(serial, data->popup_parent->click_serial, "%u");
    }
}

REQUEST_OVERRIDE_IMPL(xdg_popup, destroy) {
    struct surface_data_t* data = wl_resource_get_user_data(xdg_popup);
    ASSERT(data->xdg_surface);
    data->xdg_popup = NULL;
    surface_data_unmap(data);
}

REQUEST_OVERRIDE_IMPL(zwlr_layer_surface_v1, set_anchor) {
    UINT_ARG(anchor, 0);
    struct surface_data_t* data = wl_resource_get_user_data(zwlr_layer_surface_v1);
    data->layer_send_configure = true;
    data->layer_anchor = anchor;
}

REQUEST_OVERRIDE_IMPL(zwlr_layer_surface_v1, set_size) {
    UINT_ARG(width, 0);
    UINT_ARG(height, 1);
    struct surface_data_t* data = wl_resource_get_user_data(zwlr_layer_surface_v1);
    data->layer_send_configure = true;
    data->layer_set_w = width;
    data->layer_set_h = height;
}

REQUEST_OVERRIDE_IMPL(zwlr_layer_surface_v1, set_margin) {
    INT_ARG(top, 0);
    INT_ARG(right, 1);
    INT_ARG(bottom, 2);
    INT_ARG(left, 3);
    struct surface_data_t* data = wl_resource_get_user_data(zwlr_layer_surface_v1);
    data->layer_send_configure = true;
    data->layer_margin.top = top;
    data->layer_margin.right = right;
    data->layer_margin.bottom = bottom;
    data->layer_margin.left = left;
}

REQUEST_OVERRIDE_IMPL(zwlr_layer_surface_v1, get_popup) {
    RESOURCE_ARG(xdg_popup, popup, 0);
    struct surface_data_t* data = wl_resource_get_user_data(zwlr_layer_surface_v1);
    struct surface_data_t* popup_data = wl_resource_get_user_data(popup);
    ASSERT(!popup_data->popup_parent);
    surface_data_add_pupup(data, popup_data);
}

REQUEST_OVERRIDE_IMPL(zwlr_layer_shell_v1, get_layer_surface) {
    RESOURCE_ARG(wl_surface, surface, 1);
    RESOURCE_ARG(wl_output, output, 2);
    struct surface_data_t* data = wl_resource_get_user_data(surface);
    surface_data_set_role(data, SURFACE_ROLE_LAYER);
    wl_resource_set_user_data(new_resource, data);
    data->layer_send_configure = true;
    data->layer_surface = new_resource;
    if (output) {
        data->explicit_output = find_output(output);
        ASSERT(data->explicit_output);
    }
    data->effective_output = data->explicit_output ? data->explicit_output : default_output();

    if (destroy_outputs_on_layer_surface_create) {
        for (int i = 0; i < OUTPUT_SLOTS; i++) {
            if (outputs[i].global) {
                destroy_output(i);
            }
        }
        destroy_outputs_on_layer_surface_create = false;
    }
}

REQUEST_OVERRIDE_IMPL(zwlr_layer_surface_v1, ack_configure) {
    struct surface_data_t* data = wl_resource_get_user_data(zwlr_layer_surface_v1);
    UINT_ARG(serial, 0);
    if (serial && serial == data->configure_serial) {
        data->initial_configure_acked = true;
    }
}

REQUEST_OVERRIDE_IMPL(zwlr_layer_surface_v1, destroy) {
    struct surface_data_t* data = wl_resource_get_user_data(zwlr_layer_surface_v1);
    data->layer_surface = NULL;
    surface_data_unmap(data);
}

REQUEST_OVERRIDE_IMPL(ext_session_lock_manager_v1, lock) {
    if (current_session_lock) {
        ext_session_lock_v1_send_finished(new_resource);
    } else {
        current_session_lock = new_resource;
        ext_session_lock_v1_send_locked(new_resource);
    }
}

REQUEST_OVERRIDE_IMPL(ext_session_lock_v1, destroy) {
    if (ext_session_lock_v1 == current_session_lock) {
        FATAL(".destroy (instead of .unlock_and_destroy) called on active lock");
    }
}

REQUEST_OVERRIDE_IMPL(ext_session_lock_v1, unlock_and_destroy) {
    if (ext_session_lock_v1 != current_session_lock) {
        FATAL(".unlock_and_destroy (instead of .destroy) called on inactive lock");
    }
    current_session_lock = NULL;
}

REQUEST_OVERRIDE_IMPL(ext_session_lock_v1, get_lock_surface) {
    RESOURCE_ARG(wl_surface, surface, 1);
    RESOURCE_ARG(wl_output, output, 2);
    struct surface_data_t* data = wl_resource_get_user_data(surface);
    surface_data_set_role(data, SURFACE_ROLE_SESSION_LOCK);
    wl_resource_set_user_data(new_resource, data);
    data->lock_surface = new_resource;
    ASSERT(output);
    data->effective_output = data->explicit_output = find_output(output);
    surface_data_queue_configure(data);
}

REQUEST_OVERRIDE_IMPL(ext_session_lock_surface_v1, ack_configure) {
    UINT_ARG(serial, 0);
    struct surface_data_t* data = wl_resource_get_user_data(ext_session_lock_surface_v1);
    if (serial && serial == data->configure_serial) {
        data->initial_configure_acked = true;
    }
}

REQUEST_OVERRIDE_IMPL(ext_session_lock_surface_v1, destroy) {
    struct surface_data_t* data = wl_resource_get_user_data(ext_session_lock_surface_v1);
    data->lock_surface = NULL;
}

static void create_output(int width, int height) {
    ASSERT(next_output_slot < OUTPUT_SLOTS);
    outputs[next_output_slot] = (struct output_data_t) {
        .global = wl_global_create(display, &wl_output_interface, 2, &outputs[next_output_slot], wl_output_bind),
        .slot = next_output_slot,
        .width = width,
        .height = height,
    };
    next_output_slot++;
}

static void destroy_output(int slot) {
    struct output_data_t* output = &outputs[slot];
    for (int i = 0; i < next_surface_slot; i++) {
        if (surfaces[i].layer_surface && surfaces[i].effective_output == output) {
            zwlr_layer_surface_v1_send_closed(surfaces[i].layer_surface);
        }
    }
    if (slot < 0 || slot >= OUTPUT_SLOTS || !outputs[slot].global)
        FATAL_FMT("destroying invalid output %d", slot);
    wl_global_remove(output->global);
    *output = (struct output_data_t){0};
}

void init() {
    OVERRIDE_REQUEST(wl_surface, commit);
    OVERRIDE_REQUEST(wl_surface, frame);
    OVERRIDE_REQUEST(wl_surface, attach);
    OVERRIDE_REQUEST(wl_surface, destroy);
    OVERRIDE_REQUEST(wl_compositor, create_surface);
    OVERRIDE_REQUEST(wl_seat, get_pointer);
    OVERRIDE_REQUEST(xdg_wm_base, get_xdg_surface);
    OVERRIDE_REQUEST(xdg_surface, destroy);
    OVERRIDE_REQUEST(xdg_surface, set_window_geometry);
    OVERRIDE_REQUEST(xdg_surface, ack_configure);
    OVERRIDE_REQUEST(xdg_surface, get_toplevel);
    OVERRIDE_REQUEST(xdg_toplevel, destroy);
    OVERRIDE_REQUEST(xdg_surface, get_popup);
    OVERRIDE_REQUEST(xdg_popup, grab);
    OVERRIDE_REQUEST(xdg_popup, destroy);
    OVERRIDE_REQUEST(zwlr_layer_shell_v1, get_layer_surface);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, set_anchor);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, set_size);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, set_margin);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, get_popup);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, ack_configure);
    OVERRIDE_REQUEST(zwlr_layer_surface_v1, destroy);
    OVERRIDE_REQUEST(ext_session_lock_manager_v1, lock);
    OVERRIDE_REQUEST(ext_session_lock_v1, destroy);
    OVERRIDE_REQUEST(ext_session_lock_v1, unlock_and_destroy);
    OVERRIDE_REQUEST(ext_session_lock_v1, get_lock_surface);
    OVERRIDE_REQUEST(ext_session_lock_surface_v1, ack_configure);
    OVERRIDE_REQUEST(ext_session_lock_surface_v1, destroy);

    create_output(DEFAULT_OUTPUT_WIDTH, DEFAULT_OUTPUT_HEIGHT);

    wl_global_create(display, &wl_seat_interface, 6, NULL, wl_seat_bind);
    default_global_create(display, &wl_shm_interface, 1);
    default_global_create(display, &wl_data_device_manager_interface, 2);
    default_global_create(display, &wl_compositor_interface, 4);
    default_global_create(display, &wl_subcompositor_interface, 1);
    default_global_create(display, &xdg_wm_base_interface, 2);
    default_global_create(display, &zwlr_layer_shell_v1_interface, 4);
    default_global_create(display, &ext_session_lock_manager_v1_interface, 1);
    default_global_create(display, &xdg_wm_dialog_v1_interface, 1);
}

static void client_disconnect(struct wl_listener *listener, void *data) {
    struct wl_client* client = (struct wl_client*)data;
    struct client_data_t* client_data = client_from_wl_client(client);
    *client_data = (struct client_data_t){0};
    fprintf(stderr, "Client %d disconnected\n", client_data->slot);
    bool clients_still_connected = false;
    for (int i = 0; i < CLIENT_SLOTS; i++) if (clients[i].client) clients_still_connected = true;
    if (!clients_still_connected) {
        fprintf(stderr, "Shutting down\n");
        wl_display_terminate(display);
    }
}

void register_client(struct wl_client* client) {
    for (int i = 0; i < CLIENT_SLOTS; i++) {
        if (!clients[i].client) {
            clients[i] = (struct client_data_t) {
                .client = client,
                .slot = i,
                .disconnect_listener.notify = client_disconnect,
            };
            fprintf(stderr, "Client %d connected\n", i);
            wl_client_add_destroy_listener(client, &clients[i].disconnect_listener);
            return;
        }
    }
    FATAL("ran out of client slots");
}

static double parse_number(const char* str) {
    bool valid = true;
    if (str && *str) {
        for (const char* c = str; *c; c++) {
            if (!(*c >= '0' && *c <= '9') && *c != '.') {
                valid = false;
            }
        }
    } else {
        valid = false;
    }
    if (!valid) {
        FATAL_FMT("invalid number '%s'", str);
    }
    return atof(str);
}

const char* handle_command(const char** argv) {
    fprintf(stderr, "Got command:");
    for (int i = 0; argv[i]; i++) {
        fprintf(stderr, " %s", argv[i]);
    }
    fprintf(stderr, "\n");
    if (strcmp(argv[0], "enable_configure_delay") == 0) {
        configure_delay_enabled = true;
        return "configure_delay_enabled";
    } else if (strcmp(argv[0], "destroy_outputs_on_layer_surface_create") == 0) {
        destroy_outputs_on_layer_surface_create = true;
        return "destroy_outputs_on_layer_surface_create_enabled";
    } else if (strcmp(argv[0], "click_latest_surface") == 0) {
        // Move the pointer onto the surface and click
        // This is needed to trigger a tooltip or popup menu to open for the popup tests
        ASSERT(latest_surface);
        struct wl_resource* pointer = latest_surface->client->pointer;
        ASSERT(latest_surface->client->pointer);
        wl_fixed_t x = wl_fixed_from_double(parse_number(argv[1]));
        wl_fixed_t y = wl_fixed_from_double(parse_number(argv[2]));
        wl_pointer_send_enter(pointer, wl_display_next_serial(display), latest_surface->surface, x, y);
        wl_pointer_send_frame(pointer);
        latest_surface->click_serial = wl_display_next_serial(display);
        wl_pointer_send_button(pointer, latest_surface->click_serial, 0, BTN_LEFT, WL_POINTER_BUTTON_STATE_PRESSED);
        wl_pointer_send_frame(pointer);
        wl_pointer_send_button(pointer, wl_display_next_serial(display), 0, BTN_LEFT, WL_POINTER_BUTTON_STATE_RELEASED);
        wl_pointer_send_frame(pointer);
        return "latest_surface_clicked";
    } else if (strcmp(argv[0], "create_output") == 0) {
        int width = parse_number(argv[1]);
        int height = parse_number(argv[2]);
        create_output(width, height);
        return "output_created";
    } else if (strcmp(argv[0], "destroy_output") == 0) {
        int slot = parse_number(argv[1]);
        destroy_output(slot);
        return "output_destroyed";
    } else {
        FATAL_FMT("unkown command: %s", argv[0]);
    }
}
