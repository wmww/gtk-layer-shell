#include <stdio.h>
#include "gtk-layer-shell.h"

void some_func() {
    printf("some_func()\n");
}

void add_hello_button(GtkContainer *container)
{
    GtkWidget *button = gtk_button_new();
    gtk_button_set_label(GTK_BUTTON(button), "Hello");
    gtk_container_add(container, button);
}
