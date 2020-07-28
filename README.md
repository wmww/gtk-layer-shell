# GTK Layer Shell

![GTK Layer Shell demo screenshot](https://i.imgur.com/dIuYcBM.png)

A library to write [GTK](https://www.gtk.org/) applications that use [Layer Shell](https://github.com/swaywm/wlr-protocols/blob/master/unstable/wlr-layer-shell-unstable-v1.xml). Layer Shell is a Wayland protocol for desktop shell components, such as panels, notifications and wallpapers. You can use it to anchor your windows to a corner or edge of the output, or stretch them across the entire output. This library only makes sense on Wayland compositors that support Layer Shell, and will not work on X11. It supports all Layer Shell features including popups and popovers (GTK popups Just Work™). Please open issues for any bugs you come across.

## Installing with your package manager
GTK Layer Shell is packaged for the following distros
- [Ubuntu](https://packages.ubuntu.com/source/focal/gtk-layer-shell) (>=20.04)
- [Fedora](https://src.fedoraproject.org/rpms/gtk-layer-shell) (>=31)
- [Arch](https://www.archlinux.org/packages/community/x86_64/gtk-layer-shell/)

## Building from source
1. Clone this repo
2. Install build dependencies (see below)
3. `$ meson build -Dexamples=true -Ddocs=true`
4. `$ ninja -C build`
5. `$ sudo ninja -C build install`
6. `$ sudo ldconfig`

### Build dependencies
* [Meson](https://mesonbuild.com/) (>=0.45.1)
* [libwayland](https://gitlab.freedesktop.org/wayland/wayland) (>=1.10.0)
* [GTK3](https://www.gtk.org/) (>=3.22.0)
* [GObject introspection](https://gitlab.gnome.org/GNOME/gobject-introspection/)
* [GTK Doc](https://www.gtk.org/gtk-doc/) (Optional)

To install these dependencies on Ubuntu 18.04 and later:
```
sudo apt install meson libwayland-dev libgtk-3-dev gobject-introspection libgirepository1.0-dev
```

### Meson options
* `-Dexamples`: If to build gtk-layer-example and if to build and install gtk-layer-demo (default false)
* `-Ddocs`: If to generate the docs (default false)

## Using the library
* `gtk-layer-demo` is installed if examples are enabled. Its UI exposes all features of the library, and it's useful for testing layer shell support in compositors
* [gtk-layer-shell.h](include/gtk-layer-shell.h) shows the full API
* [example/example.c](example/example.c) is a minimal working app
* [demo/](demo/) contains the code for `gtk-layer-demo` (a more complex app)
* The easiest way to build is to use the `gtk-layer-shell-0` pkg-config package. Refer to your build system or the pkg-config docs for further instructions

## Licensing
__By contributing to this project, you agree for your modifications to be licensed under the same license as the files they are made to (which may be a permissive license)__

### Technical details
Parts of GTK Layer Shell are extracted from GTK, and therefore the project as a whole is licensed under [GNU Lesser General Public License (LGPL) version 3](https://www.gnu.org/licenses/lgpl-3.0.en.html) (a newer version of the same license as GTK). See [LICENSE_LGPL.txt](LICENSE_LGPL.txt) for details. Almost all of the non-generated code, however, is licensed under [MIT](https://en.wikipedia.org/wiki/MIT_License) ([LICENSE_MIT.txt](LICENSE_MIT.txt)). At the top of each file should be a header that specifies which license applies to it. Please refer to that if in doubt.

### What this means for you
This library isn't useful unless you're already using GTK, and it doesn't impose additional restrictions. Furthermore, most of the code within this library can be used in permissively licensed or proprietary projects.
