#ifndef LIBWAYLAND_WRAPPERS_H
#define LIBWAYLAND_WRAPPERS_H

#include <wayland-client-core.h>

#define DISPATCH_CLIENT_FACING_EVENT(listener, proxy, event, ...) if (((struct wl_proxy*)proxy)->object.implementation) { \
    ((struct listener *)((struct wl_proxy*)proxy)->object.implementation)->event( \
        ((struct wl_proxy*)proxy)->user_data, __VA_ARGS__); }

// From wayland-private.h in libwayland
struct wl_object {
	const struct wl_interface *interface;
	const void *implementation;
	uint32_t id;
};

// From wayland-client.c in libwayland
struct wl_proxy {
	struct wl_object object;
	struct wl_display *display;
	struct wl_event_queue *queue;
	uint32_t flags;
	int refcount;
	void *user_data;
	wl_dispatcher_func_t dispatcher;
	uint32_t version;
	const char * const *tag;
};

typedef struct wl_proxy *(*client_facing_proxy_handler_func_t) (
    void* data,
    struct wl_proxy *proxy,
    uint32_t opcode,
    const struct wl_interface *interface,
    uint32_t version,
    uint32_t flags,
    union wl_argument *args);

typedef void (*client_facing_proxy_destroy_func_t) (
    void* data,
    struct wl_proxy *proxy);

struct wl_proxy *create_client_facing_proxy (
    struct wl_proxy *factory,
    const struct wl_interface *interface,
    uint32_t version,
    client_facing_proxy_handler_func_t handler,
    client_facing_proxy_destroy_func_t destroy,
    void* data);

extern struct wl_proxy * (*real_wl_proxy_marshal_array_flags) (
    struct wl_proxy *proxy,
    uint32_t opcode,
    const struct wl_interface *interface,
    uint32_t version,
    uint32_t flags,
    union wl_argument *args);

#endif // LIBWAYLAND_WRAPPERS_H
