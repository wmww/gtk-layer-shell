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

enum surface_role_t {
    SURFACE_ROLE_NONE = 0,
    SURFACE_ROLE_XDG_TOPLEVEL,
    SURFACE_ROLE_XDG_POPUP,
    SURFACE_ROLE_LAYER,
    SURFACE_ROLE_SESSION_LOCK,
};

struct surface_data_t {
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
    uint32_t click_serial; // The most recent serial that was used to click on this surface
    uint32_t configure_serial; // The latest serial used to configure the surface
    bool initial_configure_acked; // If the initial configure event has been acked
    struct surface_data_t* most_recent_popup; // Start of the popup linked list
    struct surface_data_t* previous_popup_sibling; // Forms a linked list of popups
    struct surface_data_t* popup_parent;
};

static struct wl_resource* seat_global = NULL;
static struct wl_resource* pointer_global = NULL;
static struct wl_resource* output_global = NULL;
static struct wl_resource* current_session_lock = NULL;
bool configure_delay_enabled = false;
struct surface_data_t* latest_surface = NULL;

static void layer_surface_send_configure(struct surface_data_t* data);
static void lock_surface_send_configure(struct surface_data_t* data);

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

static int surface_data_configure_timer_callback(void *userdata) {
    struct surface_data_t* data = userdata;
    if (data->role == SURFACE_ROLE_LAYER && data->layer_send_configure && data->layer_surface) {
        layer_surface_send_configure(data);
    } else if (data->role == SURFACE_ROLE_SESSION_LOCK && data->lock_surface) {
        lock_surface_send_configure(data);
    }
    return 0;
}

static void surface_data_send_configure(struct surface_data_t* data) {
    if (configure_delay_enabled) {
        struct wl_event_source* source = wl_event_loop_add_timer(
            wl_display_get_event_loop(display),
            surface_data_configure_timer_callback,
            data
        );
        wl_event_source_timer_update(source, 100);
    } else {
        surface_data_configure_timer_callback(data);
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

    if (data->role == SURFACE_ROLE_LAYER && data->has_committed_buffer && !data->initial_configure_acked) {
        FATAL("Layer surface committed buffer before initial configure");
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
        surface_data_send_configure(data);
    }
}

REQUEST_OVERRIDE_IMPL(wl_surface, destroy) {
    struct surface_data_t* data = wl_resource_get_user_data(wl_surface);
    surface_data_assert_no_role(data);
    data->surface = NULL;
    // Don't free surfaces to guarantee traversing popups is always safe
    // We're employing the missile memory management pattern here https://x.com/pomeranian99/status/858856994438094848
}

REQUEST_OVERRIDE_IMPL(wl_compositor, create_surface) {
    struct surface_data_t* data = calloc(1, sizeof(struct surface_data_t));
    wl_resource_set_user_data(new_resource, data);
    data->surface = new_resource;
    latest_surface = data;
}

void wl_seat_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id) {
    ASSERT(!seat_global);
    seat_global = wl_resource_create(client, &wl_seat_interface, version, id);
    use_default_impl(seat_global);
    wl_seat_send_capabilities(seat_global, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
};

void wl_output_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id) {
    ASSERT(!output_global);
    output_global = wl_resource_create(client, &wl_output_interface, version, id);
    use_default_impl(output_global);
    wl_output_send_done(output_global);
};

