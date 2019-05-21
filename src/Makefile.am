AM_CFLAGS = -I$(top_srcdir)/include $(GTK_CFLAGS)
lib_LIBRARIES = libgtk-layer-shell.a
libgtk_layer_shell_a_SOURCES = \
	gtk-layer-shell.c \
	gtk-wayland.c \
	custom-shell-surface.c \
	layer-surface.c \
	xdg-popup-surface.c \
	xdg-toplevel-surface.c \
	gdk-window-hack.c \
	simple-conversions.c \
	protocol/xdg-shell.c \
	protocol/wlr-layer-shell-unstable-v1.c
AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = protocol
