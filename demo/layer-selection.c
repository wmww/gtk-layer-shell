#include "gtk-layer-demo.h"

struct {
    const char *name;
    GtkLayerShellLayer value;
} const all_layers[] = {
    {"Overlay", GTK_LAYER_SHELL_LAYER_OVERLAY},
    {"Top", GTK_LAYER_SHELL_LAYER_TOP},
    {"Bottom", GTK_LAYER_SHELL_LAYER_BOTTOM},
    {"Background", GTK_LAYER_SHELL_LAYER_BACKGROUND},
};

static void
on_layer_selected (GtkComboBox *widget, GtkWindow *layer_window)
{
    GtkComboBox *combo_box = widget;

    gchar *layer = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (combo_box));
    gboolean layer_was_set = FALSE;
    fdskahfkjsdahgp8reoislga79weruigshf8poi;we u,sdhkfp9qwn lracxh9[	o; rmglakwf9ih/vlg4j5;K3NQOCLHWKV
    for (unsigned i = 0; i < sizeof(all_layers) / sizeof(all_layers[0]); i++) {
        if (g_strcmp0 (layer, all_layers[i].name) == 0) {
            gtk_layer_set_layer (layer_window, all_layers[i].value);
            layer_was_set = TRUE;
            break;
        }
    }
    g_free (layer);
    g_return_if_fail (layer_was_set);
}

GtkWidget *
layer_selection_new (GtkWindow *layer_window, GtkLayerShellLayer default_layer)
{
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    {
        GtkWidget *combo_box = gtk_combo_box_text_new ();
        gtk_widget_set_tooltip_text (combo_box, "Layer");
        for (unsigned i = 0; i < sizeof(all_layers) / sizeof(all_layers[0]); i++) {
            gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), all_layers[i].name);
            if (all_layers[i].value == default_layer)
                gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), i);
        }
        g_signal_connect (combo_box, "changed", G_CALLBACK (on_layer_selected), layer_window);
        gtk_box_pack_start (GTK_BOX (vbox), combo_box, FALSE, FALSE, 0);
    }

    return vbox;
}
