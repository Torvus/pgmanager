#include "pgmdatabaselistmodel.h"

#include "pgmserver.h"
#include "pgmdatabase.h"

#define TRACE_FUNC g_debug( "%s()", __func__ )

static void pgm_database_list_tree_model_init( GtkTreeModelIface *iface );
static void pgm_database_list_model_init( PgmDatabaseListModel *model );
static void pgm_database_list_model_class_init( PgmDatabaseListModelClass *klass );

static GtkTreeModelFlags pgm_database_list_model_get_flags       ( GtkTreeModel      *tree_model );
static gint              pgm_database_list_model_get_n_columns   ( GtkTreeModel      *tree_model );
static GType             pgm_database_list_model_get_column_type ( GtkTreeModel      *tree_model,
																   gint               index );
static gboolean          pgm_database_list_model_get_iter        ( GtkTreeModel      *tree_model,
																   GtkTreeIter       *iter,
																   GtkTreePath       *path );
static GtkTreePath *     pgm_database_list_model_get_path        ( GtkTreeModel      *tree_model,
																   GtkTreeIter       *iter );
static void              pgm_database_list_model_get_value       ( GtkTreeModel      *tree_model,
																   GtkTreeIter       *iter,
																   gint               column,
																   GValue            *value );
static gboolean          pgm_database_list_model_iter_next       ( GtkTreeModel      *tree_model,
																   GtkTreeIter       *iter );
static gboolean          pgm_database_list_model_iter_children   ( GtkTreeModel      *tree_model,
																   GtkTreeIter       *iter,
																   GtkTreeIter       *parent );
static gboolean          pgm_database_list_model_iter_has_child  ( GtkTreeModel      *tree_model,
																   GtkTreeIter       *iter );
static gint              pgm_database_list_model_iter_n_children ( GtkTreeModel      *tree_model,
																   GtkTreeIter       *iter );
static gboolean          pgm_database_list_model_iter_nth_child  ( GtkTreeModel      *tree_model,
																   GtkTreeIter       *iter,
																   GtkTreeIter       *parent,
																   gint               n );
static gboolean          pgm_database_list_model_iter_parent     ( GtkTreeModel      *tree_model,
																   GtkTreeIter       *iter,
																   GtkTreeIter       *child );

G_DEFINE_TYPE_WITH_CODE( PgmDatabaseListModel, pgm_database_list_model, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE( GTK_TYPE_TREE_MODEL,
							   pgm_database_list_tree_model_init ) )

static void
pgm_database_list_tree_model_init( GtkTreeModelIface *iface )
{
  iface->get_flags = pgm_database_list_model_get_flags;
  iface->get_n_columns = pgm_database_list_model_get_n_columns;
  iface->get_column_type = pgm_database_list_model_get_column_type;
  iface->get_iter = pgm_database_list_model_get_iter;
  iface->get_path = pgm_database_list_model_get_path;
  iface->get_value = pgm_database_list_model_get_value;
  iface->iter_next = pgm_database_list_model_iter_next;
  iface->iter_children = pgm_database_list_model_iter_children;
  iface->iter_has_child = pgm_database_list_model_iter_has_child;
  iface->iter_n_children = pgm_database_list_model_iter_n_children;
  iface->iter_nth_child = pgm_database_list_model_iter_nth_child;
  iface->iter_parent = pgm_database_list_model_iter_parent;
}

static void
pgm_database_list_model_init( PgmDatabaseListModel *model )
{
	model->list = NULL;
	model->stamp = g_random_int();
}

static void
pgm_database_list_model_class_init( PgmDatabaseListModelClass *klass )
{}

PgmDatabaseListModel*
pgm_database_list_model_new( PgmServerList *list )
{
	PgmDatabaseListModel *model;
	PgmObject *server;
	PgmObject *database;
	gint servers;
	gint databases;
	gint i, u;

	model = g_object_new( PGM_TYPE_DATABASE_LIST_MODEL, NULL );

	servers = pgm_object_get_n_childs( PGM_OBJECT( list ) );

	for( i = 0; i < servers; i++ )
	{
		server = pgm_object_get_nth_child( PGM_OBJECT( list ), i );
		databases = pgm_object_get_n_childs( server );
		for( u = 0; u < databases; u++ )
		{
			database = pgm_object_get_nth_child( server, u );
			if( pgm_database_is_connected( PGM_DATABASE( database ) ) )
				model->list = g_list_append( model->list, database );
		}
	}

	return model;
}

PgmObject*
pgm_database_list_model_get_database( PgmDatabaseListModel *model, GtkTreeIter *iter )
{
	g_return_val_if_fail( model != NULL, NULL );
	g_return_val_if_fail( iter != NULL, NULL);
	g_return_val_if_fail( iter->stamp == model->stamp, NULL );
	return PGM_OBJECT( iter->user_data );
}

gboolean
pgm_database_list_model_get_database_iter( PgmDatabaseListModel *model, PgmObject *database, GtkTreeIter *iter )
{
	g_return_val_if_fail( model != NULL, FALSE );
	g_return_val_if_fail( iter != NULL, FALSE);

	iter->stamp = 0;

	if( g_list_find( model->list, database ) == NULL )
		return FALSE;

	iter->stamp = model->stamp;
	iter->user_data = database;
	return TRUE;
}

