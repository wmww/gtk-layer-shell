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

// This is an implementation of a mock Wayland compositor for testing
// It does not show anything on the screen, and is only as conforment as is required by GTK

// Useful regex for importing a Wayland protocol. First, copy from header file then:
// Find: (struct (\w+)_interface (\{?\n\t.*)*)[\{;]\n(\t[/ ]\*.*\n)+\tvoid \(\*(\w+)\)(\((.*(,\n)?)*\));\n\};((([^=]*\n)*)\nstatic const struct \w+ \w+ = \{\n((    \.\w+ = \w+\,\n)*)\};)?
// Replace with: \1;\n};\nstatic void \2_\5\6\n{\n    FATAL_NOT_IMPL;\n}\n\{10}\nstatic const struct \2_interface \2_impl = {\n    .\5 = \2_\5,\n\{12}};
// Then remove the struct artifact and replace this with a space: \n\t\s*

#pragma once

#include "test-common.h"
#include <wayland-server.h>
#include <stdbool.h>
#include "xdg-shell-server.h"
#include "xdg-dialog-v1-server.h"
#include "ext-session-lock-v1-server.h"
#include "wlr-layer-shell-unstable-v1-server.h"

extern struct wl_display* display;

#define REQUEST_OVERRIDE_IMPL(type, method) static void type##_##method( \
    struct wl_resource* type, \
    const struct wl_message* message, \
    struct wl_resource* new_resource, \
    union wl_argument* args)
#define OVERRIDE_REQUEST(type, method) install_request_override(&type##_interface, #method, type##_##method)
#define RESOURCE_ARG(type, name, index) ASSERT(type_code_at_index(message, index) == 'o'); ASSERT(message->types[index] == &type##_interface); struct wl_resource* name = (struct wl_resource*)args[index].o;
#define UINT_ARG(name, index) ASSERT(type_code_at_index(message, index) == 'u'); uint32_t name = args[index].u;
#define INT_ARG(name, index) ASSERT(type_code_at_index(message, index) == 'i'); int32_t name = args[index].i;

typedef void (*request_override_function_t)(struct wl_resource* resource, const struct wl_message* message, struct wl_resource* created, union wl_argument* args);
void install_request_override(const struct wl_interface* interface, const char* name, request_override_function_t function);
void use_default_impl(struct wl_resource* resource);
void default_global_create(struct wl_display* display, const struct wl_interface* interface, int version);
char type_code_at_index(const struct wl_message* message, int index);

void init();
void register_client(struct wl_client* client);
const char* handle_command(const char** argv);
