project('gtk-layer-shell',
    ['c'],
    version: '0.1.0',
    license: 'MIT',
    meson_version: '>=0.45.1',
    default_options: ['c_std=c99', 'warning_level=2'])

gtk = dependency('gtk+-wayland-3.0', version: '>=3.22.0')
wayland_client = dependency('wayland-client', version: '>=1.10.0')
# wayland_scanner is required, but we can find it without pkg-config
wayland_scanner = dependency('wayland-scanner', version: '>=1.10.0', required: false)

pkg_config = import('pkgconfig')
gnome = import('gnome')

subdir('include')
subdir('src')
subdir('example')
subdir('demo')