static GtkTreeModelFlags
pgm_database_list_model_get_flags( GtkTreeModel      *tree_model )
{
	return GTK_TREE_MODEL_ITERS_PERSIST | GTK_TREE_MODEL_LIST_ONLY;
}

static gint
pgm_database_list_model_get_n_columns( GtkTreeModel      *tree_model )
{
	return 1;
}

static GType
pgm_database_list_model_get_column_type( GtkTreeModel      *tree_model,
									     gint               index )
{
	return G_TYPE_STRING;
}

static gboolean
pgm_database_list_model_get_iter( GtkTreeModel      *tree_model,
							      GtkTreeIter       *iter,
								  GtkTreePath       *path )
{
	PgmDatabaseListModel *model;
	gint row;

	model = PGM_DATABASE_LIST_MODEL( tree_model );

	row = gtk_tree_path_get_indices( path )[ 0 ];
	if( row >= g_list_length( model->list ) )
	{
		iter->stamp = 0;
		return FALSE;
	}

	iter->stamp = model->stamp;
	iter->user_data = g_list_nth_data( model->list, row );
	return TRUE;
}

static GtkTreePath *
pgm_database_list_model_get_path( GtkTreeModel      *tree_model,
								  GtkTreeIter       *iter )
{
	PgmDatabaseListModel *model;
	GtkTreePath *path;

	model = PGM_DATABASE_LIST_MODEL( tree_model );

	g_return_val_if_fail( iter->stamp == model->stamp, NULL);

	if( iter->user_data == NULL )
		return NULL;

	path = gtk_tree_path_new();
	gtk_tree_path_append_index( path, g_list_index( model->list, iter->user_data ) );

	return path;
}

static void
pgm_database_list_model_get_value( GtkTreeModel      *tree_model,
								   GtkTreeIter       *iter,
								   gint               column,
								   GValue            *value )
{
	PgmDatabaseListModel *model;
	PgmObject *database;
	PgmObject *server;
	gchar *db_name;
	gchar *host;
	gchar *login;
	GString *str;

	model = PGM_DATABASE_LIST_MODEL( tree_model );

	g_return_if_fail( iter != NULL );
	g_return_if_fail( iter->stamp == model->stamp );

	database = PGM_OBJECT( iter->user_data );
	server = pgm_object_get_parent( database );

	db_name = pgm_object_get_name( database );
	host = pgm_server_get_host( PGM_SERVER( server ) );
	login = pgm_server_get_login( PGM_SERVER( server ) );

	str = g_string_new( NULL );
	g_string_printf( str, "%s on %s@%s", db_name, host, login );

	g_value_init( value, G_TYPE_STRING );
	g_value_set_string( value, str->str );

	g_string_free( str, TRUE );
}

static gboolean
pgm_database_list_model_iter_next( GtkTreeModel      *tree_model,
								   GtkTreeIter       *iter )
{
	PgmDatabaseListModel *model = PGM_DATABASE_LIST_MODEL( tree_model );
	GList *val;

	g_return_val_if_fail( iter->stamp == model->stamp, FALSE );

	val = g_list_find( model->list, iter->user_data );
	if( val == NULL )
	{
		iter->stamp = 0;
		return FALSE;
	}

	val = g_list_next( val );
	if( val == NULL )
	{
		iter->stamp = 0;
		return FALSE;
	}
	iter->user_data = val->data;
	return TRUE;
}

static gboolean
pgm_database_list_model_iter_children( GtkTreeModel      *tree_model,
									   GtkTreeIter       *iter,
									   GtkTreeIter       *parent )
{
	PgmDatabaseListModel *model = PGM_DATABASE_LIST_MODEL( tree_model );

	g_return_val_if_fail( iter != NULL, FALSE );
	g_return_val_if_fail( iter->stamp == model->stamp, FALSE );

	if( parent != NULL )
	{
		iter->stamp = 0;
		return FALSE;
	}

	iter->user_data = g_list_first( model->list )->data;
	return TRUE;
}

static gboolean
pgm_database_list_model_iter_has_child( GtkTreeModel      *tree_model,
										GtkTreeIter       *iter )
{
	return FALSE;
}

static gint
pgm_database_list_model_iter_n_children( GtkTreeModel      *tree_model,
										 GtkTreeIter       *iter )
{
	PgmDatabaseListModel *model = PGM_DATABASE_LIST_MODEL( tree_model );

	if( iter == NULL )
		return g_list_length( model->list );

	g_return_val_if_fail( iter->stamp == model->stamp, -1 );

	return 0;
}

static gboolean
pgm_database_list_model_iter_nth_child( GtkTreeModel      *tree_model,
										GtkTreeIter       *iter,
										GtkTreeIter       *parent,
										gint               n )
{
	PgmDatabaseListModel *model = PGM_DATABASE_LIST_MODEL( tree_model );

	iter->stamp = 0;

	if( parent )
		return FALSE;

	if( n > g_list_length( model->list ) )
		return FALSE;

	iter->stamp = model->stamp;
	iter->user_data = g_list_nth_data( model->list, n );
	return TRUE;
}

static gboolean
pgm_database_list_model_iter_parent( GtkTreeModel      *tree_model,
									 GtkTreeIter       *iter,
									 GtkTreeIter       *child )
{
	iter->stamp = 0;
	return FALSE;
}
