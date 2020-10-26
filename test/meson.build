subdir('mock-server')

test_clients = [
    'basic-test',
    'test-expect-fail',
    'test-anchors',
    'test-set-layer',
    'test-set-size-request',
    'test-adapts-to-screen-size',
    'test-auto-exclusive-zone-no-margin',
    'test-auto-exclusive-zone-with-margin',
    'test-auto-exclusive-zone-weird-bool-values',
    'test-menu-popup',
]

test_client_common_inc = include_directories('test-client-common')
py = find_program('python3')
run_test_script = files(meson.current_source_dir() + '/run-integration-test.py')

foreach test_client : test_clients
    srcs = files(
        'test-client-common/test-client-common.h',
        'test-client-common/test-client-common.c',
        'test-clients/' + test_client + '.c',
    )
    exe = executable(
        test_client,
        srcs,
        dependencies: [gtk, wayland_client],
        include_directories: [gtk_layer_shell_inc, test_client_common_inc],
        link_with: [gtk_layer_shell_lib])
    expect_fail = test_client.endswith('expect-fail')
    test(
        test_client,
        py,
        should_fail: expect_fail,
        workdir: meson.current_source_dir(),
        args: [
            run_test_script,
            meson.current_build_dir(),
            test_client
        ])
endforeach

test_get_version = executable(
    'test-get-version',
    ['unit-tests/test-get-version.c'],
    dependencies: [gtk],
    include_directories: [gtk_layer_shell_inc],
    link_with: [gtk_layer_shell_lib])
test('test-get-version', test_get_version, args: [meson.project_version()])

check_licenses_script = files(meson.current_source_dir() + '/check-licenses.py')
test('check-licenses', py, args: [check_licenses_script])
