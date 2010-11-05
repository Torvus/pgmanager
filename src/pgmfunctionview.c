#include "pgmfunctionview.h"

#include "pgmsqleditor.h"
#include "pgmquery.h"
#include "pgmtranslate.h"
#include "pgmdatastore.h"
#include "pgmschemalistmodel.h"
#include "pgmlanguagelistmodel.h"

G_DEFINE_TYPE( PgmFunctionView, pgm_function_view, GTK_TYPE_NOTEBOOK )

static gchar ui[] =
	"<ui>"
	"	<toolbar name='MainToolBar'>"
	"		<toolitem action='ActionSave' />"
	"	</toolbar>"
	"</ui>";

static void pgm_function_view_init( PgmFunctionView *view );
static void pgm_function_view_class_init( PgmFunctionViewClass *klass );
static void pgm_function_view_update( PgmFunctionView *view );
static void pgm_function_view_type_changed( GtkCellRendererText *renderer, gchar *path, gchar *new_text, gpointer data );

static void on_save_activated( GtkAction *action, gpointer data );

static gboolean prepare_type_list( GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data );
static void parse_type_list( PgmFunctionView *view );

static GtkActionEntry actions[] =
{
	{ "ActionSave", GTK_STOCK_SAVE, "_Save", NULL, "Save changes", G_CALLBACK( on_save_activated ) }
};

static void
pgm_function_view_init( PgmFunctionView *view )
{}

static void
pgm_function_view_class_init( PgmFunctionViewClass *klass )
{}

