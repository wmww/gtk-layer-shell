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

#include "integration-test-common.h"

static GtkWindow *window;

static void callback_0()
{
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface);

    window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    GtkWidget *menu_bar = gtk_menu_bar_new();
    gtk_container_add(GTK_CONTAINER(window), menu_bar);
    GtkWidget *menu_item = gtk_menu_item_new_with_label("Popup menu");
    gtk_widget_set_size_request(menu_item, 100, 100);
    gtk_container_add(GTK_CONTAINER(menu_bar), menu_item);
    GtkWidget* popup_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), popup_menu);
    GtkWidget *sub_item = gtk_menu_item_new_with_label("Menu item");
    gtk_widget_set_size_request(sub_item, 100, 100);
    gtk_menu_shell_append(GTK_MENU_SHELL(popup_menu), sub_item);
    GtkWidget *sub_sub_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(sub_item), sub_sub_menu);
    GtkWidget *sub_sub_item = gtk_menu_item_new_with_label("Sub item");
    gtk_widget_set_size_request(sub_sub_item, 100, 100);
    gtk_menu_shell_append(GTK_MENU_SHELL(sub_sub_menu), sub_sub_item);
    gtk_layer_init_for_window(window);
    gtk_widget_show_all(GTK_WIDGET(window));
}

static void callback_1()
{
    EXPECT_MESSAGE(xdg_surface .get_popup nil); // Menu
    send_command("click_latest_surface 50 50", "latest_surface_clicked");
}

static void callback_2()
{
    EXPECT_MESSAGE(xdg_surface .get_popup xdg_surface); // Sub-menu
    send_command("click_latest_surface 50 50", "latest_surface_clicked");
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
)
