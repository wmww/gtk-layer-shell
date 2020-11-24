test_get_version = executable(
    'test-get-version',
    files('test-get-version.c'),
    dependencies: [gtk, gtk_layer_shell, test_common])

test('test-get-version', test_get_version, args: [meson.project_version()])
