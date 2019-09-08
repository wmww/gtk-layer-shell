subdir('xml')

glib_prefix = dependency('glib-2.0').get_pkgconfig_variable('prefix')
glib_docpath = join_paths(glib_prefix, 'share', 'gtk-doc', 'html')
docpath = join_paths(get_option('datadir'), 'gtk-doc', 'html')

gtk_doc_dep = dependency('gtk-doc')

gnome.gtkdoc(
    'gtk-layer-shell',
    main_xml: 'gtk-layer-shell-docs.sgml',
    src_dir: [
        join_paths(meson.source_root(), 'include'),
        join_paths(meson.build_root(), 'include'),
    ],
    dependencies: [ gtk, wayland_client, wayland_scanner, ],
    gobject_typesfile: 'gtk-layer-shell.types',
    #scan_args: [
    #    '--rebuild-types',
    #    '--rebuild-sections',
    #    '--ignore-headers=' + ' '.join(private_headers),
    #],
    fixxref_args: [
        '--html-dir=@0@'.format(docpath),
        '--extra-dir=@0@'.format(join_paths(glib_docpath, 'glib')),
        '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gobject')),
        '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gio')),
        '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gtk3')),
    ],
    install_dir: 'gtk-layer-shell',
    install: true
)
