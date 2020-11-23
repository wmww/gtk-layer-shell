subdir('demo')

demo = executable(
    'gtk-layer-demo',
    demo_srcs,
    build_by_default: get_option('examples'),
    dependencies: [gtk],
    include_directories: [gtk_layer_shell_inc],
    link_with: [gtk_layer_shell_lib],
    install: get_option('examples'))

example = executable('gtk-layer-example',
    'example.c',
    build_by_default: get_option('examples'),
    dependencies: [gtk],
    include_directories: [gtk_layer_shell_inc],
    link_with: [gtk_layer_shell_lib])
