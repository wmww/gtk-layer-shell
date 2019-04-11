AM_CFLAGS = -I$(top_srcdir)/include $(GTK_CFLAGS)
bin_PROGRAMS = gtk-layer-example
gtk_layer_example_SOURCES = \
	example.c
gtk_layer_example_LDADD = ../src/libgtk-layer-shell.a $(GTK_LIBS) $(WAYLAND_LIBS)
