#include "gtk-layer-demo.h"

static GtkLayerShellLayer default_layer = GTK_LAYER_SHELL_LAYER_TOP;

static gboolean default_anchors[] = {FALSE, FALSE, FALSE, FALSE};
static int default_margins[] = {0, 0, 0, 0};

static gboolean default_auto_exclusive_zone = FALSE; // always set by command line option
static gboolean default_keyboard_interactivity = FALSE; // always set by command line option
static gboolean default_fixed_size = FALSE; // always set by command line option

const char *prog_name = "gtk-layer-demo";
const char *prog_summary = "A GTK application for demonstrating the functionality of the Layer Shell Wayland protocol";
const char *prog_details = "See https://github.com/wmww/gtk-layer-shell for more information, and to report bugs";

const char *anchor_edges_key = "anchor_edges";

gboolean layer_option_callback (const gchar *option_name, const gchar *value, void *data, GError **error);
gboolean anchor_option_callback (const gchar *option_name, const gchar *value, void *data, GError **error);
gboolean margin_option_callback (const gchar *option_name, const gchar *value, void *data, GError **error);

static const GOptionEntry options[] = {
    {
        .long_name = "layer",
        .short_name = 'l',
        .flags = G_OPTION_FLAG_NONE,
        .arg = G_OPTION_ARG_CALLBACK,
        .arg_data = (void *)&layer_option_callback,
        .description = "\"overlay\", \"top\", \"bottom\" or background (or \"o\", \"t\", \"b\" or \"g\")",
        .arg_description = NULL,
    },
    {
        .long_name = "anchor",
        .short_name = 'a',
        .flags = G_OPTION_FLAG_OPTIONAL_ARG,
        .arg = G_OPTION_ARG_CALLBACK,
        .arg_data = (void *)&anchor_option_callback,
        .description = "A sequence of 'l', 'r', 't' and 'b' to anchor to those edges, or \"0\" for no anchor",
        .arg_description = NULL,
    },
    {
        .long_name = "margin",
        .short_name = 'm',
        .flags = G_OPTION_FLAG_NONE,
        .arg = G_OPTION_ARG_CALLBACK,
        .arg_data = (void *)&margin_option_callback,
        .description = "Comma separated list of margin values, in the order LEFT,RIGHT,TOP,BOTTOM",
        .arg_description = NULL,
    },
    {
        .long_name = "exclusive",
        .short_name = 'e',
        .flags = G_OPTION_FLAG_NONE,
        .arg = G_OPTION_ARG_NONE,
        .arg_data = &default_auto_exclusive_zone,
        .description = "Enable auto exclusive zone",
        .arg_description = NULL,
    },
    {
        .long_name = "keyboard",
        .short_name = 'k',
        .flags = G_OPTION_FLAG_NONE,
        .arg = G_OPTION_ARG_NONE,
        .arg_data = &default_keyboard_interactivity,
        .description = "Enable keyboard interactivity",
        .arg_description = NULL,
    },
    {
        .long_name = "fixed-size",
        .short_name = 'f',
        .flags = G_OPTION_FLAG_NONE,
        .arg = G_OPTION_ARG_NONE,
        .arg_data = &default_fixed_size,
        .description = "Enable a fixed window size",
        .arg_description = NULL,
    },
    { NULL, 0, 0, 0, NULL, NULL, NULL }
};

gboolean
layer_option_callback (const gchar *_option_name, const gchar *value, void *_data, GError **error)
{
    (void)_option_name;
    (void)_data;

    if (g_strcmp0 (value, "overlay") == 0 || g_strcmp0 (value, "o") == 0) {
        default_layer = GTK_LAYER_SHELL_LAYER_OVERLAY;
    } else if (g_strcmp0 (value, "top") == 0 || g_strcmp0 (value, "t") == 0) {
        default_layer = GTK_LAYER_SHELL_LAYER_TOP;
    } else if (g_strcmp0 (value, "bottom") == 0 || g_strcmp0 (value, "b") == 0) {
        default_layer = GTK_LAYER_SHELL_LAYER_BOTTOM;
    } else if (g_strcmp0 (value, "background") == 0 || g_strcmp0 (value, "g") == 0) {
        default_layer = GTK_LAYER_SHELL_LAYER_BACKGROUND;
    } else {
        g_set_error (error,
                     G_OPTION_ERROR,
                     G_OPTION_ERROR_FAILED,
                     "Invalid layer \"%s\"", value);
        return FALSE;
    }
    return TRUE;
}

