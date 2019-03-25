AM_CFLAGS = -I$(top_srcdir)/include $(GTK_CFLAGS)
lib_LIBRARIES = libgtk-layer-shell.a
libgtk_layer_shell_a_SOURCES = \
	lib.c \
	protocol/xdg-shell.c \
	protocol/wlr-layer-shell-unstable-v1.c
AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = protocol
