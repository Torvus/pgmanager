#include "pgmqueryeditor.h"

#include "pgmdatastore.h"
#include "pgmdataview.h"
#include "pgmtoolcombobox.h"
#include "pgmdatabaselistmodel.h"
#include "pgmmainwindow.h"
#include "pgmstock.h"

G_DEFINE_TYPE( PgmQueryEditor, pgm_query_editor, GTK_TYPE_VBOX )

static void pgm_query_editor_init( PgmQueryEditor *editor );
static void pgm_query_editor_class_init( PgmQueryEditorClass *klass );

static void pgm_query_editor_can_undo_changed( PgmSqlEditor *editor, gpointer data );
static void pgm_query_editor_can_redo_changed( PgmSqlEditor *editor, gpointer data );

static void on_new_file_activated( GtkAction *action, gpointer data );
static void on_open_file_activated( GtkAction *action, gpointer data );
static void on_save_file_activated( GtkAction *action, gpointer data );
static void on_undo_activated( GtkAction *action, gpointer data );
static void on_redo_activated( GtkAction *action, gpointer data );
static void on_execute_activated( GtkAction *action, gpointer data );
static void on_cut_activated( GtkAction *action, gpointer data );
static void on_copy_activated( GtkAction *action, gpointer data );
static void on_paste_activated( GtkAction *action, gpointer data );

static gchar tool_bar_ui_str[] =
	"<ui>"
	"	<toolbar name='ToolMain'>"
	"		<toolitem name='NewFile' action='ActionNewFile' />"
	"		<toolitem name='OpenFile' action='ActionOpenFile' />"
	"		<toolitem name='SaveFile' action='ActionSaveFile' />"
	"		<separator />"
	"		<toolitem name='Cut' action='ActionCut' />"
	"		<toolitem name='Copy' action='ActionCopy' />"
	"		<toolitem name='Paste' action='ActionPaste' />"
	"		<separator />"
	"		<toolitem name='Undo' action='ActionUndo' />"
	"		<toolitem name='Redo' action='ActionRedo' />"
	"		<separator />"
	"		<toolitem name='Execute' action='ActionExecute' />"
	"		<separator />"
	"	</toolbar>"
	"</ui>";

static GtkActionEntry tool_actions[] =
{
	{ "ActionNewFile", GTK_STOCK_NEW, "_New", "<Control>N", "New file", G_CALLBACK( on_new_file_activated ) },
	{ "ActionOpenFile", GTK_STOCK_OPEN, "_Open", "<Control>O", "Open file", G_CALLBACK( on_open_file_activated ) },
	{ "ActionSaveFile", GTK_STOCK_SAVE, "_Save", "<Control>S", "Save file", G_CALLBACK( on_save_file_activated ) },
	{ "ActionUndo", GTK_STOCK_UNDO, "_Undo", "<Control>Z", "Undo", G_CALLBACK( on_undo_activated ) },
	{ "ActionRedo", GTK_STOCK_REDO, "_Redo", "<Control>Y", "Redo", G_CALLBACK( on_redo_activated ) },
	{ "ActionExecute", PGM_STOCK_EXECUTE, "_Execute", "F5", "Execute", G_CALLBACK( on_execute_activated ) },
	{ "ActionCut", GTK_STOCK_CUT, "_Cut", NULL, "Cut", G_CALLBACK( on_cut_activated ) },
	{ "ActionCopy", GTK_STOCK_COPY, "_Copy", NULL, "Copy", G_CALLBACK( on_copy_activated ) },
	{ "ActionPaste", GTK_STOCK_PASTE, "_Paste", NULL, "Paste", G_CALLBACK( on_paste_activated ) }
};

static void
pgm_query_editor_init( PgmQueryEditor *editor )
{}

static void
pgm_query_editor_class_init( PgmQueryEditorClass *klass )
{}

