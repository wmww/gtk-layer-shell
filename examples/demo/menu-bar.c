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

#include "gtk-layer-demo.h"

static void
quit_activated(GSimpleAction *action, GVariant *parameter, GApplication *application) {
    g_application_quit (application);
}

void
set_up_menubar (GtkWindow *layer_window)
{
    GSimpleAction *act_quit = g_simple_action_new ("quit", NULL);
    g_action_map_add_action (G_ACTION_MAP (gtk_window_get_application (layer_window)), G_ACTION (act_quit));
    g_signal_connect (act_quit, "activate", G_CALLBACK (quit_activated), gtk_window_get_application (layer_window));

    GMenu *menubar = g_menu_new ();
    gtk_application_set_menubar (GTK_APPLICATION (gtk_window_get_application (layer_window)), G_MENU_MODEL (menubar));
    gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (layer_window), TRUE);
    GMenuItem *menu_item = g_menu_item_new ("Menu", NULL);
    g_menu_append_item (menubar, menu_item);
    GMenu *menu = g_menu_new ();
    GMenuItem *menu_item_quit = g_menu_item_new ("Quit", "app.quit");
    g_menu_append_item (menu, menu_item_quit);
    g_menu_item_set_submenu (menu_item, G_MENU_MODEL (menu));
    g_menu_append_item (menubar, menu_item);
    g_object_unref (menu_item_quit);
    g_object_unref (menu_item);
}
