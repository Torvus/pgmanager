#ifndef PGMMAINWINDOW_H
#define PGMMAINWINDOW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

GtkWidget*    pgm_main_window_new                     ();
GdkPixbuf*    pgm_main_window_render_icon             ( const gchar *stock_id );
GtkTreeStore* pgm_main_window_get_tree_store          ();
gboolean      pgm_main_window_row_expanded            ( GtkTreePath *path );
gboolean      pgm_main_window_expand_row              ( GtkTreePath *path, gboolean expand_all );
gboolean      pgm_main_window_collapse_row            ( GtkTreePath *path );
gint          pgm_main_window_show_message_dialog     ( GtkMessageType type, GtkButtonsType buttons, const gchar *message_format, ... );
void          pgm_main_window_append_widget           ( GtkWidget *widget, const gchar *name, const gchar *stock_id );
GtkWidget*    pgm_main_window_get_file_chooser_dialog ( const gchar *title, GtkFileChooserAction action, const gchar *first_button_text, ... );
void          pgm_main_window_add_accel_group         ( GtkAccelGroup *accel_group );
void          pgm_main_window_start_wait              ();
void          pgm_main_window_stop_wait               ();
void          pgm_main_window_start_wait_for_widget   ( GtkWidget *widget );
void          pgm_main_window_stop_wait_for_widget    ( GtkWidget *widget );
void          pgm_main_window_save_config             ();
GKeyFile*     pgm_main_window_get_config              ();

G_END_DECLS

#endif /* PGMMAINWINDOW_H */
