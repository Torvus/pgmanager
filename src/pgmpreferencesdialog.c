#include "pgmpreferencesdialog.h"

#include "pgmtranslate.h"

static GList *pref_list = NULL;
static GHashTable *pref_hash = NULL;

static void current_preference_changed( GtkTreeView *view, gpointer data );
static GtkWidget * pgm_preferences_dialog_new( GtkWindow *parent );

void
pgm_preferences_dialog_register( PgmPreference *pref )
{
    pref_list = g_list_append( pref_list, pref );
}

static GtkWidget *
pgm_preferences_dialog_new( GtkWindow *parent )
{
    GtkWidget *dialog;
    GtkWidget *dialog_content;
    GtkWidget *h_paned;
    GtkWidget *tree_view_scrolled;
    GtkWidget *tree_view;
    GtkTreeViewColumn *tree_column;
    GtkCellRenderer *tree_column_renderer;
    GtkTreeStore *tree_store;
    GtkWidget *notebook;
    GtkTreeIter iter;
    GList *pref;

    if( !pref_hash )
        pref_hash = g_hash_table_new( g_str_hash, g_str_equal );

    dialog = gtk_dialog_new_with_buttons( _("Preferences"),
                                          parent,
                                          GTK_DIALOG_MODAL,
                                          GTK_STOCK_OK,
                                          GTK_RESPONSE_OK,
                                          GTK_STOCK_CANCEL,
                                          GTK_RESPONSE_CANCEL,
                                          NULL );
    dialog_content = gtk_dialog_get_content_area( GTK_DIALOG( dialog ) );

    h_paned = gtk_hpaned_new();
    gtk_container_add( GTK_CONTAINER( dialog_content ), h_paned );

    tree_view_scrolled = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( tree_view_scrolled ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

    tree_store = gtk_tree_store_new( 1, G_TYPE_STRING );
    tree_view = gtk_tree_view_new_with_model( GTK_TREE_MODEL( tree_store ) );
    gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( tree_view ), FALSE );
    gtk_container_add( GTK_CONTAINER( tree_view_scrolled ), tree_view );
    gtk_paned_add1( GTK_PANED( h_paned ), tree_view_scrolled );

    tree_column_renderer = gtk_cell_renderer_text_new();

    tree_column = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start( tree_column, tree_column_renderer, TRUE );
    gtk_tree_view_column_set_attributes( tree_column,
                                         tree_column_renderer,
                                         "text",
                                         0,
                                         NULL );
    gtk_tree_view_append_column( GTK_TREE_VIEW( tree_view ), tree_column );

    notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs( GTK_NOTEBOOK( notebook ), FALSE );
    gtk_notebook_set_show_border( GTK_NOTEBOOK( notebook ), FALSE );
    gtk_paned_add2( GTK_PANED( h_paned ), notebook );

    pref = pref_list;
    while( pref )
    {
        PgmPreference *preference = (PgmPreference*)pref->data;
        GtkWidget *pref_widget;
        GtkTreePath *path = NULL;
        gchar *key;
        gint page_num;

        if( preference->get_title && preference->get_widget )
        {
            pref_widget = preference->get_widget();
            gtk_tree_store_append( tree_store, &iter, NULL );
            gtk_tree_store_set( tree_store, &iter, 0, preference->get_title(), -1 );
            path = gtk_tree_model_get_path( GTK_TREE_MODEL( tree_store ), &iter );
            key = gtk_tree_path_to_string( path );
            page_num = gtk_notebook_append_page( GTK_NOTEBOOK( notebook ), pref_widget, NULL );
            g_hash_table_insert( pref_hash,
                                 key,
                                 GINT_TO_POINTER( page_num ) );
        }
        pref = g_list_next( pref );
    }

    gtk_widget_show_all( dialog_content );

    g_signal_connect( tree_view, "cursor-changed", G_CALLBACK( current_preference_changed ), notebook );

    return dialog;
}

static void
current_preference_changed( GtkTreeView *view, gpointer data )
{
    GtkTreePath *path;
    gchar *key;
    gint page_index;

    gtk_tree_view_get_cursor( GTK_TREE_VIEW( view ), &path, NULL );
    if( !path )
        return;

    key = gtk_tree_path_to_string( path );
    page_index = GPOINTER_TO_INT( g_hash_table_lookup( pref_hash, key ) );
    g_free( key );

    gtk_notebook_set_current_page( GTK_NOTEBOOK( data ), page_index );
}

void
pgm_preferences_dialog_show( GtkWindow *parent, GKeyFile *file )
{
    GList *pref;
    GtkWidget *dialog = pgm_preferences_dialog_new( parent );
    gint response;

    pref = pref_list;
    while( pref )
    {
        PgmPreference *preference = (PgmPreference*)pref->data;

        if( preference->load_config )
            preference->load_config( file );
        pref = g_list_next( pref );
    }

    response = gtk_dialog_run( GTK_DIALOG( dialog ) );
    if( response == GTK_RESPONSE_OK )
    {
        pref = pref_list;
        while( pref )
        {
            PgmPreference *preference = (PgmPreference*)pref->data;

            if( preference->save_config )
                preference->save_config( file );
            pref = g_list_next( pref );
        }
    }
    gtk_widget_destroy( dialog );
}
