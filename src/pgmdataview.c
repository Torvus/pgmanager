#include "pgmdataview.h"

#include "pgmcellrenderertext.h"
#include "pgmcellrenderertoggle.h"
#include "pgmcellrendererspin.h"
#include "pgmmarshal.h"

G_DEFINE_TYPE( PgmDataView, pgm_data_view, GTK_TYPE_TREE_VIEW )

typedef struct _EditedStruct EditedStruct;

struct _EditedStruct
{
	PgmDataView *view;
	gint column;
};

static void pgm_data_view_init( PgmDataView *view );
static void pgm_data_view_class_init( PgmDataViewClass *klass );
static void pgm_data_view_finalize( GObject *object );
/*static void pgm_data_view_renderer_toggled( GtkCellRenderer *renderer, const gchar *path, gpointer data );*/
static void pgm_data_view_renderer_edited( GtkCellRenderer *renderer, const gchar *path, const gchar *new_text, gpointer data );
static void pgm_data_view_set_property( GObject                  *object,
										guint                     param_id,
										const GValue             *value,
										GParamSpec               *pspec );
static void pgm_data_view_get_property( GObject                  *object,
										guint                     param_id,
										GValue                   *value,
										GParamSpec               *pspec );

enum
{
	EDITED,
	LAST_SIGNAL
};

enum
{
	PROP_0,
	PROP_EDITABLE
};

static guint signals[ LAST_SIGNAL ] = {0};

static void
pgm_data_view_init( PgmDataView *view )
{
	view->editable = FALSE;
	view->list = NULL;
}

static void
pgm_data_view_class_init( PgmDataViewClass *klass )
{
	GObjectClass *gclass = G_OBJECT_CLASS( klass );

	gclass->set_property = pgm_data_view_set_property;
	gclass->get_property = pgm_data_view_get_property;
	gclass->finalize = pgm_data_view_finalize;

	signals[ EDITED ] =
			g_signal_new( "edited",
						  G_OBJECT_CLASS_TYPE( gclass ),
						  G_SIGNAL_RUN_LAST,
						  G_STRUCT_OFFSET( PgmDataViewClass, edited ),
						  pgm_boolean_handled_accumulator, NULL,
						  pgm_marshal_BOOL_INT_INT_POINTER,
						  G_TYPE_BOOLEAN, 3,
						  G_TYPE_INT,
						  G_TYPE_INT,
						  G_TYPE_POINTER);

	g_object_class_install_property( gclass,
                                     PROP_EDITABLE,
                                     g_param_spec_boolean( "editable",
                                                           "Editable",
                                                           "Whether the text can be modified by the user",
                                                           FALSE,
                                                           G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS ) );
}

static void
pgm_data_view_finalize( GObject *object )
{
	PgmDataView *view = PGM_DATA_VIEW( object );
	GList *cur;

	cur = g_list_first( view->list );
	while( cur )
	{
		g_free( cur->data );
		cur = g_list_next( cur );
	}
	g_list_free( view->list );

	G_OBJECT_CLASS( pgm_data_view_parent_class )->finalize( object );
}

void
pgm_data_view_set_model( PgmDataView *view, PgmDataStore *store )
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	gint col;
	gchar *column_name;
	GList *columns;
	/*GValue editable = {0};

	g_value_init( &editable, G_TYPE_BOOLEAN );
	g_value_set_boolean( &editable, TRUE );*/

	columns = gtk_tree_view_get_columns( GTK_TREE_VIEW( view ) );
	while( columns )
	{
		gtk_tree_view_remove_column( GTK_TREE_VIEW( view ), GTK_TREE_VIEW_COLUMN( columns->data ) );
		columns = g_list_next( columns );
	}

	gtk_tree_view_set_model( GTK_TREE_VIEW( view ), GTK_TREE_MODEL( store ) );

	if( store == NULL )
		return;

	for( col = 0; col < gtk_tree_model_get_n_columns( GTK_TREE_MODEL( store ) ); col++ )
	{
		column_name = pgm_data_store_get_column_title( store, col );
		switch( gtk_tree_model_get_column_type( GTK_TREE_MODEL( store ), col ) )
		{
			case G_TYPE_BOOLEAN:
								renderer = pgm_cell_renderer_toggle_new();
								/*g_signal_connect( G_OBJECT( renderer ), "toggled", G_CALLBACK( pgm_data_view_renderer_toggled ), store );*/
								break;
			case G_TYPE_INT:
			case G_TYPE_LONG:
			case G_TYPE_ULONG:
			case G_TYPE_DOUBLE:
			case G_TYPE_FLOAT:
								renderer = pgm_cell_renderer_spin_new();
								break;
			default:
								renderer = pgm_cell_renderer_text_new();
								/*g_signal_connect( G_OBJECT( renderer ), "edited", G_CALLBACK( pgm_data_view_renderer_edited ), store );*/
								break;
		}
		/*g_object_set_property( G_OBJECT( renderer ), "editable", &editable );*/
		column = gtk_tree_view_column_new_with_attributes( column_name, renderer, "text", col, NULL );
		gtk_tree_view_column_set_resizable( column, TRUE );
		gtk_tree_view_column_set_min_width( column, 1 );
		gtk_tree_view_column_set_sort_column_id( column, col );
		gtk_tree_view_append_column( GTK_TREE_VIEW( view ), column );
	}
}

GtkWidget*
pgm_data_view_new()
{
	GtkWidget *view = g_object_new( PGM_TYPE_DATA_VIEW, NULL );
	/*gtk_tree_view_set_grid_lines( GTK_TREE_VIEW( view ), GTK_TREE_VIEW_GRID_LINES_BOTH );*/
	return view;
}