gboolean
anchor_option_callback (const gchar *_option_name, const gchar *value, void *_data, GError **error)
{
    (void)_option_name;
    (void)_data;

    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        default_anchors[i] = FALSE;
    }

    if (!value || !*value || g_strcmp0 (value, "0") == 0 || g_strcmp0 (value, "none") == 0) {
        return TRUE;
    }

    for (const char *c = value; *c; c++) {
        if (*c == 'l') {
            default_anchors[GTK_LAYER_SHELL_EDGE_LEFT] = TRUE;
        } else if (*c == 'r') {
            default_anchors[GTK_LAYER_SHELL_EDGE_RIGHT] = TRUE;
        } else if (*c == 't') {
            default_anchors[GTK_LAYER_SHELL_EDGE_TOP] = TRUE;
        } else if (*c == 'b') {
            default_anchors[GTK_LAYER_SHELL_EDGE_BOTTOM] = TRUE;
        } else {
            g_set_error (error,
                         G_OPTION_ERROR,
                         G_OPTION_ERROR_FAILED,
                         "Invalid anchor edge '%c'", *c);
            return FALSE;
        }
    }

    return TRUE;
}

gboolean
margin_option_callback (const gchar *_option_name, const gchar *value, void *_data, GError **error)
{
    (void)_option_name;
    (void)_data;

    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        if (!*value) {
            g_set_error (error,
                         G_OPTION_ERROR,
                         G_OPTION_ERROR_FAILED,
                         "Not enought comma separated arguments for margin");
            return FALSE;
        }
        char *end;
        long long margin = strtol (value, &end, 10);
        default_margins[i] = margin;
        if (end == value) {
            g_set_error (error,
                         G_OPTION_ERROR,
                         G_OPTION_ERROR_FAILED,
                         "Unable to parse margin");
            return FALSE;
        }
        value = end;
        if (*value == ',')
            value++;
    }
    if (*value) {
        g_set_error (error,
                     G_OPTION_ERROR,
                     G_OPTION_ERROR_FAILED,
                     "Too many comma separated arguments for margin");
        return FALSE;
    }
    return TRUE;
}

static void
process_args (int *argc, char ***argv)
{
    GOptionContext *context = g_option_context_new ("");
    g_option_context_add_group (context, gtk_get_option_group (TRUE));
    g_option_context_set_summary (context, prog_summary);
    g_option_context_set_description (context, prog_details);
    g_option_context_add_main_entries (context, options, NULL);

    GError *error = NULL;
    if (!g_option_context_parse (context, argc, argv, &error)) {
        g_printerr ("%s\n", error->message);
        g_error_free (error);
        g_option_context_free (context);
        exit (1);
    }

    g_option_context_free (context);
}

void
layer_window_update_orientation (GtkWindow *layer_window)
{
    ToplevelData *data = g_object_get_data (G_OBJECT (layer_window), anchor_edges_key);
    gboolean horiz = data->edges[GTK_LAYER_SHELL_EDGE_LEFT] == data->edges[GTK_LAYER_SHELL_EDGE_RIGHT];
    gboolean vert = data->edges[GTK_LAYER_SHELL_EDGE_TOP] == data->edges[GTK_LAYER_SHELL_EDGE_BOTTOM];
    WindowOrientation orientation = WINDOW_ORIENTATION_NONE;
    if (horiz && (!vert || (data->edges[GTK_LAYER_SHELL_EDGE_LEFT] && !data->edges[GTK_LAYER_SHELL_EDGE_TOP]))) {
        orientation = WINDOW_ORIENTATION_HORIZONTAL;
    } else if (vert && (!horiz || (data->edges[GTK_LAYER_SHELL_EDGE_TOP] && !data->edges[GTK_LAYER_SHELL_EDGE_LEFT]))) {
        orientation = WINDOW_ORIENTATION_VERTICAL;
    }
    if (orientation != data->orientation) {
        data->orientation = orientation;
        g_signal_emit_by_name(layer_window, "orientation-changed", orientation);
    }
}

static void
on_orientation_changed (GtkWindow *window, WindowOrientation orientation, ToplevelData *data)
{
    GtkOrientation orient_toplevel, orient_sub;
    orient_toplevel = GTK_ORIENTATION_HORIZONTAL;
    orient_sub = GTK_ORIENTATION_VERTICAL;

    switch (orientation) {
        case WINDOW_ORIENTATION_HORIZONTAL:
            orient_toplevel = GTK_ORIENTATION_HORIZONTAL;
            orient_sub = GTK_ORIENTATION_HORIZONTAL;
            break;
        case WINDOW_ORIENTATION_VERTICAL:
            orient_toplevel = GTK_ORIENTATION_VERTICAL;
            orient_sub = GTK_ORIENTATION_VERTICAL;
            break;
        case WINDOW_ORIENTATION_NONE:
            orient_toplevel = GTK_ORIENTATION_HORIZONTAL;
            orient_sub = GTK_ORIENTATION_VERTICAL;
            break;
    }
    gtk_orientable_set_orientation (GTK_ORIENTABLE (data->toplevel_box), orient_toplevel);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (data->first_box), orient_sub);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (data->second_box), orient_sub);
    gtk_window_resize (window, 1, 1); // force the window to shrink to the smallest size it can
}

