subdir('demo')
# vala-standalone is it's own meson project, so do not subdir() it

executable(
    'gtk-layer-demo',
    demo_srcs,
    build_by_default: get_option('examples'),
    dependencies: [gtk, gtk_layer_shell],
    install: get_option('examples'))

executable(
    'simple-example-c',
    files('simple-example.c'),
    build_by_default: get_option('examples'),
    dependencies: [gtk, gtk_layer_shell],
    install: false)
