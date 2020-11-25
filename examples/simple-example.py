'''This entire file is licensed under MIT.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import gi
gi.require_version('Gtk', '3.0')

try:
    gi.require_version('GtkLayerShell', '0.1')
except ValueError:
    import sys
    raise RuntimeError('\n\n' +
        'If you haven\'t installed GTK Layer Shell, you need to point Python to the\n' +
        'library by setting GI_TYPELIB_PATH and LD_LIBRARY_PATH to <build-dir>/src/.\n' +
        'For example you might need to run:\n\n' +
        'GI_TYPELIB_PATH=build/src LD_LIBRARY_PATH=build/src python3 ' + ' '.join(sys.argv))

from gi.repository import Gtk, GtkLayerShell

window = Gtk.Window()
label = Gtk.Label(label='GTK Layer Shell with Python!')
window.add(label)

GtkLayerShell.init_for_window(window)
GtkLayerShell.auto_exclusive_zone_enable(window)
GtkLayerShell.set_margin(window, GtkLayerShell.Edge.TOP, 10)
GtkLayerShell.set_margin(window, GtkLayerShell.Edge.BOTTOM, 10)
GtkLayerShell.set_anchor(window, GtkLayerShell.Edge.BOTTOM, 1)

window.show_all()
window.connect('destroy', Gtk.main_quit)
Gtk.main()