static void
on_window_destroy(GtkWindow *_window, void *_data)
{
    (void)_window;
    (void)_data;

    gtk_main_quit ();
}

static GtkWidget *
layer_window_new ()
{
    GtkWindow *gtk_window = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_layer_init_for_window (gtk_window);

    ToplevelData *data = g_new0 (ToplevelData, 1);
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        data->edges[i] = default_anchors[i];
        gtk_layer_set_anchor (gtk_window, i, default_anchors[i]);
    }
    g_object_set_data_full (G_OBJECT (gtk_window), anchor_edges_key, data, g_free);
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++)
        gtk_layer_set_margin (gtk_window, i, default_margins[i]);
    gtk_layer_set_layer (gtk_window, default_layer);
    gtk_layer_set_exclusive_zone (gtk_window, default_auto_exclusive_zone);
    gtk_layer_set_keyboard_interactivity (gtk_window, default_keyboard_interactivity);
    gtk_layer_set_namespace (gtk_window, "demo");

    GtkWidget *centered_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (gtk_window), centered_vbox);
    GtkWidget *centered_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (centered_vbox), centered_hbox, TRUE, FALSE, 0);
    data->toplevel_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width (GTK_CONTAINER (data->toplevel_box), 16);
    gtk_box_pack_start (GTK_BOX (centered_hbox), data->toplevel_box, TRUE, FALSE, 0);
    {
        data->first_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
        gtk_box_pack_start (GTK_BOX (data->toplevel_box), data->first_box, FALSE, FALSE, 0);
        {
            GtkWidget *selections_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
            gtk_box_pack_start (GTK_BOX (selections_box), menu_bar_new (gtk_window), FALSE, FALSE, 0);
            gtk_box_pack_start (GTK_BOX (data->first_box), selections_box, FALSE, FALSE, 0);
            gtk_box_pack_start (GTK_BOX (selections_box),
                                monitor_selection_new (gtk_window),
                                FALSE, FALSE, 0);
            gtk_box_pack_start (GTK_BOX (selections_box),
                                layer_selection_new (gtk_window, default_layer),
                                FALSE, FALSE, 0);
        }{
            gtk_box_pack_start (GTK_BOX (data->first_box),
                                anchor_control_new (gtk_window, default_anchors),
                                FALSE, FALSE, 0);
        }
    }{
        data->second_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
        gtk_box_pack_start (GTK_BOX (data->toplevel_box), data->second_box, FALSE, FALSE, 0);
        {
            GtkWidget *toggles_box = mscl_toggles_new (gtk_window,
                                                 default_auto_exclusive_zone,
                                                 default_keyboard_interactivity,
                                                 default_fixed_size);
            gtk_box_pack_start (GTK_BOX (data->second_box),
                                toggles_box,
                                FALSE, FALSE, 0);
            gtk_box_pack_start (GTK_BOX (data->second_box),
                                margin_control_new (gtk_window, default_margins),
                                FALSE, FALSE, 0);
        }
    }

    g_signal_connect (gtk_window, "orientation-changed", G_CALLBACK (on_orientation_changed), data);
    data->orientation = -1; // invalid value will force anchor_edges_update_orientation to update
    layer_window_update_orientation (gtk_window);

    return GTK_WIDGET (gtk_window);
}

int
main (int argc, char **argv)
{
    g_set_prgname (prog_name);
    gtk_init (&argc, &argv);
    process_args (&argc, &argv);

    // The int arg is an enum of type WindowOrientation
    // Signal is emitted in anchor-control.c
    g_signal_new ("orientation-changed",
                  GTK_TYPE_WINDOW,
                  G_SIGNAL_RUN_FIRST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1, G_TYPE_INT);

    GtkWidget *initial_window = layer_window_new ();
    g_signal_connect (initial_window, "destroy", G_CALLBACK (on_window_destroy), NULL);
    gtk_widget_show_all (GTK_WIDGET (initial_window));

    gtk_main ();
}