GtkWidget*
pgm_function_view_new( PgmFunction *function )
{
	PgmFunctionView *view;
	GtkWidget *name_label;
	GtkWidget *name_hbox;
	GtkWidget *ret_label;
	GtkWidget *ret_hbox;
	GtkWidget *lang_label;
	GtkWidget *args_label;
	GtkWidget *args_scrolled;
	GtkWidget *alter_opt_widget;
	GtkWidget *alter_vbox;
	GtkWidget *alter_spacer;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	gchar *query;
	GValue val = {0};
	PgmDataStore *type_store;
	GtkListStore *schema_model;
	GtkListStore *language_model;

	view = g_object_new( PGM_TYPE_FUNCTION_VIEW, NULL );

	view->function = function;

	query = g_strdup_printf( "select format_type(oid, -1) "
							   "from pg_catalog.pg_type" );
	type_store = pgm_data_store_new( PGM_DATABASE( pgm_object_get_database( PGM_OBJECT( function ) ) ), query );

	schema_model = pgm_schema_list_model_new( PGM_OBJECT( function ) );
	language_model = pgm_language_list_model_new( PGM_OBJECT( function ) );

	view->ui_manager = gtk_ui_manager_new();
	view->actions = gtk_action_group_new( "PgmFunctionViewActions" );
	gtk_action_group_add_actions( view->actions, actions, G_N_ELEMENTS( actions ), view );
	gtk_ui_manager_insert_action_group( view->ui_manager, view->actions, 0 );
	gtk_ui_manager_add_ui_from_string( view->ui_manager, ui, -1, NULL );

	view->source_view_widget = gtk_vbox_new( FALSE, 0 );

	view->source_tab_bar = gtk_ui_manager_get_widget( view->ui_manager, "/MainToolBar" );
	gtk_box_pack_start( GTK_BOX( view->source_view_widget ), view->source_tab_bar, FALSE, FALSE, 0 );

	view->source_options_widget = gtk_table_new( 4, 3, FALSE );

	name_label = gtk_label_new( _("Name") );
	gtk_misc_set_alignment( GTK_MISC( name_label ), 0, 0.5 );
	gtk_table_attach_defaults( GTK_TABLE( view->source_options_widget ), name_label, 0, 1, 0, 1 );

	name_hbox = gtk_hbox_new( FALSE, 0 );

	view->source_schema_edit = gtk_combo_box_entry_new_with_model( GTK_TREE_MODEL( schema_model ), 0 );
	gtk_box_pack_start( GTK_BOX( name_hbox ), view->source_schema_edit, FALSE, FALSE, 0 );

	view->source_name_edit = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX( name_hbox ), view->source_name_edit, TRUE, TRUE, 0 );

	gtk_table_attach_defaults( GTK_TABLE( view->source_options_widget ), name_hbox, 1, 2, 0, 1 );

	ret_label = gtk_label_new( _("Return type") );
	gtk_misc_set_alignment( GTK_MISC( ret_label ), 0, 0.5 );
	gtk_table_attach_defaults( GTK_TABLE( view->source_options_widget ), ret_label, 0, 1, 1, 2 );

	ret_hbox = gtk_hbox_new( FALSE, 0 );

	view->source_ret_setof = gtk_check_button_new_with_label( _("Set of") );
	gtk_box_pack_start( GTK_BOX( ret_hbox ), view->source_ret_setof, FALSE, FALSE, 0 );

	view->source_ret_type = gtk_combo_box_entry_new_with_model( GTK_TREE_MODEL( type_store ), 0 );
	gtk_box_pack_start( GTK_BOX( ret_hbox ), view->source_ret_type, TRUE, TRUE, 0 );
	gtk_table_attach_defaults( GTK_TABLE( view->source_options_widget ), ret_hbox, 1, 2, 1, 2 );

	lang_label = gtk_label_new( _("Language") );
	gtk_misc_set_alignment( GTK_MISC( lang_label ), 0, 0.5 );
	gtk_table_attach_defaults( GTK_TABLE( view->source_options_widget ), lang_label, 0, 1, 2, 3 );

	view->source_lang = gtk_combo_box_entry_new_with_model( GTK_TREE_MODEL( language_model ), 0 );
	gtk_table_attach_defaults( GTK_TABLE( view->source_options_widget ), view->source_lang, 1, 2, 2, 3 );

	args_label = gtk_label_new( _("Arguments") );
	gtk_misc_set_alignment( GTK_MISC( args_label ), 0, 0 );
	gtk_table_attach_defaults( GTK_TABLE( view->source_options_widget ), args_label, 0, 1, 3, 4 );

	view->type_model = GTK_TREE_MODEL( gtk_list_store_new( 2, G_TYPE_INT, G_TYPE_STRING ) );

	args_scrolled = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( args_scrolled ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

	view->source_args = gtk_tree_view_new_with_model( view->type_model );
	gtk_container_add( GTK_CONTAINER( args_scrolled ), view->source_args );
	gtk_table_attach_defaults( GTK_TABLE( view->source_options_widget ), args_scrolled, 1, 2, 3, 4 );

	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes( "", renderer, "text", 0, NULL );

	gtk_tree_view_append_column( GTK_TREE_VIEW( view->source_args ), column );

	renderer = gtk_cell_renderer_combo_new();
	g_value_init( &val, GTK_TYPE_TREE_MODEL );
	g_value_set_object( &val, type_store );
	g_object_set_property( G_OBJECT( renderer ), "model", &val );
	g_value_unset( &val );
	g_value_init( &val, G_TYPE_INT );
	g_value_set_int( &val, 0 );
	g_object_set_property( G_OBJECT( renderer ), "text_column", &val );
	g_value_unset( &val );
	g_value_init( &val, G_TYPE_BOOLEAN );
	g_value_set_boolean( &val, TRUE );
	g_object_set_property( G_OBJECT( renderer ), "editable", &val );
	g_signal_connect( renderer, "edited", G_CALLBACK( pgm_function_view_type_changed ), view );
	/*g_value_set_boolean( &val, FALSE );
	g_object_set_property( G_OBJECT( renderer ), "has-entry", &val );*/

	column = gtk_tree_view_column_new_with_attributes( _("Type"), renderer, "text", 1, NULL );

	gtk_tree_view_append_column( GTK_TREE_VIEW( view->source_args ), column );

	alter_opt_widget = gtk_vbox_new( FALSE, 3 );
	gtk_table_attach_defaults( GTK_TABLE( view->source_options_widget ), alter_opt_widget, 2, 3, 0, 4 );

	alter_spacer = gtk_vbox_new( FALSE, 0 );
	gtk_box_pack_start( GTK_BOX( alter_opt_widget ), alter_spacer, TRUE, TRUE, 0 );
/*
	view->source_ret_non = gtk_check_button_new_with_label( _("Return null on null input") );
	gtk_box_pack_start( GTK_BOX( alter_opt_widget ), view->source_ret_non, FALSE, FALSE, 0 );

	view->source_ex_w_def_priv = gtk_check_button_new_with_label( _("Execute with definer's privileges") );
	gtk_box_pack_start( GTK_BOX( alter_opt_widget ), view->source_ex_w_def_priv, FALSE, FALSE, 0 );
*/
	view->source_opt_frame = gtk_frame_new( _("Replace multiple calls with single on same arguments") );
	gtk_box_pack_start( GTK_BOX( alter_opt_widget ), view->source_opt_frame, FALSE, FALSE, 0 );

	alter_vbox = gtk_vbox_new( TRUE, 0 );
	gtk_container_add( GTK_CONTAINER( view->source_opt_frame ), alter_vbox );

	view->source_immutable = gtk_radio_button_new_with_label( NULL, _("Always (IMMUTABLE)") );
	gtk_box_pack_start( GTK_BOX( alter_vbox ), view->source_immutable, TRUE, TRUE, 0 );

	view->source_stable = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON( view->source_immutable ), _("Within transaction (STABLE)") );
	gtk_box_pack_start( GTK_BOX( alter_vbox ), view->source_stable, TRUE, TRUE, 0 );

	view->source_volatile = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON( view->source_immutable ), _("Never (VOLATILE)") );
	gtk_box_pack_start( GTK_BOX( alter_vbox ), view->source_volatile, TRUE, TRUE, 0 );

	gtk_box_pack_start( GTK_BOX( view->source_view_widget ), view->source_options_widget, FALSE, FALSE, 0 );

	view->source_editor_scrolled = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( view->source_editor_scrolled ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

	view->source_editor = pgm_sql_editor_new();
	gtk_container_add( GTK_CONTAINER( view->source_editor_scrolled ), view->source_editor );

	gtk_box_pack_start( GTK_BOX( view->source_view_widget ), view->source_editor_scrolled, TRUE, TRUE, 0 );

	gtk_widget_show_all( view->source_view_widget );

	view->description_widget = gtk_vbox_new( FALSE, 0 );

	gtk_widget_show_all( view->description_widget );

	gtk_notebook_append_page( GTK_NOTEBOOK( view ), view->source_view_widget, NULL );
	gtk_notebook_set_tab_label_text( GTK_NOTEBOOK( view ), view->source_view_widget, _("Function") );
	gtk_notebook_append_page( GTK_NOTEBOOK( view ), view->description_widget, NULL );
	gtk_notebook_set_tab_label_text( GTK_NOTEBOOK( view ), view->description_widget, _("Description") );

	pgm_function_view_update( view );

	g_object_unref( language_model );
	g_object_unref( schema_model );
	g_free( query );
	return GTK_WIDGET( view );
}

