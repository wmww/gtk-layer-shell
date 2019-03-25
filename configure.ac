AC_PREREQ([2.61])
AC_INIT([gtk-layer-shell], [0.1], [wm@wmww.sh])
AM_INIT_AUTOMAKE([-Wall foreign])

AC_PROG_CC

LT_PREREQ([2.2])
LT_INIT([])

PKG_CHECK_MODULES(GTK, [gtk+-3.0 >= 3.22])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(WAYLAND, wayland-client)
AC_SUBST(WAYLAND_CFLAGS)
AC_SUBST(WAYLAND_LIBS)

PKG_CHECK_MODULES(WAYLAND_SCANNER, [wayland-scanner])

AC_CONFIG_FILES([Makefile include/Makefile src/Makefile src/protocol/Makefile example/Makefile])

AC_OUTPUT
