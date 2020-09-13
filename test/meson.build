subdir('mock-server')

test_clients = [
    'basic-test',
]

py = find_program('python3')

foreach test_client : test_clients
    exe = executable(
        test_client,
        'test-clients/' + test_client + '.c',
        dependencies: [gtk],
        include_directories: [gtk_layer_shell_inc],
        link_with: [gtk_layer_shell_lib])
    test(
        test_client,
        py,
        workdir: meson.current_source_dir(),
        args: [
            meson.current_source_dir() + '/run-test.py',
            meson.current_build_dir(),
            test_client
        ])
endforeach
