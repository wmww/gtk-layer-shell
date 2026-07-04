/* This entire file is licensed under MIT
 *
 * Copyright 2026 Sophie Winter
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// Regression test for the popup mis-positioning fixed by the partial revert of 9f297865
// (see the PR "Restore GtkWindow move/resize in popup configure").
//
// Each time a popup is (re)mapped it gets a fresh xdg_surface, so
// xdg_surface.set_window_geometry must be re-sent before the first buffer is committed.
// The only thing that triggers sending it is a GTK size-allocate on the popup's toplevel
// after the xdg_popup.configure event. 9f297865 removed the gtk_window_move/resize calls
// from the configure handler, and gdk_window_move_resize alone does not queue a
// size-allocate when nothing changes, so on re-map of a reused GtkMenu the geometry was
// never sent and the compositor positioned the popup as if its drop shadow were part of
// the window.
//
// Reproducing this against the mock compositor requires recreating two properties of
// real compositors that hid the bug from the test suite:
//
// 1. enable_configure_delay: the mock normally replies to the map commit instantly, so
//    the client processes xdg_popup.configure while the frame clock still has the show's
//    own layout phase pending, and that leftover layout sends the geometry. Real
//    compositors reply after GTK's layout has already finished.
//
// 2. A shadowless menu (the CSS below): GTK sizes the popup's GdkWindow excluding the
//    CSD shadow while the configure handler resizes it to the shadow-inflated size. With
//    a shadow, those always disagree, and the resulting synthesized GDK configure event
//    causes a size-allocate that sends the geometry. With no shadow the handler's resize
//    is a true no-op, matching the do-nothing conditions observed on real compositors.

#include "integration-test-common.h"

#define PANEL_HEIGHT 40

static GtkWindow* window;
static GtkWidget* menu;

static void popup_menu()
{
    // No trigger event is available; GTK prints a warning (ignored by the test runner)
    // and pops the menu up anyway, which is all this test needs
    gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(window), GDK_GRAVITY_SOUTH, GDK_GRAVITY_NORTH, NULL);
}

static void callback_0()
{
    EXPECT_MESSAGE(zwlr_layer_shell_v1 .get_layer_surface);

    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "decoration { box-shadow: none; margin: 0; border: none; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    window = create_default_window();

    gtk_layer_init_for_window(window);
    gtk_layer_set_layer(window, GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);

    gtk_widget_set_size_request(GTK_WIDGET(window), -1, PANEL_HEIGHT);

    gtk_widget_show_all(GTK_WIDGET(window));

    menu = gtk_menu_new();
    gtk_menu_attach_to_widget(GTK_MENU(menu), GTK_WIDGET(window), NULL);
    for (gint i = 0; i < 3; ++i) {
        GtkWidget* item = gtk_menu_item_new_with_label("menu item");
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    }
    gtk_widget_show_all(menu);
}

static void callback_1()
{
    EXPECT_MESSAGE(xdg_surface .get_popup nil);
    EXPECT_MESSAGE(xdg_surface .ack_configure);
    EXPECT_MESSAGE(xdg_surface .set_window_geometry);
    EXPECT_MESSAGE(wl_surface .attach);
    EXPECT_MESSAGE(wl_surface .commit);

    send_command("enable_configure_delay", "configure_delay_enabled");
    popup_menu();
}

static void callback_2()
{
    EXPECT_MESSAGE(xdg_popup .destroy);
    EXPECT_MESSAGE(xdg_surface .destroy);

    gtk_menu_popdown(GTK_MENU(menu));
}

static void callback_3()
{
    // The regression: on re-map of the reused menu nothing triggered a size-allocate, so
    // set_window_geometry was not sent before the newly mapped popup's first buffer commit
    EXPECT_MESSAGE(xdg_surface .get_popup nil);
    EXPECT_MESSAGE(xdg_surface .ack_configure);
    EXPECT_MESSAGE(xdg_surface .set_window_geometry);
    EXPECT_MESSAGE(wl_surface .attach);
    EXPECT_MESSAGE(wl_surface .commit);

    popup_menu();
}

static void callback_4()
{
    EXPECT_MESSAGE(xdg_popup .destroy);
    EXPECT_MESSAGE(xdg_surface .destroy);

    gtk_menu_popdown(GTK_MENU(menu));
}

TEST_CALLBACKS(
    callback_0,
    callback_1,
    callback_2,
    callback_3,
    callback_4,
)
