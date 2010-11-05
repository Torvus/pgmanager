#include "pgmdatastore.h"

#define TRACE g_debug( "%s():%d", __func__, __LINE__ )

static void              pgm_data_store_init            ( PgmDataStore *store );
static void              pgm_data_store_class_init      ( PgmDataStoreClass *klass );
static void              pgm_data_store_dispose         ( GObject *gobject );
static void              pgm_data_store_finalize        ( GObject *gobject );
static void              pgm_data_store_tree_model_init ( GtkTreeModelIface *iface );
static void              pgm_data_store_sortable_init   (GtkTreeSortableIface   *iface);
static GtkTreeModelFlags pgm_data_store_get_flags       ( GtkTreeModel      *tree_model );
static gint              pgm_data_store_get_n_columns   ( GtkTreeModel      *tree_model );
static GType             pgm_data_store_get_column_type ( GtkTreeModel      *tree_model,
						                                  gint               index );
static gboolean          pgm_data_store_get_iter        ( GtkTreeModel      *tree_model,
						                                  GtkTreeIter       *iter,
						                                  GtkTreePath       *path );
static GtkTreePath *     pgm_data_store_get_path        ( GtkTreeModel      *tree_model,
						                                  GtkTreeIter       *iter );
static void              pgm_data_store_get_value       ( GtkTreeModel      *tree_model,
						                                  GtkTreeIter       *iter,
						                                  gint               column,
						                                  GValue            *value );
static gboolean          pgm_data_store_iter_next       ( GtkTreeModel      *tree_model,
						                                  GtkTreeIter       *iter );
static gboolean          pgm_data_store_iter_children   ( GtkTreeModel      *tree_model,
						                                  GtkTreeIter       *iter,
						                                  GtkTreeIter       *parent );
static gboolean          pgm_data_store_iter_has_child  ( GtkTreeModel      *tree_model,
						                                  GtkTreeIter       *iter );
static gint              pgm_data_store_iter_n_children ( GtkTreeModel      *tree_model,
						                                  GtkTreeIter       *iter );
static gboolean          pgm_data_store_iter_nth_child  ( GtkTreeModel      *tree_model,
						                                  GtkTreeIter       *iter,
						                                  GtkTreeIter       *parent,
						                                  gint               n );
static gboolean          pgm_data_store_iter_parent     ( GtkTreeModel      *tree_model,
						                                  GtkTreeIter       *iter,
						                                  GtkTreeIter       *child );

/* sortable */
static gboolean pgm_data_store_get_sort_column_id    ( GtkTreeSortable        *sortable,
													   gint                   *sort_column_id,
													   GtkSortType            *order );
static void     pgm_data_store_set_sort_column_id    ( GtkTreeSortable        *sortable,
													   gint                    sort_column_id,
													   GtkSortType             order );
static void     pgm_data_store_set_sort_func         ( GtkTreeSortable        *sortable,
													   gint                    sort_column_id,
													   GtkTreeIterCompareFunc  func,
													   gpointer                data,
													   GDestroyNotify          destroy );
static void     pgm_data_store_set_default_sort_func ( GtkTreeSortable        *sortable,
													   GtkTreeIterCompareFunc  func,
													   gpointer                data,
													   GDestroyNotify          destroy );
static gboolean pgm_data_store_has_default_sort_func ( GtkTreeSortable        *sortable );


static gchar* pgm_data_store_prepare_sorted_query( PgmDataStore *store );

G_DEFINE_TYPE_WITH_CODE( PgmDataStore, pgm_data_store, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE( GTK_TYPE_TREE_MODEL,
						pgm_data_store_tree_model_init )
			 G_IMPLEMENT_INTERFACE( GTK_TYPE_TREE_SORTABLE,
						pgm_data_store_sortable_init)
						)

static void
pgm_data_store_init( PgmDataStore *store )
{
	store->stamp = g_random_int();
	store->n_rows = 0;
}

static void
pgm_data_store_class_init( PgmDataStoreClass *klass )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( klass );

	gobject_class->dispose = pgm_data_store_dispose;
	gobject_class->finalize = pgm_data_store_finalize;
}

static void
pgm_data_store_dispose( GObject *gobject )
{
	PgmDataStore *store = PGM_DATA_STORE( gobject );

	if( store->query )
		g_object_unref( G_OBJECT( store->query ) );
	if( store->query_str )
		g_free( store->query_str );

	G_OBJECT_CLASS( pgm_data_store_parent_class )->dispose( gobject );
}

static void
pgm_data_store_finalize( GObject *gobject )
{
	G_OBJECT_CLASS( pgm_data_store_parent_class )->finalize( gobject );
}

