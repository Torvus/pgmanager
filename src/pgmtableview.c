#include "pgmtableview.h"

#include <glib-object.h>

#include "pgmdataview.h"
#include "pgmdatastore.h"

G_DEFINE_TYPE( PgmTableView, pgm_table_view, GTK_TYPE_VBOX )

static gchar tool_bar_str[] =
	"<ui>"
	"	<toolbar name='MainToolBar'>"
/*	"		<toolitem action='ActionSave' />"
	"		<separator />"*/
	"		<toolitem action='ActionRefresh' />"
/*	"		<toolitem action='ActionUndo' />"*/
	"	</toolbar>"
	"</ui>";

static void pgm_table_view_init( PgmTableView *view );
static void pgm_table_view_class_init( PgmTableViewClass *klass );
static gboolean pgm_table_view_is_string_type( GType column_type );

static void on_refresh_activated( GtkAction *action, gpointer data );
static gboolean on_data_view_edited( PgmDataView *view, gint row, gint column, gchar *new_text, gpointer data );

static GtkActionEntry actions[] =
{
	/*{ "ActionSave", GTK_STOCK_SAVE, "Save", NULL, "Save changes" },*/
	{ "ActionRefresh", GTK_STOCK_REFRESH, "Refresh", "<Control>R", "Refresh data", G_CALLBACK( on_refresh_activated ) },
	/*{ "ActionUndo", GTK_STOCK_UNDO, "Undo", NULL, "Undo changes" }*/
};

static void
pgm_table_view_init( PgmTableView *view )
{}

static void
pgm_table_view_class_init( PgmTableViewClass *klass )
{}

