test_get_version = executable(
    'test-get-version',
    files('test-get-version.c'),
    dependencies: [gtk],
    include_directories: [gtk_layer_shell_inc, test_common_inc],
    link_with: [gtk_layer_shell_lib])
test('test-get-version', test_get_version, args: [meson.project_version()])
