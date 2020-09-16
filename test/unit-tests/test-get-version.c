/* This entire file is licensed under MIT
 *
 * Copyright 2020 William Wold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include "gtk-layer-shell.h"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Incorrect number of arguments (%d)", argc);
        exit(1);
    }

    char version_string[1024];
    sprintf(
        version_string,
        "%d.%d.%d",
        gtk_layer_get_major_version(),
        gtk_layer_get_minor_version(),
        gtk_layer_get_micro_version());

    const char* version_arg = argv[1];

    if (strcmp(version_arg, version_string) != 0)
    {
        fprintf(stderr, "Version provided by GTK Layer Shell: %s\n", version_string);
        fprintf(stderr, "Version sent to this test via argument: %s\n", version_arg);
        exit(1);
    }

    return 0;
}