GtkWidget*
pgm_table_view_new( PgmTable *table )
{
	PgmTableView *view;
	gchar *query;

	view = g_object_new( PGM_TYPE_TABLE_VIEW, "homogeneous", FALSE, "spacing", 0, NULL );

	view->table_name = pgm_object_get_name( PGM_OBJECT( table ) );
	view->schema_name = pgm_object_get_name( pgm_object_get_schema( PGM_OBJECT( table ) ) );

	view->ui_manager = gtk_ui_manager_new();

	view->action_group = gtk_action_group_new( "PgmTableViewActions" );
	gtk_action_group_add_actions( view->action_group, actions, G_N_ELEMENTS( actions ), view );

	gtk_ui_manager_insert_action_group( view->ui_manager, view->action_group, 0 );
	gtk_ui_manager_add_ui_from_string( view->ui_manager, tool_bar_str, -1, NULL );

	view->tool_bar = gtk_ui_manager_get_widget( view->ui_manager, "/MainToolBar" );

	gtk_box_pack_start( GTK_BOX( view ), view->tool_bar, FALSE, FALSE, 0 );

	/*view->query = pgm_query_new( PGM_DATABASE( pgm_object_get_database( PGM_OBJECT( table ) ) ) );
	pgm_query_exec( view->query, "select * from %s.%s;", view->schema_name, view->table_name );*/
	query = g_strdup_printf( "select * from %s.%s;", view->schema_name, view->table_name );
	view->data_store = GTK_TREE_MODEL( pgm_data_store_new( PGM_DATABASE( pgm_object_get_database( PGM_OBJECT( table ) ) ), query ) );
	g_free( query );
	view->data_view = pgm_data_view_new_with_store( PGM_DATA_STORE( view->data_store ) );

	view->data_scrolled = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( view->data_scrolled ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

	gtk_container_add( GTK_CONTAINER( view->data_scrolled ), view->data_view );

	gtk_box_pack_start( GTK_BOX( view ), view->data_scrolled, TRUE, TRUE, 0 );

	pgm_data_view_set_editable( PGM_DATA_VIEW( view->data_view ), TRUE );

	g_signal_connect( view->data_view, "edited", G_CALLBACK( on_data_view_edited ), view );

	return GTK_WIDGET( view );
}

static void
on_refresh_activated( GtkAction *action, gpointer data )
{
	PgmTableView *view = PGM_TABLE_VIEW( data );

	pgm_data_store_update( PGM_DATA_STORE( view->data_store ) );
	/*pgm_query_reexec( view->query );*/
}

static gboolean
on_data_view_edited( PgmDataView *view, gint row, gint column, gchar *new_text, gpointer data )
{
	PgmTableView *table_view = PGM_TABLE_VIEW( data );
	GtkTreePath *path;
	GtkTreeIter iter;
	GValue value = {0};
	GValue tmp_value = {0};
	/*gchar *text;*/
	gint col;
	gchar *column_name;
	GString *query_str;
	gboolean need_and = FALSE;
	PgmQuery *query;
	GType column_type;

	/*if( new_text[ 0 ] == 0 )
		return FALSE;*/

	g_value_init( &tmp_value, G_TYPE_STRING );

	path = gtk_tree_path_new();
	gtk_tree_path_append_index( path, row );
	gtk_tree_model_get_iter( table_view->data_store, &iter, path );
	gtk_tree_model_get_value( table_view->data_store, &iter, column, &value );
	g_value_transform( &value, &tmp_value );
	/*text = g_strdup_value_contents( &value );*/
	if( g_strcmp0( g_value_get_string( &tmp_value ), new_text ) == 0 )
	{
		/*g_free( text );*/
		return FALSE;
	}
	column_name = pgm_data_store_get_column_title( PGM_DATA_STORE( table_view->data_store ), column );
	column_type = gtk_tree_model_get_column_type( table_view->data_store, column );
	query_str = g_string_new( NULL );

	if( pgm_table_view_is_string_type( column_type ) && new_text[ 0 ] != 0 )
	{
		g_string_printf( query_str, "update %s.%s set %s = '%s' where ",
						 table_view->schema_name,
						 table_view->table_name,
						 column_name,
						 new_text );
	}
	else
	{
		g_string_printf( query_str, "update %s.%s set %s = %s where ",
						 table_view->schema_name,
						 table_view->table_name,
						 column_name,
						 new_text && new_text[0] != 0 ? new_text : "null" );
	}
	for( col = 0; col < gtk_tree_model_get_n_columns( table_view->data_store ); col++ )
	{
		GValue tmp_val = {0};
		GValue tmp_str_val = {0};
		const gchar *tmp_val_str;

		/*if( col == column )
			continue;*/

		if( need_and )
			g_string_append( query_str, " and " );

		g_value_init( &tmp_str_val, G_TYPE_STRING );

		gtk_tree_model_get_value( table_view->data_store, &iter, col, &tmp_val );
		/*tmp_val_str = g_strdup_value_contents( &tmp_val );*/
		g_value_transform( &tmp_val, &tmp_str_val );
		tmp_val_str = g_value_get_string( &tmp_str_val );

		column_name = pgm_data_store_get_column_title( PGM_DATA_STORE( table_view->data_store ), col );
		g_string_append( query_str, column_name );

		column_type = gtk_tree_model_get_column_type( table_view->data_store, col );

		if( g_strcmp0( tmp_val_str, "#<!-NULL->#" ) == 0 )
		{
			g_string_append( query_str, " is null " );
		}
		else
		{
			g_string_append( query_str, " = " );
			if( pgm_table_view_is_string_type( column_type ) )
				g_string_append( query_str, "'" );
			g_string_append( query_str, tmp_val_str );
			if( pgm_table_view_is_string_type( column_type ) )
				g_string_append( query_str, "'" );
		}
		need_and = TRUE;
		/*g_free( tmp_val_str );*/
	}
	g_debug( "update query = %s", query_str->str );

	query = pgm_query_new( PGM_DATA_STORE( table_view->data_store )->query->database );

	pgm_query_exec( query, "%s", query_str->str );

	g_string_free( query_str, TRUE );
	gtk_tree_path_free( path );
	/*g_free( text );*/

	pgm_data_store_update( PGM_DATA_STORE( table_view->data_store ) );
	/*pgm_query_reexec( table_view->query );*/

	return TRUE;
}

static gboolean
pgm_table_view_is_string_type( GType type )
{
	return !( type == G_TYPE_BOOLEAN ||
			  type == G_TYPE_INT ||
			  type == G_TYPE_LONG ||
			  type == G_TYPE_ULONG ||
			  type == G_TYPE_DOUBLE ||
			  type == G_TYPE_FLOAT );

}
