project('vala-gtk-layer-shell-example',
    ['vala', 'c'],
    version: '0.1.0',
    license: 'MIT',
    meson_version: '>=0.45.1')

gtk = dependency('gtk+-3.0')
gtk_layer_shell = dependency('gtk-layer-shell-0')

executable(
    'vala-layer-shell',
    files('main.vala'),
    dependencies: [ gtk, gtk_layer_shell ])
