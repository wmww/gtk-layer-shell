AM_CFLAGS = -I$(top_srcdir)/include $(GTK_CFLAGS)
bin_PROGRAMS = gtk-layer-shell-example
gtk_layer_shell_example_SOURCES = \
	main.c \
	anchor-control.c \
	margin-control.c \
	layer-selection.c \
	mscl-toggles.c \
	menu-bar.c
gtk_layer_shell_example_LDADD = ../src/libgtk-layer-shell.a $(GTK_LIBS) $(WAYLAND_LIBS)
