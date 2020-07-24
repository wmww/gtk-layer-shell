#include "gtk-layer-demo.h"

GtkWidget *
version_info_new ()
{
    gchar *version_str = g_strdup_printf("GTK v%d.%d.%d\nage: %d/%d",
                                         gtk_get_major_version (),
                                         gtk_get_minor_version (),
                                         gtk_get_micro_version (),
                                         gtk_get_binary_age (),
                                         gtk_get_interface_age ());
    GtkWidget *version_info = gtk_label_new(version_str);
    g_free(version_str);

    gtk_label_set_justify(GTK_LABEL(version_info), GTK_JUSTIFY_CENTER);
    gtk_label_set_xalign(GTK_LABEL(version_info), 0.5);
    gtk_label_set_yalign(GTK_LABEL(version_info), 0.25);
    return version_info;
}
