subdir('mock-server')

test_clients = [
    'basic-test',
    'test-anchors',
]

test_client_common_inc = include_directories('test-client-common')
py = find_program('python3')

foreach test_client : test_clients
    srcs = files(
        'test-client-common/test-client-common.h',
        'test-client-common/test-client-common.c',
        'test-clients/' + test_client + '.c',
    )
    run_test_script = files(meson.current_source_dir() + '/run-integration-test.py')
    exe = executable(
        test_client,
        srcs,
        dependencies: [gtk],
        include_directories: [gtk_layer_shell_inc, test_client_common_inc],
        link_with: [gtk_layer_shell_lib])
    test(
        test_client,
        py,
        workdir: meson.current_source_dir(),
        args: [
            run_test_script,
            meson.current_build_dir(),
            test_client
        ])
endforeach

check_licenses_script = files(meson.current_source_dir() + '/check-licenses.py')
test('check-licenses', py, args: [check_licenses_script])
