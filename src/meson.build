subdir('protocol')

srcs = files(
    'gtk-layer-shell.c',
    'gtk-wayland.c',
    'custom-shell-surface.c',
    'layer-surface.c',
    'xdg-popup-surface.c',
    'xdg-toplevel-surface.c',
    'gdk-window-hack.c',
    'simple-conversions.c')

gtk_layer_shell_lib = library('gtk-layer-shell',
    srcs,
    xdg_shell_client_header, xdg_shell_private_code,
    layer_shell_client_header, layer_shell_private_code,
    include_directories: [gtk_layer_shell_inc],
    dependencies: [gtk, wayland_client],
    version: meson.project_version(),
    install: true)

pkg_config_name = 'gtk-layer-shell-0'

gir = gnome.generate_gir(
    gtk_layer_shell_lib,
    sources: srcs + [ '../include/gtk-layer-shell.h' ],
    namespace: 'GtkLayerShell',
    nsversion: '0.1',
    identifier_prefix: 'GtkLayerShell',
    symbol_prefix: 'gtk_layer',
    export_packages: pkg_config_name,
    includes: [ 'Gtk-3.0' ],
    header: 'gtk-layer-shell/gtk-layer-shell.h',
    install: true)

pkg_config.generate(
    name: 'gtk-layer-shell',
    version: meson.project_version(),
    libraries: [gtk_layer_shell_lib],
    filebase: pkg_config_name,
    subdirs: 'gtk-layer-shell',
    description: 'Use the Layer Shell Wayland protocol with GTK',
    url: 'https://github.com/wmww/gtk-layer-shell')