GtkWidget*
pgm_query_editor_new( PgmServerList *list, PgmObject *current_database )
{
	GtkWidget *source_scrolled;
	GtkWidget *result_scrolled;
	PgmQueryEditor *editor;
	GtkAction *action;
	GError *error = NULL;
	GtkTreeIter iter;

	editor = g_object_new( PGM_TYPE_QUERY_EDITOR, NULL );

	g_assert( editor != NULL );

	editor->server_list = list;

	editor->ui_manager = gtk_ui_manager_new();
	editor->actions = gtk_action_group_new( "PgmQueryEditorActions" );
	gtk_action_group_add_actions( editor->actions, tool_actions, G_N_ELEMENTS( tool_actions ), editor );
	gtk_ui_manager_insert_action_group( editor->ui_manager, editor->actions, 0 );
	gtk_ui_manager_add_ui_from_string( editor->ui_manager, tool_bar_ui_str, -1, &error );
	gtk_ui_manager_ensure_update( editor->ui_manager );
	pgm_main_window_add_accel_group( gtk_ui_manager_get_accel_group( editor->ui_manager ) );

	editor->tool_bar = gtk_ui_manager_get_widget( editor->ui_manager, "/ToolMain" );
	gtk_toolbar_set_show_arrow( GTK_TOOLBAR( editor->tool_bar ), TRUE );
	gtk_box_pack_start( GTK_BOX( editor ), editor->tool_bar, FALSE, FALSE, 0 );

	editor->db_list_model = GTK_TREE_MODEL( pgm_database_list_model_new( list ) );
	editor->db_combo_box = pgm_tool_combo_box_new_with_model( editor->db_list_model, 0 );
	if( pgm_database_list_model_get_database_iter( PGM_DATABASE_LIST_MODEL( editor->db_list_model ), current_database, &iter ) )
		pgm_tool_combo_box_set_active( PGM_TOOL_COMBO_BOX( editor->db_combo_box ), &iter );
	gtk_toolbar_insert( GTK_TOOLBAR( editor->tool_bar ), editor->db_combo_box, -1 );

	editor->paned = gtk_vpaned_new();
	gtk_box_pack_start( GTK_BOX( editor ), editor->paned, TRUE, TRUE, 0 );

	source_scrolled = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( source_scrolled ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

	editor->source_view = pgm_sql_editor_new();

	gtk_container_add( GTK_CONTAINER( source_scrolled ), GTK_WIDGET( editor->source_view ) );
	gtk_paned_pack1( GTK_PANED( editor->paned ), source_scrolled, TRUE, TRUE );

	result_scrolled = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( result_scrolled ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );

	editor->result_store = GTK_TREE_MODEL( pgm_data_store_new( PGM_DATABASE( current_database ), NULL ) );
	editor->result_view = GTK_WIDGET( pgm_data_view_new_with_store( PGM_DATA_STORE( editor->result_store ) ) );

	gtk_container_add( GTK_CONTAINER( result_scrolled ), editor->result_view );
	gtk_paned_pack2( GTK_PANED( editor->paned ), result_scrolled, TRUE, TRUE );

	action = gtk_action_group_get_action( editor->actions, "ActionUndo" );
	if( action )
		gtk_action_set_sensitive( action, FALSE );
	action = gtk_action_group_get_action( editor->actions, "ActionRedo" );
	if( action )
		gtk_action_set_sensitive( action, FALSE );

	g_signal_connect( editor->source_view, "can-undo-changed", G_CALLBACK( pgm_query_editor_can_undo_changed ), editor );
	g_signal_connect( editor->source_view, "can-redo-changed", G_CALLBACK( pgm_query_editor_can_redo_changed ), editor );

	return GTK_WIDGET( editor );
}


static void
on_new_file_activated( GtkAction *action, gpointer data )
{
	PgmQueryEditor *editor;

	g_return_if_fail( data != NULL );

	editor = PGM_QUERY_EDITOR( data );
	pgm_sql_editor_clear( PGM_SQL_EDITOR( editor->source_view ) );
}

static void
on_open_file_activated( GtkAction *action, gpointer data )
{
	PgmQueryEditor *editor;
	GtkWidget *dialog;
	gchar *filename;
	GtkFileFilter *filter;

	editor = PGM_QUERY_EDITOR( data );

	dialog = pgm_main_window_get_file_chooser_dialog( "Open File",
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name( filter, "Query (*.sql)" );
	gtk_file_filter_add_pattern( filter, "*.sql" );
	gtk_file_chooser_add_filter( GTK_FILE_CHOOSER( dialog ), filter );

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name( filter, "All Files" );
	gtk_file_filter_add_pattern( filter, "*" );
	gtk_file_chooser_add_filter( GTK_FILE_CHOOSER( dialog ), filter );

	if( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_ACCEPT )
	{
		filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER( dialog ) );
		gtk_widget_destroy( dialog );
		pgm_sql_editor_load_from_file( PGM_SQL_EDITOR( editor->source_view ), filename );
	}
	else
		gtk_widget_destroy( dialog );
}

static void
on_save_file_activated( GtkAction *action, gpointer data )
{
	PgmQueryEditor *editor;

	editor = PGM_QUERY_EDITOR( data );

	pgm_sql_editor_save( PGM_SQL_EDITOR( editor->source_view ) );
}

static void
on_undo_activated( GtkAction *action, gpointer data )
{
	PgmQueryEditor *editor = PGM_QUERY_EDITOR( data );
	pgm_sql_editor_undo( PGM_SQL_EDITOR( editor->source_view ) );
}

static void
on_redo_activated( GtkAction *action, gpointer data )
{
	PgmQueryEditor *editor = PGM_QUERY_EDITOR( data );
	pgm_sql_editor_redo( PGM_SQL_EDITOR( editor->source_view ) );
}

static void
on_execute_activated( GtkAction *action, gpointer data )
{
	PgmQueryEditor *editor = PGM_QUERY_EDITOR( data );
	gchar *text;
	GtkTreeIter db_iter;
	PgmObject *database;
	GtkWidget *db_combo;

	db_combo = pgm_tool_combo_box_get_combo_box( PGM_TOOL_COMBO_BOX( editor->db_combo_box ) );
	gtk_combo_box_get_active_iter( GTK_COMBO_BOX( db_combo ), &db_iter );

	database = pgm_database_list_model_get_database( PGM_DATABASE_LIST_MODEL( editor->db_list_model ), &db_iter );

	if( !database )
		return;

	pgm_data_store_set_database( PGM_DATA_STORE( editor->result_store ), PGM_DATABASE( database ) );

	if( pgm_sql_editor_has_selection( PGM_SQL_EDITOR( editor->source_view ) ) )
		text = pgm_sql_editor_get_selected_text( PGM_SQL_EDITOR( editor->source_view ) );
	else
		text = pgm_sql_editor_get_text( PGM_SQL_EDITOR( editor->source_view ) );

	pgm_data_store_set_query( PGM_DATA_STORE( editor->result_store ), text );
	pgm_data_store_update( PGM_DATA_STORE( editor->result_store ) );

	g_free( text );
	pgm_data_view_set_store( PGM_DATA_VIEW( editor->result_view ), PGM_DATA_STORE( NULL ) );
	pgm_data_view_set_store( PGM_DATA_VIEW( editor->result_view ), PGM_DATA_STORE( editor->result_store ) );
}

static void
pgm_query_editor_can_undo_changed( PgmSqlEditor *editor, gpointer data )
{
	PgmQueryEditor *query_editor = PGM_QUERY_EDITOR( data );
	GtkAction *action;

	action = gtk_action_group_get_action( query_editor->actions, "ActionUndo" );
	if( action )
		gtk_action_set_sensitive( action, pgm_sql_editor_can_undo( editor ) );
}

static void
pgm_query_editor_can_redo_changed( PgmSqlEditor *editor, gpointer data )
{
	PgmQueryEditor *query_editor = PGM_QUERY_EDITOR( data );
	GtkAction *action;

	action = gtk_action_group_get_action( query_editor->actions, "ActionRedo" );
	if( action )
		gtk_action_set_sensitive( action, pgm_sql_editor_can_redo( editor ) );
}

static void
on_cut_activated( GtkAction *action, gpointer data )
{
	PgmQueryEditor *editor = PGM_QUERY_EDITOR( data );
	pgm_sql_editor_cut( PGM_SQL_EDITOR( editor->source_view ) );
}

static void
on_copy_activated( GtkAction *action, gpointer data )
{
	PgmQueryEditor *editor = PGM_QUERY_EDITOR( data );
	pgm_sql_editor_copy( PGM_SQL_EDITOR( editor->source_view ) );
}

static void
on_paste_activated( GtkAction *action, gpointer data )
{
	PgmQueryEditor *editor = PGM_QUERY_EDITOR( data );
	pgm_sql_editor_paste( PGM_SQL_EDITOR( editor->source_view ) );
}
