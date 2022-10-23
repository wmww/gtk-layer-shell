'''
MIT License

Copyright 2020 Sophie Winter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

'''
The list of GTK structures to build header for
It is assumed these are the 'typedef' names, and the actual struct names are the same with an underscore prepended
Adding a name here is all that's needed to start building it's header (searching, naming the header, etc are all done automatically)
'''
STRUCT_LIST = [
    'GdkWindow',
    'GdkWindowImplWayland',
    'GdkWindowImplWaylandClass',
    'GdkWindowImpl',
    'GdkWindowImplClass',
    'GdkWaylandSeat',
    'GdkWaylandPointerData',
    'GdkWaylandPointerFrameData',
    'GdkWaylandTouchData',
    'GdkWaylandTabletData',
]

MIN_SUPPORTED_GTK = '3.22.0' # same format as the tag names
MAX_SUPPORTED_GTK = '3.70.0' # they randomly jumped to 3.89.1 for beta 4.0 releases
