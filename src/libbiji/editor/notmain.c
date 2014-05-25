#include <gtk/gtk.h>
#include "biji-webkit2-editor.h"

static void
button_cb (GtkWidget *button, gpointer user_data)
{
  GtkWindow *window = user_data;
  BijiWebkit2Editor *editor = g_object_get_data ((GObject*)window, "webkit2-editor");
  // biji_webkit2_editor_cut (view);
  // biji_webkit2_editor_apply_format (editor, BIJI_STRIKE);
  biji_webkit2_editor_get_selection (editor);
}

int
main (int   argc,
      char *argv[])
{
  GtkWidget *window;
  BijiWebkit2Editor *editor;
  GtkWidget *box, *button;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Biji WebKit2 Editor");

  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  gtk_container_set_border_width (GTK_CONTAINER (box), 6);
  gtk_container_add (GTK_CONTAINER (window), box);

  editor = biji_webkit2_editor_new ();
  g_object_set_data ((GObject*)window, "webkit2-editor", editor);
  gtk_box_pack_start (GTK_BOX (box), GTK_WIDGET (WEBKIT_WEB_VIEW (editor)), TRUE, TRUE, 0);

  button = gtk_button_new_with_label ("copy");
  g_signal_connect (button, "clicked", G_CALLBACK (button_cb), window);
  gtk_box_pack_start (GTK_BOX (box), button, TRUE, TRUE, 0);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}