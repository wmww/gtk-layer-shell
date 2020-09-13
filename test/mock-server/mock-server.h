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

// Useful regex for importing a Wayland protocol. First, copy from header file then:
// Find: (struct (\w+)_interface (\{?\n\t.*)*)[\{;]\n(\t[/ ]\*.*\n)+\tvoid \(\*(\w+)\)(\((.*(,\n)?)*\));\n\};((([^=]*\n)*)\nstatic const struct \w+ \w+ = \{\n((    \.\w+ = \w+\,\n)*)\};)?
// Replace with: \1;\n};\nstatic void \2_\5\6\n{\n    FATAL_NOT_IMPL;\n}\n\{10}\nstatic const struct \2_interface \2_impl = {\n    .\5 = \2_\5,\n\{12}};
// Then remove the struct artifact and replace this with a space: \n\t\s*

#ifndef MOCK_SERVER_H
#define MOCK_SERVER_H

#include <wayland-server.h>
#include "xdg-shell-server.h"
#include "wlr-layer-shell-unstable-v1-server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern struct wl_display* display;

#define FATAL_FMT(format, ...) do {fprintf(stderr, "Fatal error: " format "\n", ##__VA_ARGS__); exit(1);} while (0)
#define FATAL(message) FATAL_FMT(message"%s", "")
#define FATAL_NOT_IMPL FATAL_FMT("%s() not implemented", __func__); exit(1)
#define ASSERT(assertion) do {if (!(assertion)) {FATAL_FMT("assertion failed: %s", #assertion);}} while (0)

#define alloc_struct(type) ((type*)alloc_zeroed(sizeof(type)))
void* alloc_zeroed(size_t size);

typedef struct
{
    struct wl_resource* pending_frame;
} SurfaceData;

int ignore_or_destroy_dispatcher(const void* data, void* resource, uint32_t opcode, const struct wl_message* message, union wl_argument* args);
void free_data_destroy_func(struct wl_resource *resource);
void wl_compositor_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id);
void wl_seat_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id);
void xdg_wm_base_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id);
void wl_shm_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id);

#endif // MOCK_SERVER_H