GtkWidget*
pgm_data_view_new_with_store( PgmDataStore *store )
{
	GtkWidget *view = g_object_new( PGM_TYPE_DATA_VIEW, NULL );
	pgm_data_view_set_model( PGM_DATA_VIEW( view ), store );
	/*gtk_tree_view_set_grid_lines( GTK_TREE_VIEW( view ), GTK_TREE_VIEW_GRID_LINES_BOTH );*/
	return view;
}

void
pgm_data_view_set_store( PgmDataView *view, PgmDataStore *store )
{
	pgm_data_view_set_model( view, store );
}

static void
pgm_data_view_set_property( GObject                  *object,
							guint                     param_id,
							const GValue             *value,
							GParamSpec               *pspec )
{
	PgmDataView *view = PGM_DATA_VIEW( object );

	switch( param_id )
	{
		case PROP_EDITABLE:
			pgm_data_view_set_editable( view, g_value_get_boolean( value ) );
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
			break;
	}
}

static void
pgm_data_view_get_property( GObject                  *object,
							guint                     param_id,
							GValue                   *value,
							GParamSpec               *pspec )
{
	PgmDataView *view = PGM_DATA_VIEW( object );

	switch( param_id )
	{
		case PROP_EDITABLE:
			g_value_set_boolean( value, view->editable );
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
			break;
	}
}

void
pgm_data_view_set_editable( PgmDataView *view, gboolean editable )
{
	if( view->editable == editable )
		return;
	view->editable = editable;
	if( editable )
	{
		GList *columns;
		GList *cur_column;
		guint column_number = 0;

		columns = gtk_tree_view_get_columns( GTK_TREE_VIEW( view ) );
		cur_column = g_list_first( columns );
		while( cur_column )
		{
			GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN( cur_column->data );
			GtkCellRenderer *renderer = gtk_cell_layout_get_cells( GTK_CELL_LAYOUT( column ) )->data;
			GValue val = {0};
			EditedStruct *e_struct;

			g_value_init( &val, G_TYPE_BOOLEAN );
			g_value_set_boolean( &val, editable );
			g_object_set_property( G_OBJECT( renderer ), "editable", &val );

			e_struct = g_new0( EditedStruct, 1 );
			e_struct->view = view;
			e_struct->column = column_number;

			/*switch( gtk_tree_model_get_column_type( gtk_tree_view_get_model( GTK_TREE_VIEW( view ) ), column_number ) )
			{
				case G_TYPE_BOOLEAN:
					g_signal_connect( G_OBJECT( renderer ), "toggled", G_CALLBACK( pgm_data_view_renderer_toggled ), e_struct );
					break;

				default:
					g_signal_connect( G_OBJECT( renderer ), "edited", G_CALLBACK( pgm_data_view_renderer_edited ), e_struct );
					break;
			}*/
			g_signal_connect( G_OBJECT( renderer ), "edited", G_CALLBACK( pgm_data_view_renderer_edited ), e_struct );
			column_number++;
			cur_column = g_list_next( cur_column );
			view->list = g_list_append( view->list, e_struct );
		}
	}
}

gboolean
pgm_data_view_get_editable( PgmDataView *view )
{
	return view->editable;
}

gboolean
pgm_data_view_edited( PgmDataView *view, gint row, gint column, gchar *new_text )
{
	/*PgmDataStore *store;*/
	gboolean edited = FALSE;

	g_signal_emit( G_OBJECT( view ), signals[ EDITED ], 0, row, column, new_text, &edited );

	/*g_debug( "edited = %s", (edited ? "true" : "false") );
	if( edited )
	{
		store = PGM_DATA_STORE( gtk_tree_view_get_model( GTK_TREE_VIEW( view ) ) );
		pgm_data_store_row_changed( store, row );
	}*/
	return edited;
}
/*
static void
pgm_data_view_renderer_toggled( GtkCellRenderer *renderer, const gchar *path_str, gpointer data )
{
	GtkTreePath *path;
	gint *indices;
	gint depth;
	GValue val = {0};
	GtkTreeModel *model;
	GtkTreeIter iter;
	EditedStruct *e_struct = (EditedStruct*)data;
	gchar *text;
	gchar *val_text;
	gboolean value;

	path = gtk_tree_path_new_from_string( path_str );
	if( !path )
		return;

	depth = gtk_tree_path_get_depth( path );
	indices = gtk_tree_path_get_indices( path );

	if( depth == 0 )
		return;

	model = gtk_tree_view_get_model( GTK_TREE_VIEW( e_struct->view ) );
	gtk_tree_model_get_iter( model, &iter, path );
	gtk_tree_model_get_value( model, &iter, e_struct->column, &val );
	text = g_strdup_value_contents( &val );
	if( g_strcmp0( text, "\"#<!-NULL->#\"" ) == 0 )
	{
		value = TRUE;
		val_text = "true";
	}
	else
	{
		value = !g_value_get_boolean( &val );
		if( value == TRUE )
			val_text = "";
		else
			val_text = "false";
	}
	pgm_data_view_edited( e_struct->view, indices[ depth - 1 ], e_struct->column, val_text );
	g_free( text );
}
*/
static void
pgm_data_view_renderer_edited( GtkCellRenderer *renderer, const gchar *path_str, const gchar *new_text, gpointer data )
{
	GtkTreePath *path;
	gint *indices;
	gint depth;
	EditedStruct *e_struct = (EditedStruct*)data;

	path = gtk_tree_path_new_from_string( path_str );
	if( !path )
		return;

	depth = gtk_tree_path_get_depth( path );
	indices = gtk_tree_path_get_indices( path );

	if( depth == 0 )
		return;

	pgm_data_view_edited( e_struct->view, indices[ depth - 1 ], e_struct->column, (gchar*)new_text );
}