REQUEST_OVERRIDE_IMPL(wl_seat, get_pointer) {
    ASSERT(!pointer_global);
    pointer_global = new_resource;
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

REQUEST_OVERRIDE_IMPL(xdg_surface, get_toplevel) {
    struct wl_array states;
    wl_array_init(&states);
    xdg_toplevel_send_configure(new_resource, 0, 0, &states);
    wl_array_release(&states);
    xdg_surface_send_configure(xdg_surface, wl_display_next_serial(display));
    struct surface_data_t* data = wl_resource_get_user_data(xdg_surface);
    surface_data_set_role(data, SURFACE_ROLE_XDG_TOPLEVEL);
    wl_resource_set_user_data(new_resource, data);
    data->xdg_toplevel = new_resource;
}

REQUEST_OVERRIDE_IMPL(xdg_toplevel, destroy) {
    struct surface_data_t* data = wl_resource_get_user_data(xdg_toplevel);
    ASSERT(data->xdg_surface);
    data->xdg_toplevel = NULL;
    surface_data_unmap(data);
}

REQUEST_OVERRIDE_IMPL(xdg_surface, get_popup) {
    RESOURCE_ARG(xdg_surface, parent, 1);
    // If the configure size is too small GTK gets upset and unmaps its popup in protest
    // https://gitlab.gnome.org/GNOME/gtk/-/blob/4.16.12/gtk/gtkpopover.c?ref_type=tags#L719
    xdg_popup_send_configure(new_resource, 0, 0, 500, 500);
    xdg_surface_send_configure(xdg_surface, wl_display_next_serial(display));
    struct surface_data_t* data = wl_resource_get_user_data(xdg_surface);
    surface_data_set_role(data, SURFACE_ROLE_XDG_POPUP);
    wl_resource_set_user_data(new_resource, data);
    data->xdg_popup = new_resource;
    if (parent) {
        struct surface_data_t* parent_data = wl_resource_get_user_data(parent);
        surface_data_add_pupup(parent_data, data);
    }
}

REQUEST_OVERRIDE_IMPL(xdg_popup, grab) {
    struct surface_data_t* data = wl_resource_get_user_data(xdg_popup);
    RESOURCE_ARG(wl_seat, seat, 0);
    UINT_ARG(serial, 1);
    ASSERT_EQ(seat, seat_global, "%p");
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

REQUEST_OVERRIDE_IMPL(zwlr_layer_surface_v1, get_popup) {
    RESOURCE_ARG(xdg_popup, popup, 0);
    struct surface_data_t* data = wl_resource_get_user_data(zwlr_layer_surface_v1);
    struct surface_data_t* popup_data = wl_resource_get_user_data(popup);
    ASSERT(!popup_data->popup_parent);
    surface_data_add_pupup(data, popup_data);
}

REQUEST_OVERRIDE_IMPL(zwlr_layer_shell_v1, get_layer_surface) {
    RESOURCE_ARG(wl_surface, surface, 1);
    struct surface_data_t* data = wl_resource_get_user_data(surface);
    surface_data_set_role(data, SURFACE_ROLE_LAYER);
    wl_resource_set_user_data(new_resource, data);
    data->layer_send_configure = true;
    data->layer_surface = new_resource;
}

static void layer_surface_send_configure(struct surface_data_t* data) {
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
    if (horiz)
        width = DEFAULT_OUTPUT_WIDTH;
    if (vert)
        height = DEFAULT_OUTPUT_HEIGHT;
    data->configure_serial = wl_display_next_serial(display);
    zwlr_layer_surface_v1_send_configure(data->layer_surface, data->configure_serial, width, height);
    data->layer_send_configure = false;
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

static void lock_surface_send_configure(struct surface_data_t* data) {
    data->configure_serial = wl_display_next_serial(display);
    ext_session_lock_surface_v1_send_configure(
        data->lock_surface,
        data->configure_serial,
        DEFAULT_OUTPUT_WIDTH,
        DEFAULT_OUTPUT_HEIGHT
    );
}

REQUEST_OVERRIDE_IMPL(ext_session_lock_v1, get_lock_surface) {
    RESOURCE_ARG(wl_surface, surface, 1);
    RESOURCE_ARG(wl_output, output, 2);
    ASSERT_EQ(output, output_global, "%p");
    struct surface_data_t* data = wl_resource_get_user_data(surface);
    surface_data_set_role(data, SURFACE_ROLE_SESSION_LOCK);
    wl_resource_set_user_data(new_resource, data);
    data->lock_surface = new_resource;
    surface_data_send_configure(data);
}

REQUEST_OVERRIDE_IMPL(ext_session_lock_surface_v1, ack_configure) {
    UINT_ARG(serial, 0);
    struct surface_data_t* data = wl_resource_get_user_data(ext_session_lock_surface_v1);
    if (serial && serial == data->configure_serial) {
        data->initial_configure_acked = true;
    }
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
    OVERRIDE_REQUEST(ext_session_lock_manager_v1, lock);
    OVERRIDE_REQUEST(ext_session_lock_v1, destroy);
    OVERRIDE_REQUEST(ext_session_lock_v1, unlock_and_destroy);
    OVERRIDE_REQUEST(ext_session_lock_v1, get_lock_surface);
    OVERRIDE_REQUEST(ext_session_lock_surface_v1, ack_configure);

    wl_global_create(display, &wl_seat_interface, 6, NULL, wl_seat_bind);
    wl_global_create(display, &wl_output_interface, 2, NULL, wl_output_bind);
    default_global_create(display, &wl_shm_interface, 1);
    default_global_create(display, &wl_data_device_manager_interface, 2);
    default_global_create(display, &wl_compositor_interface, 4);
    default_global_create(display, &wl_subcompositor_interface, 1);
    default_global_create(display, &xdg_wm_base_interface, 2);
    default_global_create(display, &zwlr_layer_shell_v1_interface, 4);
    default_global_create(display, &ext_session_lock_manager_v1_interface, 1);
    default_global_create(display, &xdg_wm_dialog_v1_interface, 1);
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
    fprintf(stderr, "got command:");
    for (int i = 0; argv[i]; i++) {
        fprintf(stderr, " %s", argv[i]);
    }
    fprintf(stderr, "\n");
    if (strcmp(argv[0], "enable_configure_delay") == 0) {
        configure_delay_enabled = true;
        return "configure_delay_enabled";
    } else if (strcmp(argv[0], "click_latest_surface") == 0) {
        // Move the pointer onto the surface and click
        // This is needed to trigger a tooltip or popup menu to open for the popup tests
        ASSERT(pointer_global);
        double x = parse_number(argv[1]);
        double y = parse_number(argv[2]);
        wl_pointer_send_enter(
            pointer_global,
            wl_display_next_serial(display),
            latest_surface->surface,
            wl_fixed_from_double(x), wl_fixed_from_double(y));
        wl_pointer_send_frame(pointer_global);
        latest_surface->click_serial = wl_display_next_serial(display);
        wl_pointer_send_button(
            pointer_global,
            latest_surface->click_serial, 0,
            BTN_LEFT, WL_POINTER_BUTTON_STATE_PRESSED);
        wl_pointer_send_frame(pointer_global);
        wl_pointer_send_button(
            pointer_global,
            wl_display_next_serial(display), 0,
            BTN_LEFT, WL_POINTER_BUTTON_STATE_RELEASED);
        wl_pointer_send_frame(pointer_global);
        return "latest_surface_clicked";
    } else {
        FATAL_FMT("unkown command: %s", argv[0]);
    }
}
