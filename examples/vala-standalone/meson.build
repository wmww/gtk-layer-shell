project('vala-gtk-layer-shell-example',
    ['vala', 'c'],
    version: '0.1.0',
    license: 'MIT',
    meson_version: '>=0.45.1')

executable(
    'vala-layer-shell',
    files('main.vala'),
    vala_args: ['--pkg=GtkLayerShell-0.1'],
    dependencies: [
        dependency('gtk+-3.0'),
        meson.get_compiler('c').find_library('gtk-layer-shell')])