void
pgm_function_view_set_source( PgmFunctionView *view, const gchar *source )
{
	pgm_sql_editor_set_text( PGM_SQL_EDITOR( view->source_editor ), source, -1 );
}

static void
pgm_function_view_update( PgmFunctionView *view )
{
		PgmQuery *query;
		gchar *schema_name;
		gint src_column;
		gint retset_column;
		gint volatile_column;
		gint rettype_column;
		gint lang_column;

		query = pgm_query_new( PGM_DATABASE( pgm_object_get_database( PGM_OBJECT( view->function ) ) ) );

		schema_name = pgm_object_get_name( pgm_object_get_schema( PGM_OBJECT( view->function ) ) );

		if( pgm_query_exec( query, "select p.prosrc, "
		                                  "p.proretset, "
										  "p.provolatile, "
										  "format_type(p.prorettype, -1) AS return_type, "
										  "l.lanname "
									 "from pg_proc p "
								"left join pg_language l ON p.prolang=l.oid "
                                "left join pg_namespace nsp ON p.pronamespace=nsp.oid "
                                    "where p.proname = '%s' "
                                      "and nsp.nspname = '%s' "
                                      "and oidvectortypes(p.proargtypes) = '%s'", view->function->name, schema_name, view->function->args) )
		{
			src_column = pgm_query_get_column_number( query, "prosrc" );
			retset_column = pgm_query_get_column_number( query, "proretset" );
			volatile_column = pgm_query_get_column_number( query, "provolatile" );
			rettype_column = pgm_query_get_column_number( query, "return_type" );
			lang_column = pgm_query_get_column_number( query, "lanname" );

			pgm_function_view_set_source( view, pgm_query_get_value( query, 0, src_column ) );
			gtk_entry_set_text( GTK_ENTRY( view->source_name_edit ), view->function->name );
			gtk_entry_set_text( GTK_ENTRY( GTK_BIN( view->source_ret_type )->child ), pgm_query_get_value( query, 0, rettype_column ) );
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( view->source_ret_setof ), pgm_query_get_value( query, 0, retset_column )[0] == 't' );
			gtk_entry_set_text( GTK_ENTRY( GTK_BIN( view->source_lang )->child ), pgm_query_get_value( query, 0, lang_column ) );
			gtk_entry_set_text( GTK_ENTRY( GTK_BIN( view->source_schema_edit )->child ), schema_name );
			switch( pgm_query_get_value( query, 0, volatile_column )[0] )
			{
				case 'v':
					gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( view->source_volatile ), TRUE );
					break;
				case 'i':
					gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( view->source_immutable ), TRUE );
					break;
				case 's':
					gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( view->source_stable ), TRUE );
					break;
			}
			parse_type_list( view );
		}
}

