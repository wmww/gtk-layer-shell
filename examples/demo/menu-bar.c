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
on_close_clicked (GtkMenuItem *_item, GtkWindow *layer_window)
{
    (void)_item;

    gtk_window_close (layer_window);
}

GtkWidget *
menu_bar_new (GtkWindow *layer_window)
{
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    {
        GtkWidget *menu_bar = gtk_menu_bar_new ();
        gtk_box_pack_start (GTK_BOX (vbox), menu_bar, FALSE, FALSE, 0);
        {
            GtkWidget *menu_item = gtk_menu_item_new_with_label ("Popup menu");
            gtk_container_add (GTK_CONTAINER (menu_bar), menu_item);
            {
                GtkWidget *submenu = gtk_menu_new ();
                gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), submenu);
                {
                    GtkWidget *nested_menu_item = gtk_menu_item_new_with_label ("Nested popup");
                    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), nested_menu_item);
                    {
                        GtkWidget *nested_menu = gtk_menu_new ();
                        gtk_menu_item_set_submenu (GTK_MENU_ITEM (nested_menu_item), nested_menu);
                        for (int i = 0; i < 3; i++)
                        {
                            GString *label = g_string_new ("");
                            g_string_printf (label, "Menu item %d", i);
                            GtkWidget *submenu_item = gtk_menu_item_new_with_label (label->str);
                            g_string_free (label, TRUE);
                            gtk_menu_shell_append (GTK_MENU_SHELL (nested_menu), submenu_item);
                        }
                        {
                            GtkWidget *submenu_item = gtk_menu_item_new_with_label ("Nested again");
                            gtk_menu_shell_append (GTK_MENU_SHELL (nested_menu), submenu_item);
                            {
                                GtkWidget *nested_menu = gtk_menu_new ();
                                gtk_menu_item_set_submenu (GTK_MENU_ITEM (submenu_item), nested_menu);
                                {
                                    GtkWidget *submenu_item = gtk_menu_item_new_with_label ("Final item");
                                    gtk_menu_shell_append (GTK_MENU_SHELL (nested_menu), submenu_item);
                                }
                            }
                        }
                    }
                }
                {
                    GtkWidget *close_item = gtk_menu_item_new_with_label ("Close");
                    g_signal_connect (close_item, "activate", G_CALLBACK (on_close_clicked), layer_window);
                    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), close_item);
                }
            }
        }
    }
    return vbox;
}
