#include "gtk-layer-demo.h"

gboolean
on_exclusive_zone_state_set (GtkToggleButton *_toggle_button, gboolean state, GtkWindow *layer_window)
{
    (void)_toggle_button;

    if (state) {
        gtk_layer_auto_exclusive_zone_enable (layer_window);
    } else {
        gtk_layer_set_exclusive_zone (layer_window, 0);
    }
    return FALSE;
}

gboolean
on_keyboard_interactivity_state_set (GtkToggleButton *_toggle_button, gboolean state, GtkWindow *layer_window)
{
    (void)_toggle_button;

    gtk_layer_set_keyboard_interactivity (layer_window, state);
    return FALSE;
}

gboolean
on_fixed_size_set (GtkToggleButton *_toggle_button, gboolean state, GtkWindow *layer_window)
{
    (void)_toggle_button;

    if (state) {
        gtk_widget_set_size_request (GTK_WIDGET (layer_window), 600, 500);
    } else {
        gtk_widget_set_size_request (GTK_WIDGET (layer_window), -1, -1);
    }
    gtk_window_resize (layer_window, 1, 1);
    return FALSE;
}

struct {
    const char *name;
    const char *tooltip;
    gboolean (*callback) (GtkToggleButton *toggle_button, gboolean state, GtkWindow *layer_window);
} const mscl_toggles[] = {
    {"Exclusive", "Create an exclusive zone when anchored", on_exclusive_zone_state_set},
    {"Keyboard", "Get keyboard events", on_keyboard_interactivity_state_set},
    {"Set Size", "Set a fixed window size", on_fixed_size_set},
};

GtkWidget *
mscl_toggles_new (GtkWindow *layer_window,
                  gboolean default_auto_exclusive_zone,
                  gboolean default_keyboard_interactivity,
                  gboolean default_fixed_size)
{
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    for (unsigned i = 0; i < sizeof (mscl_toggles) / sizeof (mscl_toggles[0]); i++) {
        GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
        gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
        {
            GtkWidget *label = gtk_label_new (mscl_toggles[i].name);
            gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
        }{
            GtkWidget *toggle = gtk_switch_new ();
            gtk_widget_set_tooltip_text (toggle, mscl_toggles[i].tooltip);
            gboolean default_value;
            if (mscl_toggles[i].callback == on_exclusive_zone_state_set)
                default_value = default_auto_exclusive_zone;
            else if (mscl_toggles[i].callback == on_keyboard_interactivity_state_set)
                default_value = default_keyboard_interactivity;
            else if (mscl_toggles[i].callback == on_fixed_size_set)
                default_value = default_fixed_size;
            else
                g_assert_not_reached ();
            gtk_switch_set_active (GTK_SWITCH (toggle), default_value);
            g_signal_connect (toggle, "state-set", G_CALLBACK (mscl_toggles[i].callback), layer_window);
            gtk_box_pack_end (GTK_BOX (hbox), toggle, FALSE, FALSE, 0);
        }
    }
    return vbox;
}
