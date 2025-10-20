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
#include <fcntl.h>
#include <sys/stat.h>

// Time for each callback to run. 60ms is three frames and change
static int step_time = 60;

static int return_code = 0;
static int callback_index = 0;
static gboolean auto_continue = FALSE;
static gboolean complete = FALSE;
struct wl_display* wl_display = NULL;

char command_fifo_path[255] = {0};
char response_fifo_path[255] = {0};
static void init_paths() {
    const char* test_dir = getenv("GTKLS_TEST_DIR");
    if (test_dir) {
        char wayland_display[255] = {0};
        sprintf(wayland_display, "%s/gtkls-test-display", test_dir);
        setenv("WAYLAND_DISPLAY", wayland_display, TRUE);
        setenv("XDG_RUNTIME_DIR", test_dir, TRUE);
    } else {
        test_dir = getenv("XDG_RUNTIME_DIR");
    }
    if (!test_dir || strlen(test_dir) == 0) {
        FATAL_FMT("GTKLS_TEST_DIR or XDG_RUNTIME_DIR must be set");
    }
    ASSERT(strlen(test_dir) < 200);
    sprintf(command_fifo_path, "%s/gtkls-test-command", test_dir);
    sprintf(response_fifo_path, "%s/gtkls-test-response", test_dir);
}

void send_command(const char* command, const char* expected_response) {
    fprintf(stderr, "sending command: %s\n", command);

    ASSERT(strlen(response_fifo_path));
    int response_fd;
    mkfifo(response_fifo_path, 0666);

    ASSERT(strlen(command_fifo_path));
    int command_fd;
    ASSERT((command_fd = open(command_fifo_path, O_WRONLY)) >= 0);
    ASSERT(write(command_fd, command, strlen(command)) > 0);
    ASSERT(write(command_fd, "\n", 1) > 0);
    close(command_fd);

    fprintf(stderr, "awaiting response: %s\n", expected_response);
    ASSERT((response_fd = open(response_fifo_path, O_RDONLY)) >= 0);
#define BUFFER_SIZE 1024
    char buffer[BUFFER_SIZE];
    int length = 0;
    while (TRUE) {
        ASSERT(length < BUFFER_SIZE);
        char* c = buffer + length;
        ssize_t bytes_read = read(response_fd, c, 1);
        ASSERT(bytes_read > 0);
        if (*c == '\n') {
            *c = '\0';
            fprintf(stderr, "got: %s\n", buffer);
            ASSERT_STR_EQ(buffer, expected_response);
            break;
        } else {
            length++;
        }
    }
    close(response_fd);
#undef BUFFER_SIZE
}

static gboolean next_step(gpointer _data) {
    (void)_data;

    CHECK_EXPECTATIONS();

    if (test_callbacks[callback_index]) {
        fprintf(stderr, "\nBEGINNING OF SECTION %d\n", callback_index);
        test_callbacks[callback_index]();
        callback_index++;
        if (auto_continue) {
            fprintf(stderr, "ROUNDTRIPPING\n");
            wl_display_roundtrip(wl_display);
            g_timeout_add(step_time, next_step, NULL);
        }
        fprintf(stderr, "END OF SECTION\n\n");
    } else {
        fprintf(stderr, "\nBEGINNING OF CLEANUP\n");
        GList* toplevels = gtk_window_list_toplevels();
        while (toplevels) {
            gtk_widget_destroy(GTK_WIDGET(toplevels->data));
            toplevels = g_list_next(toplevels);
        }
        complete = TRUE;
        fprintf(stderr, "ROUNDTRIPPING\n");
        wl_display_roundtrip(wl_display);
        fprintf(stderr, "END OF TEST\n\n");
    }

    return FALSE;
}

GtkWindow* create_default_window()
{
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    GtkWidget *label = gtk_label_new("");
    gtk_label_set_markup(
        GTK_LABEL(label),
        "<span font_desc=\"20.0\">"
            "Layer shell test"
        "</span>");
    gtk_container_add(GTK_CONTAINER(window), label);
    gtk_container_set_border_width(GTK_CONTAINER(window), 12);
    return window;
}

static void continue_button_callback(GtkWidget *_widget, gpointer _data)
{
    (void)_widget; (void)_data;
    next_step(NULL);
}

static void create_debug_control_window()
{
    // Make a window with a continue button for debugging
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_layer_init_for_window(window);
    gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
    gtk_layer_set_margin(window, GTK_LAYER_SHELL_EDGE_BOTTOM, 200);
    gtk_layer_set_layer(window, GTK_LAYER_SHELL_LAYER_OVERLAY);
    GtkWidget* button = gtk_button_new_with_label("Continue ->");
    g_signal_connect (button, "clicked", G_CALLBACK(continue_button_callback), NULL);
    gtk_container_add(GTK_CONTAINER(window), button);
    gtk_widget_show_all(GTK_WIDGET(window));
    // This will only be called once, so leaking the window is fine
}

int main(int argc, char** argv) {
    EXPECT_MESSAGE(wl_display .get_registry);

    init_paths();
    gtk_init(0, NULL);
    wl_display = gdk_wayland_display_get_wl_display(gdk_display_get_default());
    ASSERT(wl_display);

    if (argc == 1) {
        // Run with a debug mode window that lets the user advance manually
        create_debug_control_window();
    } else if (argc == 2 && g_strcmp0(argv[1], "--auto") == 0) {
        // Run normally with a timeout
        auto_continue = TRUE;
    } else {
        g_critical("Invalid arguments to integration test");
        return 1;
    }

    next_step(NULL);
    while (!complete) g_main_context_iteration(NULL, TRUE);

    return return_code;
}
