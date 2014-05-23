#include <gtk/gtk.h>
#include "biji-webkit2-editor.h"

int
main (int   argc,
      char *argv[])
{
  GtkWidget *window;
  WebKitWebView *view;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Biji WebKit2 Editor");

  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  view = biji_webkit2_editor_new ();
  gtk_container_add (GTK_CONTAINER (window), view);
  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}