static void
on_save_activated( GtkAction *action, gpointer data )
{
	PgmFunctionView *view;
	PgmObject *schema;
	GString *query;

	view = PGM_FUNCTION_VIEW( data );

	schema = pgm_object_get_schema( PGM_OBJECT( view->function ) );

	query = g_string_new( "drop function " );

	g_string_append( query, pgm_object_get_name( schema ) );
	g_string_append( query, "." );
	g_string_append( query, pgm_object_get_name( PGM_OBJECT( view->function ) ) );
	g_string_append( query, "; create or replace function " );
	g_string_append( query, gtk_combo_box_get_active_text( GTK_COMBO_BOX( view->source_schema_edit ) ) );
	g_string_append( query, "." );
	g_string_append( query, gtk_entry_get_text( GTK_ENTRY( view->source_name_edit ) ) );
	g_string_append( query, "(" );
	gtk_tree_model_foreach( view->type_model, prepare_type_list, query );
	g_string_append( query, ") returns " );

	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( view->source_ret_setof ) ) )
		g_string_append( query, "setof " );

	g_string_append( query, gtk_entry_get_text( GTK_ENTRY( GTK_BIN( view->source_ret_type )->child ) ) );
	g_string_append( query, " as '" );
	g_string_append( query, pgm_sql_editor_get_text( PGM_SQL_EDITOR( view->source_editor ) ) );
	g_string_append( query, "' language '" );
	g_string_append( query, gtk_entry_get_text( GTK_ENTRY( GTK_BIN( view->source_lang )->child ) ) );
	g_string_append( query, "'" );

	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( view->source_immutable ) ) )
		g_string_append( query, " immutable" );
	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( view->source_stable ) ) )
		g_string_append( query, " stable" );
	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( view->source_volatile ) ) )
		g_string_append( query, " volatile" );

	g_debug( "save:\n%s", query->str );

	g_string_free( query, TRUE );
}

static gboolean
prepare_type_list( GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data )
{
	GString *str = data;
	GValue val = {0};
	static gboolean is_first = TRUE;

	gtk_tree_model_get_value( model, iter, 1, &val );

	if( !is_first )
		g_string_append( str, ", " );

	g_string_append( str, g_value_get_string( &val ) );
	is_first = FALSE;

	return FALSE;
}

static void
parse_type_list( PgmFunctionView *view )
{
	gchar *type_list_str;
	gchar **type_list;
	gchar *type;
	gint i = 0;
	GtkTreeIter iter;

	type_list_str = view->function->args;
	type_list = g_strsplit( type_list_str, ", ", -1 );

	type = type_list[ i ];
	while( type )
	{
		gtk_list_store_append( GTK_LIST_STORE( view->type_model ), &iter );
		gtk_list_store_set( GTK_LIST_STORE( view->type_model ), &iter, 0, i+1, 1, type, -1 );
		type = type_list[ ++i ];
	}

	g_strfreev( type_list );
}

static void
pgm_function_view_type_changed( GtkCellRendererText *renderer, gchar *path_str, gchar *new_text, gpointer data )
{
	PgmFunctionView *view = data;
	GtkTreeIter iter;
	GtkTreePath *path;

	path = gtk_tree_path_new_from_string( path_str );

	if( !path )
		return;

	gtk_tree_model_get_iter( view->type_model, &iter, path );

	gtk_list_store_set( GTK_LIST_STORE( view->type_model ), &iter, 1, new_text, -1 );
}
