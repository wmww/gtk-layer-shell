project('gtk-layer-shell',
    ['c'],
    version: '0.8.0',
    license: 'LGPLv3',
    meson_version: '>=0.45.1',
    default_options: ['c_std=gnu11', 'warning_level=3'])

lib_so_version = '0'

add_project_arguments(
    ['-Wno-pedantic', '-Werror=unused-function'],
    language: 'c')

gtk = dependency('gtk+-3.0', version: '>=3.22.0')
wayland_client = dependency('wayland-client', version: '>=1.10.0')

# only required for the tests
wayland_server = dependency('wayland-server', version: '>=1.10.0', required: false)

# wayland_scanner is required, but we can find it without pkg-config
wayland_scanner = dependency('wayland-scanner', version: '>=1.10.0', required: false)

# use system xdg-shell protocol when available
wayland_protocols = dependency('wayland-protocols', version: '>=1.16', required: false)

pkg_config = import('pkgconfig')
gnome = import('gnome')

subdir('include')
subdir('protocol')
subdir('gtk-priv')
subdir('src')

gtk_layer_shell = declare_dependency(
    link_with: gtk_layer_shell_lib,
    include_directories: gtk_layer_shell_inc)

subdir('examples')

if get_option('docs')
    subdir('doc')
endif

if get_option('tests')
    subdir('test')
else
    # Add a single always-failing test to tell the user to reconfigure with tests enabled
    py = find_program('python3')
    tests_not_enabled_script = files(meson.current_source_dir() + '/test/tests-not-enabled.py')
    test('Tests not enabled', py, args: [tests_not_enabled_script])
endif
