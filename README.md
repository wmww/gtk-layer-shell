# GTK Layer Shell

A library to write [GTK](https://www.gtk.org/) applications that use the [Layer Shell Wayland protocol](https://github.com/swaywm/wlr-protocols/blob/master/unstable/wlr-layer-shell-unstable-v1.xml). Layer Shell is for desktop shell components, such as panels, notifications and wallpapers.

## To install
1. Clone this repo
2. Run `./autogen.sh`
3. Run `make`
4. Run `sudo make install`

## To use
* [gtk-layer-shell.h](include/gtk-layer-shell.h) has everything you need. Just call `gtk_window_init_layer ()` on a GtkWindow before it is shown, and set the properties you want it to have.
* A complex working example can be found in [example/main.c](example/main.c)
* The easiest way to build is to use Package Config to include the gtk-layer-shell-0 package in your project
