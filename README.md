# GTK Layer Shell

A library to write [GTK](https://www.gtk.org/) applications that use [Layer Shell](https://github.com/swaywm/wlr-protocols/blob/master/unstable/wlr-layer-shell-unstable-v1.xml). Layer Shell is a Wayland protocol for desktop shell components, such as panels, notifications and wallpapers. You can use it to anchor your windows to a a corner or edge of the output, or stretch them across the entire output. This library only makes sense on Wayland, and will not work on X11. It is highly experimental, so don't expect everything to work perfectly, and please open issues for any bugs you come across.

## To install
1. Clone this repo
2. Run `./autogen.sh`
3. Run `make`
4. Run `sudo make install`

## To use
* [gtk-layer-shell.h](include/gtk-layer-shell.h) has everything you need. Just call `gtk_window_init_layer ()` on a GtkWindow before it is shown, and set the properties you want it to have.
* A complex working example can be found in [example/main.c](example/main.c)
* The easiest way to build is to use Package Config to include the gtk-layer-shell-0 package in your project