static void
pgm_data_store_tree_model_init( GtkTreeModelIface *iface)
{
	iface->get_flags = pgm_data_store_get_flags;
	iface->get_n_columns = pgm_data_store_get_n_columns;
	iface->get_column_type = pgm_data_store_get_column_type;
	iface->get_iter = pgm_data_store_get_iter;
	iface->get_path = pgm_data_store_get_path;
	iface->get_value = pgm_data_store_get_value;
	iface->iter_next = pgm_data_store_iter_next;
	iface->iter_children = pgm_data_store_iter_children;
	iface->iter_has_child = pgm_data_store_iter_has_child;
	iface->iter_n_children = pgm_data_store_iter_n_children;
	iface->iter_nth_child = pgm_data_store_iter_nth_child;
	iface->iter_parent = pgm_data_store_iter_parent;
}

static void
pgm_data_store_sortable_init(GtkTreeSortableIface   *iface)
{
	iface->get_sort_column_id = pgm_data_store_get_sort_column_id;
	iface->set_sort_column_id = pgm_data_store_set_sort_column_id;
	iface->set_sort_func = pgm_data_store_set_sort_func;
	iface->set_default_sort_func = pgm_data_store_set_default_sort_func;
	iface->has_default_sort_func = pgm_data_store_has_default_sort_func;
}
/*
PgmDataStore*
pgm_data_store_new( PgmQuery *query )
{
	PgmDataStore *store;
	GString *sorted_query_str;

	store = g_object_new( PGM_TYPE_DATA_STORE, NULL );

	sorted_query_str = g_string_new( pgm_query_get_last_query( query ) );
	if( sorted_query_str->len > 0 )
	{
		gint insert_pos = sorted_query_str->len;
		if( sorted_query_str->str[ insert_pos-1 ] == ';' )
			insert_pos--;
		g_string_insert( sorted_query_str, insert_pos, " order by 1 asc" );
	}
	store->query = pgm_query_new_with_query( query->database, sorted_query_str->str );
	store->unsorted_query = query;
	if( query )
		g_object_ref( G_OBJECT( query ) );

	store->n_rows = pgm_query_get_rows( store->query );
	store->sort_column_id = 0;
	store->order = GTK_SORT_ASCENDING;

	g_signal_connect( query, "executed", G_CALLBACK( pgm_data_store_on_query_executed ), store );

	g_string_free( sorted_query_str, TRUE );
	return store;
}
*/
PgmDataStore*
pgm_data_store_new( PgmDatabase *db, const gchar *query_str )
{
	PgmDataStore *store;
	gchar *query = NULL;

	store = g_object_new( PGM_TYPE_DATA_STORE, NULL );

	store->query = pgm_query_new( db );
	store->query_str = g_strdup( query_str );

	query = pgm_data_store_prepare_sorted_query( store );

	pgm_query_exec( store->query, query );

	if( query )
		g_free( query );

	store->n_rows = pgm_query_get_rows( store->query );
	store->sort_column_id = 0;
	store->order = GTK_SORT_ASCENDING;

	return store;
}

void
pgm_data_store_set_database( PgmDataStore *store, PgmDatabase *db )
{
	pgm_query_set_database( store->query, db );
}

void
pgm_data_store_set_query( PgmDataStore *store, const gchar *query_str )
{
	if( store->query_str )
		g_free( store->query_str );
	store->query_str = g_strdup( query_str );
}

gchar*
pgm_data_store_get_column_title( PgmDataStore *store, gint column )
{
	g_return_val_if_fail( store != NULL, NULL );
	g_return_val_if_fail( column >= 0, NULL );
	g_return_val_if_fail( column < pgm_query_get_columns( store->query ), NULL );

	return pgm_query_get_column_name( store->query, column );
}

static GtkTreeModelFlags
pgm_data_store_get_flags( GtkTreeModel      *tree_model )
{
	return GTK_TREE_MODEL_ITERS_PERSIST | GTK_TREE_MODEL_LIST_ONLY;
}

static gint
pgm_data_store_get_n_columns   ( GtkTreeModel      *tree_model )
{
	PgmDataStore *store = PGM_DATA_STORE( tree_model );

	return pgm_query_get_columns( store->query );
}

static GType
pgm_data_store_get_column_type ( GtkTreeModel      *tree_model,
                                 gint               index )
{
	PgmDataStore *store = PGM_DATA_STORE( tree_model );
	return pgm_query_get_column_gtype( store->query, index );
}

