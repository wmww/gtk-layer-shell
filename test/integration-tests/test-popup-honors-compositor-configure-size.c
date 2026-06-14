/* This entire file is licensed under MIT
 *
 * Copyright 2020 Sophie Winter
 * Copyright 2026 Brian Tarricone
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "integration-test-common.h"

#define PANEL_HEIGHT 120

static GtkWindow* window;

static void handle_button_press(GtkWidget *window, GdkEventButton *event)
{
    GtkWidget* menu = gtk_menu_new();
    gtk_menu_attach_to_widget(GTK_MENU(menu), window, NULL);

    for (gint i = 0; i < 100; ++i) {
        GtkWidget* item = gtk_menu_item_new_with_label("menu item");
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    }

    gtk_widget_show_all(menu);
    gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(window), GDK_GRAVITY_SOUTH, GDK_GRAVITY_NORTH, (GdkEvent *)event);
}

static void callback_0()
{
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .set_size 0 0);
    EXPECT_MESSAGE(.create_buffer 1920 120); // size must match DEFAULT_OUTPUT_WIDTH in common.h, PANEL_HEIGHT above

    window = create_default_window();
    gtk_widget_add_events(GTK_WIDGET(window), GDK_BUTTON_PRESS_MASK);

    gtk_layer_init_for_window(window);
    gtk_layer_set_layer(window, GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_set_namespace(window, "panel");
    gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
    gtk_layer_set_exclusive_zone(window, PANEL_HEIGHT);

    gtk_widget_set_size_request(GTK_WIDGET(window), -1, PANEL_HEIGHT);

    g_signal_connect(window, "button-press-event", G_CALLBACK(handle_button_press), NULL);

    gtk_widget_show_all(GTK_WIDGET(window));
}

static void callback_1()
{
    EXPECT_MESSAGE(xdg_wm_base .get_xdg_surface);
    EXPECT_MESSAGE(xdg_surface .get_popup nil);
    EXPECT_MESSAGE(zwlr_layer_surface_v1 .get_popup xdg_popup);
    EXPECT_MESSAGE(xdg_popup .grab);

    // The mock compositor always sends 500x500 in the xdg_popup configure event.
    EXPECT_MESSAGE(.set_window_geometry 500);

    send_command("click_latest_surface 50 25", "latest_surface_clicked");
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
)
