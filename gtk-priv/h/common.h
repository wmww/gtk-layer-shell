/* This entire file is licensed under MIT
 *
 * Copyright 2020 Sophie Winter
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef GTK_PRIV_COMMON_H
#define GTK_PRIV_COMMON_H

void gtk_priv_assert_gtk_version_valid() {
    if (gtk_get_major_version() != 3 || gtk_get_minor_version() < 22) {
        g_error(
            "gtk-layer-shell only supports GTK3 >= v3.22.0 (you have v%d.%d.%d)",
            gtk_get_major_version(),
            gtk_get_minor_version(),
            gtk_get_micro_version());
        g_abort();
    }
}

void gtk_priv_warn_gtk_version_may_be_unsupported() {
    static gboolean shown = FALSE;
    if (shown)
        return;
    if (getenv("GTK_LAYER_SHELL_UNSUPPORTED_GTK_WARNING")) {
        g_warning(
            "gtk-layer-shell v%d.%d.%d may not work on GTK v%d.%d.%d. "
            "If you experience crashes, check "
            "https://github.com/wmww/gtk-layer-shell/blob/master/compatibility.md",
            GTK_LAYER_SHELL_MAJOR,
            GTK_LAYER_SHELL_MINOR,
            GTK_LAYER_SHELL_MICRO,
            gtk_get_major_version(),
            gtk_get_minor_version(),
            gtk_get_micro_version());
    }
    shown = TRUE;
}

#endif // GTK_PRIV_COMMON_H