static gboolean
pgm_data_store_get_iter( GtkTreeModel      *tree_model,
                         GtkTreeIter       *iter,
                         GtkTreePath       *path )
{
	PgmDataStore *store = PGM_DATA_STORE( tree_model );
	gint row;

	row = gtk_tree_path_get_indices( path )[ 0 ];

	if( row >= pgm_query_get_rows( store->query ) )
	{
		iter->stamp = 0;
		return FALSE;
	}

	iter->stamp = store->stamp;
	iter->user_data = GINT_TO_POINTER( row );
	return TRUE;
}

static GtkTreePath *
pgm_data_store_get_path( GtkTreeModel      *tree_model,
                         GtkTreeIter       *iter )
{
	PgmDataStore *store;
	GtkTreePath *path;
	gint row;

	g_return_val_if_fail( iter->stamp == PGM_DATA_STORE( tree_model )->stamp, NULL);

	row = GPOINTER_TO_INT( iter->user_data );
	store = PGM_DATA_STORE( tree_model );

	if( row < 0 || row >= pgm_query_get_rows( store->query ) )
		return NULL;

	path = gtk_tree_path_new();
	gtk_tree_path_append_index( path, row );

	return path;
}

static void
pgm_data_store_get_value( GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter,
                          gint               column,
                          GValue            *value )
{
	PgmDataStore *store;
	gint row;

	store = PGM_DATA_STORE( tree_model );

	g_return_if_fail( column >= 0 );
	g_return_if_fail( column < pgm_query_get_columns( store->query ) );
	g_return_if_fail( iter != NULL );
	g_return_if_fail( iter->stamp == store->stamp );

	row = GPOINTER_TO_INT( iter->user_data );

	g_return_if_fail( row >= 0 );
	g_return_if_fail( row < pgm_query_get_rows( store->query ) );

	if( pgm_query_get_is_null( store->query, row, column ) )
	{
		g_value_init( value, G_TYPE_STRING );
		g_value_set_static_string( value, "#<!-NULL->#" );
	}
	else
		pgm_query_get_gvalue( store->query, row, column, value );
}

static gboolean
pgm_data_store_iter_next( GtkTreeModel      *tree_model,
                          GtkTreeIter       *iter )
{
	PgmDataStore *store = PGM_DATA_STORE( tree_model );
	gint row;

	g_return_val_if_fail( iter->stamp == store->stamp, FALSE );

	row = GPOINTER_TO_INT( iter->user_data );

	g_return_val_if_fail( row >= 0, FALSE );
	g_return_val_if_fail( row < pgm_query_get_rows( store->query ), FALSE );

	iter->user_data = GINT_TO_POINTER( ++row );

	if( row < pgm_query_get_rows( store->query ) )
		return TRUE;

	iter->stamp = 0;
	return FALSE;
}

static gboolean
pgm_data_store_iter_children( GtkTreeModel      *tree_model,
                              GtkTreeIter       *iter,
                              GtkTreeIter       *parent )
{
	PgmDataStore *store = PGM_DATA_STORE( tree_model );

	g_return_val_if_fail( iter != NULL, FALSE );

	if( iter->stamp != store->stamp )
		return FALSE;

	if( parent != NULL )
	{
		iter->stamp = 0;
		return FALSE;
	}

	if( pgm_query_get_rows( store->query ) > 0 )
	{
		iter->stamp = store->stamp;
		iter->user_data = GINT_TO_POINTER( 0 );
		return TRUE;
	}
	iter->stamp = 0;
	return FALSE;
}

static gboolean
pgm_data_store_iter_has_child( GtkTreeModel      *tree_model,
                               GtkTreeIter       *iter )
{
	return FALSE;
}

static gint
pgm_data_store_iter_n_children ( GtkTreeModel      *tree_model,
                                 GtkTreeIter       *iter )
{
	PgmDataStore *store = PGM_DATA_STORE( tree_model );
	if( iter == NULL )
		return ( store->n_rows = pgm_query_get_rows( store->query ) );

	g_return_val_if_fail( iter->stamp == store->stamp, -1 );

	return 0;
}

static gboolean
pgm_data_store_iter_nth_child  ( GtkTreeModel      *tree_model,
                                 GtkTreeIter       *iter,
                                 GtkTreeIter       *parent,
                                 gint               n )
{
	PgmDataStore *store = PGM_DATA_STORE( tree_model );

	iter->stamp = 0;

	if( parent )
		return FALSE;

	if( n < 0 || n >= pgm_query_get_rows( store->query ) )
		return FALSE;

	iter->stamp = store->stamp;
	iter->user_data = GINT_TO_POINTER( n );
	return TRUE;
}

static gboolean
pgm_data_store_iter_parent( GtkTreeModel      *tree_model,
                            GtkTreeIter       *iter,
                            GtkTreeIter       *child )
{
	iter->stamp = 0;
	return FALSE;
}

