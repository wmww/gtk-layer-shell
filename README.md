# GTK Layer Shell

![GTK Layer Shell demo screenshot](https://i.imgur.com/dIuYcBM.png)

A library to write [GTK](https://www.gtk.org/) applications that use [Layer Shell](https://github.com/swaywm/wlr-protocols/blob/master/unstable/wlr-layer-shell-unstable-v1.xml). Layer Shell is a Wayland protocol for desktop shell components, such as panels, notifications and wallpapers. You can use it to anchor your windows to a corner or edge of the output, or stretch them across the entire output. This library only makes sense on Wayland compositors that support Layer Shell, and will not work on X11. It supports all Layer Shell features including popups and popovers (GTK popups Just Work™). Please open issues for any bugs you come across.

## To install
1. Clone this repo
2. Make sure gtk3 development packages and autotools are installed
3. Run `./autogen.sh`
4. Run `make`
5. Run `sudo make install`

## To use
* `gtk-layer-demo` is installed with this library. Its UI exposes all features of the library, and it's useful for testing layer shell support in compositors
* [gtk-layer-shell.h](include/gtk-layer-shell.h) has everything you need to use Layer Shell in a GTK app
* [example/example.c](example/example.c) is a minimal working app
* [demo/](demo/) contains a more complex app
* The easiest way to build is to use the `gtk-layer-shell-0` pkg-config package. Refer to your build system or the pkg-config docs for further instructions
