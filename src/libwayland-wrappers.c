#include <dlfcn.h>
#include <stdlib.h>
#include <glib.h>
#include <wayland-client-core.h>
#include "gtk-wayland.h"
#include "wlr-layer-shell-unstable-v1-client.h"
#include "xdg-shell-client.h"

struct wl_proxy * (*real_wl_proxy_marshal_array_flags) (
    struct wl_proxy *proxy,
    uint32_t opcode,
    const struct wl_interface *interface,
    uint32_t version,
    uint32_t flags,
    union wl_argument *args) = NULL;

struct wl_proxy * (*real_wl_proxy_create) (struct wl_proxy *factory, const struct wl_interface *interface) = NULL;

void (*real_wl_proxy_destroy) (struct wl_proxy *proxy) = NULL;

static void libwayland_wrappers_init ()
{
    if (real_wl_proxy_marshal_array_flags)
        return;

    void *handle = dlopen("libwayland-client.so", RTLD_LAZY);
    if (handle == NULL) {
        g_error("failed to dlopen libwayland");
        abort();
    }

#define INIT_SYM(name) if (!(real_##name = dlsym(handle, #name))) {\
    g_error ("dlsym failed to load %s", #name); g_abort (); }

    INIT_SYM(wl_proxy_marshal_array_flags);
    INIT_SYM(wl_proxy_create);
    INIT_SYM(wl_proxy_destroy);

#undef INIT_SYM

    //dlclose(handle);
}

#define WL_CLOSURE_MAX_ARGS 20

struct wl_object {
	const struct wl_interface *interface;
	const void *implementation;
	uint32_t id;
};

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

struct argument_details {
	char type;
	int nullable;
};

static const char *
get_next_argument(const char *signature, struct argument_details *details)
{
	details->nullable = 0;
	for(; *signature; ++signature) {
		switch(*signature) {
		case 'i':
		case 'u':
		case 'f':
		case 's':
		case 'o':
		case 'n':
		case 'a':
		case 'h':
			details->type = *signature;
			return signature + 1;
		case '?':
			details->nullable = 1;
		}
	}
	details->type = '\0';
	return signature;
}

static void
wl_argument_from_va_list(const char *signature, union wl_argument *args,
			 int count, va_list ap)
{
	int i;
	const char *sig_iter;
	struct argument_details arg;

	sig_iter = signature;
	for (i = 0; i < count; i++) {
		sig_iter = get_next_argument(sig_iter, &arg);

		switch(arg.type) {
		case 'i':
			args[i].i = va_arg(ap, int32_t);
			break;
		case 'u':
			args[i].u = va_arg(ap, uint32_t);
			break;
		case 'f':
			args[i].f = va_arg(ap, wl_fixed_t);
			break;
		case 's':
			args[i].s = va_arg(ap, const char *);
			break;
		case 'o':
			args[i].o = va_arg(ap, struct wl_object *);
			break;
		case 'n':
			args[i].o = va_arg(ap, struct wl_object *);
			break;
		case 'a':
			args[i].a = va_arg(ap, struct wl_array *);
			break;
		case 'h':
			args[i].h = va_arg(ap, int32_t);
			break;
		case '\0':
			return;
		}
	}
}

struct xdg_surface *current_xdg_surface = NULL;
struct xdg_surface *current_xdg_toplevel = NULL;

struct wl_proxy *wl_proxy_marshal_array_flags (
    struct wl_proxy *proxy,
    uint32_t opcode,
    const struct wl_interface *interface,
    uint32_t version,
    uint32_t flags,
    union wl_argument *args)
{
    const char* type = proxy->object.interface->name;
    const char* request = proxy->object.interface->methods[opcode].name;
    libwayland_wrappers_init();
    if ((!strcmp(type, "xdg_wm_base") && !strcmp(request, "get_xdg_surface")) ||
        (!strcmp(type, "xdg_surface") && !strcmp(request, "get_toplevel"))) {
        /*if (!strcmp(request, "get_xdg_surface")) {
            struct zwlr_layer_shell_v1 *layer_shell_global = gtk_wayland_get_layer_shell_global ();
            g_assert (layer_shell_global);
            struct wl_surface *wl_surface = (struct wl_surface*)args[1].o;
            zwlr_layer_shell_v1_get_layer_surface (layer_shell_global,
                                                                 wl_surface,
                                                                 NULL,
                                                                 ZWLR_LAYER_SHELL_V1_LAYER_TOP,
                                                                 "foo");
        }*/

        struct wl_proxy* created = malloc(sizeof(struct wl_proxy));
        memcpy(created, proxy, sizeof(struct wl_proxy));
        created->object.interface = proxy->object.interface->methods[opcode].types[0];
        created->object.implementation = NULL;
        created->refcount = 1;
        created->dispatcher = NULL;
        created->version = version;
        if (!strcmp(request, "get_xdg_surface")) {
            current_xdg_surface = created;
        } else if (!strcmp(request, "get_toplevel")) {
            current_xdg_toplevel = created;
        }
        g_message ("%s.%s (intercepted)", type, request);
        return created;
    } else if (!strcmp(type, "xdg_surface") || !strcmp(type, "xdg_toplevel")) {
        g_message ("%s.%s (intercepted)", type, request);
        return NULL;
    } else {
        //g_message ("%s.%s (actually running)", type, request);
        return real_wl_proxy_marshal_array_flags(proxy, opcode, interface, version, flags, args);
    }
}

void send_configure_to_xdg(uint32_t serial)
{
    if (!current_xdg_surface || !current_xdg_toplevel) {
        g_warning("no XDG surface to configure");
        return;
    }

    if (((struct wl_proxy*)current_xdg_surface)->dispatcher || ((struct wl_proxy*)current_xdg_toplevel)->dispatcher) {
        g_error("dispatchers not implemented");
        return;
    }

    if (((struct wl_proxy*)current_xdg_toplevel)->object.implementation)
    {
        struct xdg_toplevel_listener *toplevel = ((struct wl_proxy*)current_xdg_toplevel)->object.implementation;
        struct wl_array states;
        wl_array_init(&states);
        toplevel->configure(
            ((struct wl_proxy*)current_xdg_toplevel)->user_data,
            current_xdg_toplevel,
            0, 0,
            &states
        );
        wl_array_release(&states);
    }

    if (((struct wl_proxy*)current_xdg_surface)->object.implementation)
    {
        struct xdg_surface_listener *surface = ((struct wl_proxy*)current_xdg_surface)->object.implementation;
        surface->configure(((struct wl_proxy*)current_xdg_surface)->user_data, current_xdg_surface, serial);
    }
}

struct wl_proxy *
wl_proxy_marshal_flags(struct wl_proxy *proxy, uint32_t opcode,
		       const struct wl_interface *interface,
		       uint32_t version,
		       uint32_t flags, ...)
{
    union wl_argument args[WL_CLOSURE_MAX_ARGS];
	va_list ap;

	va_start(ap, flags);
	wl_argument_from_va_list(proxy->object.interface->methods[opcode].signature,
				 args, WL_CLOSURE_MAX_ARGS, ap);
	va_end(ap);

	return wl_proxy_marshal_array_flags(proxy, opcode, interface, version, flags, args);
}

void
wl_proxy_marshal(struct wl_proxy *proxy, uint32_t opcode, ...)
{
	union wl_argument args[WL_CLOSURE_MAX_ARGS];
	va_list ap;

	va_start(ap, opcode);
	wl_argument_from_va_list(proxy->object.interface->methods[opcode].signature,
				 args, WL_CLOSURE_MAX_ARGS, ap);
	va_end(ap);

	wl_proxy_marshal_array_constructor(proxy, opcode, args, NULL);
}

void
wl_proxy_marshal_array(struct wl_proxy *proxy, uint32_t opcode,
		       union wl_argument *args)
{
	wl_proxy_marshal_array_constructor(proxy, opcode, args, NULL);
}

struct wl_proxy *
wl_proxy_marshal_constructor(struct wl_proxy *proxy,
			     uint32_t opcode,
			     const struct wl_interface *interface,
			     ...)
{
	union wl_argument args[WL_CLOSURE_MAX_ARGS];
	va_list ap;

	va_start(ap, interface);
	wl_argument_from_va_list(proxy->object.interface->methods[opcode].signature,
				 args, WL_CLOSURE_MAX_ARGS, ap);
	va_end(ap);

	return wl_proxy_marshal_array_constructor(proxy, opcode,
						  args, interface);
}

struct wl_proxy *
wl_proxy_marshal_constructor_versioned(struct wl_proxy *proxy,
				       uint32_t opcode,
				       const struct wl_interface *interface,
				       uint32_t version,
				       ...)
{
	union wl_argument args[WL_CLOSURE_MAX_ARGS];
	va_list ap;

	va_start(ap, version);
	wl_argument_from_va_list(proxy->object.interface->methods[opcode].signature,
				 args, WL_CLOSURE_MAX_ARGS, ap);
	va_end(ap);

	return wl_proxy_marshal_array_constructor_versioned(proxy, opcode,
							    args, interface,
							    version);
}

struct wl_proxy *
wl_proxy_marshal_array_constructor(struct wl_proxy *proxy,
				   uint32_t opcode, union wl_argument *args,
				   const struct wl_interface *interface)
{
	return wl_proxy_marshal_array_constructor_versioned(proxy, opcode,
							    args, interface,
							    proxy->version);
}

struct wl_proxy *
wl_proxy_marshal_array_constructor_versioned(struct wl_proxy *proxy,
					     uint32_t opcode,
					     union wl_argument *args,
					     const struct wl_interface *interface,
					     uint32_t version)
{
	return wl_proxy_marshal_array_flags(proxy, opcode, interface, version, 0, args);
}

struct wl_proxy * wl_proxy_create (struct wl_proxy *factory, const struct wl_interface *interface)
{
    g_message ("creating %s", interface ? interface->name : "(nil)");
    libwayland_wrappers_init();
    return real_wl_proxy_create(factory, interface);
}

void wl_proxy_destroy (struct wl_proxy *proxy)
{
    g_message ("destroying %s", (proxy && proxy->object.interface) ? proxy->object.interface->name : "(nil)");
    libwayland_wrappers_init();
    real_wl_proxy_destroy(proxy);
}