static gboolean
pgm_data_store_get_sort_column_id( GtkTreeSortable        *sortable,
								   gint                   *sort_column_id,
								   GtkSortType            *order )
{
	PgmDataStore *store;

	g_return_val_if_fail( sortable != NULL, FALSE );
	g_return_val_if_fail( PGM_IS_DATA_STORE( sortable ), FALSE );

	store = PGM_DATA_STORE( sortable );

	if( sort_column_id )
		*sort_column_id = store->sort_column_id;
	if( order )
		*order = store->order;

	return TRUE;
}

static void
pgm_data_store_set_sort_column_id( GtkTreeSortable        *sortable,
								   gint                    sort_column_id,
								   GtkSortType             order )
{
	PgmDataStore *store = PGM_DATA_STORE( sortable );

	if( sort_column_id != GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID &&
	    sort_column_id != GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID )
		store->sort_column_id = sort_column_id;

	store->order = order;
	pgm_data_store_update( store );
	gtk_tree_sortable_sort_column_changed( sortable );
}

static void
pgm_data_store_set_sort_func( GtkTreeSortable        *sortable,
							  gint                    sort_column_id,
							  GtkTreeIterCompareFunc  func,
							  gpointer                data,
							  GDestroyNotify          destroy )
{}

static void
pgm_data_store_set_default_sort_func( GtkTreeSortable        *sortable,
									  GtkTreeIterCompareFunc  func,
									  gpointer                data,
									  GDestroyNotify          destroy )
{}

static gboolean
pgm_data_store_has_default_sort_func( GtkTreeSortable        *sortable )
{
	return TRUE;
}

void
pgm_data_store_update( PgmDataStore *store )
{
	gint n_rows;
	gint rows_difference;
	gint i;
	GtkTreePath *path = NULL;
	GtkTreeIter iter;
	gboolean append_row;
	gchar *query;

	query = pgm_data_store_prepare_sorted_query( store );

	pgm_query_exec( store->query, query );

	g_free( query );

	n_rows = pgm_query_get_rows( store->query );
	append_row = store->n_rows < n_rows;
	iter.stamp = store->stamp;
	rows_difference = append_row ? n_rows - store->n_rows : store->n_rows - n_rows;

	for( i = 0; i < rows_difference; i++ )
	{
		path = gtk_tree_path_new();
		if( append_row )
		{
			gtk_tree_path_append_index( path, store->n_rows+i );
			iter.user_data = GINT_TO_POINTER( i );
			gtk_tree_model_row_inserted( GTK_TREE_MODEL( store ), path, &iter );
		}
		else
		{
			gtk_tree_path_append_index( path, store->n_rows-i-1 );
			gtk_tree_model_row_deleted( GTK_TREE_MODEL( store ), path );
		}
		gtk_tree_path_free( path );
	}
	for( i = 0; i < n_rows; i++ )
	{
		path = gtk_tree_path_new();
		gtk_tree_path_append_index( path, i );
		iter.stamp = store->stamp;
		iter.user_data = GINT_TO_POINTER( i );
		gtk_tree_model_row_changed( GTK_TREE_MODEL( store ), path, &iter );
		if( path )
			gtk_tree_path_free( path );
	}
	store->n_rows = n_rows;
}

void
pgm_data_store_row_changed( PgmDataStore *store, gint row )
{
	GtkTreePath *path;
	GtkTreeIter iter;

	g_return_if_fail( store != NULL );
	g_return_if_fail( row >= 0 );
	g_return_if_fail( row < pgm_query_get_rows( store->query ) );

	path = gtk_tree_path_new();
	gtk_tree_path_append_index( path, row );
	iter.stamp = store->stamp;
	iter.user_data = GINT_TO_POINTER( row );
	gtk_tree_model_row_changed( GTK_TREE_MODEL( store ), path, &iter );
	gtk_tree_path_free( path );
}

static gchar*
pgm_data_store_prepare_sorted_query( PgmDataStore *store )
{
	GString *sorted_query;
	if( store->query_str == NULL || *store->query_str == 0 )
		return NULL;
	sorted_query = g_string_new( store->query_str );
	if( sorted_query->len > 0 )
	{
		gchar *pos_str;
		gint insert_pos = sorted_query->len;
		if( sorted_query->str[ insert_pos-1 ] == ';' )
			insert_pos--;
		pos_str = g_strdup_printf( " order by %d %s", store->sort_column_id+1, (store->order == GTK_SORT_ASCENDING ? "asc" : "desc") );
		g_string_insert( sorted_query, insert_pos, pos_str );
		g_free( pos_str );
	}
	return g_string_free( sorted_query, FALSE );
}
