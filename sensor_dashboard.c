#include <gtk/gtk.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

static const char *FIFO = "/tmp/pir_fifo";
static GtkWidget *label;

static gboolean on_io(GIOChannel *ch, GIOCondition cond, gpointer data) {
    if (cond & (G_IO_HUP | G_IO_ERR)) return FALSE;

    gchar *line = NULL; gsize len = 0; GError *err = NULL;
    if (g_io_channel_read_line(ch, &line, &len, NULL, &err) == G_IO_STATUS_NORMAL && line) {
        bool motion = strstr(line, "\"motion\": true") != NULL;
        gtk_label_set_markup(GTK_LABEL(label),
            motion ? "<span size='40000' weight='bold' foreground='white' background='green'>MOTION</span>"
                   : "<span size='40000' weight='bold' foreground='white' background='gray'>IDLE</span>");
        g_free(line);
    }
    if (err) g_error_free(err);
    return TRUE;
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    // ensure FIFO exists (driver also creates it)
    mkfifo(FIFO, 0666);
    int fd = open(FIFO, O_RDONLY | O_NONBLOCK);
    if (fd < 0) { perror("open fifo"); return 1; }

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "PIR Monitor");
    gtk_window_set_default_size(GTK_WINDOW(win), 640, 360);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label),
        "<span size='40000' weight='bold' background='gray' foreground='white'>IDLE</span>");
    gtk_container_add(GTK_CONTAINER(win), label);
    gtk_widget_show_all(win);

    GIOChannel *ch = g_io_channel_unix_new(fd);
    g_io_channel_set_encoding(ch, NULL, NULL);
    g_io_add_watch(ch, G_IO_IN | G_IO_HUP | G_IO_ERR, on_io, NULL);

    gtk_main();
    g_io_channel_unref(ch); close(fd);
    return 0;
}
