AM_CFLAGS = -I$(top_srcdir)/include $(GTK_CFLAGS)
bin_PROGRAMS = gtk-layer-demo
gtk_layer_demo_SOURCES = \
	gtk-layer-demo.c \
	anchor-control.c \
	margin-control.c \
	layer-selection.c \
	mscl-toggles.c \
	menu-bar.c
gtk_layer_demo_LDADD = ../src/libgtk-layer-shell.a $(GTK_LIBS) $(WAYLAND_LIBS)
