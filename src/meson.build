srcs = files(
    'api.c',
    'gtk-wayland.c',
    'custom-shell-surface.c',
    'layer-surface.c',
    'xdg-popup-surface.c',
    'xdg-toplevel-surface.c',
    'gtk-priv-access.c',
    'simple-conversions.c')

version_args = [
    '-DGTK_LAYER_SHELL_MAJOR=' + meson.project_version().split('.')[0],
    '-DGTK_LAYER_SHELL_MINOR=' + meson.project_version().split('.')[1],
    '-DGTK_LAYER_SHELL_MICRO=' + meson.project_version().split('.')[2],
]

gtk_layer_shell_lib = library('gtk-layer-shell',
    srcs, client_protocol_srcs,
    c_args: version_args,
    include_directories: [gtk_layer_shell_inc],
    dependencies: [gtk, wayland_client, gtk_priv],
    version: meson.project_version(),
    soversion: lib_so_version,
    install: true)

pkg_config_name = 'gtk-layer-shell-0'

# GObject introspection file used to interface with other languages
if get_option('introspection')
    gir = gnome.generate_gir(
        gtk_layer_shell_lib,
        dependencies: [gtk],
        sources: srcs + files('../include/gtk-layer-shell.h'),
        namespace: 'GtkLayerShell',
        nsversion: '0.1',
        identifier_prefix: 'GtkLayerShell',
        symbol_prefix: 'gtk_layer',
        export_packages: pkg_config_name,
        includes: [ 'Gtk-3.0' ],
        header: 'gtk-layer-shell/gtk-layer-shell.h',
        install: true)

    if get_option('vapi')
        gnome.generate_vapi(
            pkg_config_name,
            sources: gir[0],
            packages: 'gtk+-3.0',
            install: true)
    endif
endif

pkg_config.generate(
    name: 'gtk-layer-shell',
    version: meson.project_version(),
    libraries: [gtk_layer_shell_lib],
    filebase: pkg_config_name,
    subdirs: 'gtk-layer-shell',
    description: 'Use the Layer Shell Wayland protocol with GTK',
    url: 'https://github.com/wmww/gtk-layer-shell')
