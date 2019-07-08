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
