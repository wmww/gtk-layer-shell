# GTK Layer Shell

![GTK Layer Shell demo screenshot](https://i.imgur.com/dIuYcBM.png)

A library to write [GTK](https://www.gtk.org/) applications that use [Layer Shell](https://github.com/swaywm/wlr-protocols/blob/master/unstable/wlr-layer-shell-unstable-v1.xml). Layer Shell is a Wayland protocol for desktop shell components, such as panels, notifications and wallpapers. You can use it to anchor your windows to a corner or edge of the output, or stretch them across the entire output. It supports all Layer Shell features including popups and popovers (GTK popups Just Work™). This Library is compatible with C, C++ and any language that supports GObject introspection files (Python, Vala, etc, see using the library below).

## Supported Desktops
This library only works on Wayland, and only on Wayland compositors that support the Layer Shell protcol. Layer shell __is supported__ on:
- wlroots based compositors (such as __Sway__)
- Mir-based compositors (some may not enable the protocol by default and require `--add-wayland-extension zwlr_layer_shell_v1`)

Layer shell __is not supported__ on:
- Gnome-on-Wayland
- Any X11 desktop

## Using the Library
### Demo
`gtk-layer-demo` is built if examples are enabled. Its UI exposes all features of the library, and it's useful for testing layer shell support in compositors. Its code can be found in [examples/demo/](examples/demo/).

### C/C++
The easiest way to build against GTK Layer Shell is to use the `gtk-layer-shell-0` pkg-config package. Refer to your build system or the pkg-config docs for further instructions. [gtk-layer-shell.h](include/gtk-layer-shell.h) shows the full API, which can be used directly in C or C++. [examples/simple-example.c](examples/simple-example.c) is a minimal complete app written in C.

### Python
[examples/simple-example.py](examples/simple-example.py) contains sample Python code.

### Vala
[examples/vala-standalone](examples/vala-standalone) contains a minimal working standalone Vala project, see [the readme](examples/vala-standalone/README.md) for details.

### Rust
[@pentamassiv](https://github.com/pentamassiv) maintains [safe Rust bindings](https://github.com/pentamassiv/gtk-layer-shell-gir) and the [crates.io crate](https://crates.io/crates/gtk-layer-shell/). Rust examples can be found [here](https://github.com/pentamassiv/gtk-layer-shell-gir/tree/main/gtk-layer-shell/examples).

## Distro Packages
[![List of distros GTK Layer Shell is packaged for](https://repology.org/badge/vertical-allrepos/gtk-layer-shell.svg)](https://repology.org/project/gtk-layer-shell/versions)

## Building From Source
1. Clone this repo
2. Install build dependencies (see below)
3. `$ meson build -Dexamples=true -Ddocs=true -Dtests=true`
4. `$ ninja -C build`
5. `$ sudo ninja -C build install`
6. `$ sudo ldconfig`

### Build Dependencies
* [Meson](https://mesonbuild.com/) (>=0.45.1)
* [libwayland](https://gitlab.freedesktop.org/wayland/wayland) (>=1.10.0)
* [GTK3](https://www.gtk.org/) (>=3.22.0)
* [GObject introspection](https://gitlab.gnome.org/GNOME/gobject-introspection/)
* [GTK Doc](https://www.gtk.org/gtk-doc/) (only required if docs are enabled)
* [Vala](https://wiki.gnome.org/Projects/Vala) (only required if vapi is enabled)

To install these dependencies on Ubuntu 18.04 and later:
```
sudo apt install meson libwayland-dev libgtk-3-dev gobject-introspection libgirepository1.0-dev gtk-doc-tools valac
```

### Meson Options
* `-Dexamples` (default `false`): If to build the example C apps; gtk-layer-demo is installed if examples are built; The Vala example is never built with the rest of the project
* `-Ddocs` (default `false`): If to generate the docs
* `-Dtests` (default `false`): If to build the tests
* `-Dintrospection` (default: `true`): If to build GObject Introspection data (used for bindings to langauges other than C/C++)
* `-Dvapi` (default: `true`): If to build VAPI data (allows this library to be used in Vala). Requires `-Dintrospection=true`

### Running the Tests
* `ninja -C build test`

## Licensing
GTK Layer Shell is licensed under the GNU Lesser General Public License version 3.0 or any later version.

Most of the individual source files are licensed under MIT.

To prevent possible future confusion, all contributions must contain the following in the PR message: *By opening this pull request, I agree for my modifications to be licensed under whatever licenses are indicated at the start of the files I modified*

### Licensing Rationale
I want everyone to be able to use GTK Layer Shell however they desire, but parts of it are extracted from GTK. Therefore, the project as a whole is licensed under [GNU Lesser General Public License (LGPL) version 3](https://www.gnu.org/licenses/lgpl-3.0.en.html) or any later version (a newer version of the same license as GTK). See [LICENSE_LGPL.txt](LICENSE_LGPL.txt) and [LICENSE_GPL.txt](LICENSE_GPL.txt) for details. Almost all of the non-generated code, however, is licensed under [MIT](https://en.wikipedia.org/wiki/MIT_License) ([LICENSE_MIT.txt](LICENSE_MIT.txt)). At the top of each file should be a header that specifies which license applies to it. Please refer to that if in doubt.

### What This Means For You
This library can be linked against under similar terms as GTK itself, so licensing shouldn't be a problem for most potential users. Furthermore, most of the code within this library can be used in permissively licensed or proprietary projects.
