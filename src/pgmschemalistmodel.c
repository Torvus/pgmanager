#include "pgmschemalistmodel.h"

#include "pgmdatabase.h"

GtkListStore*
pgm_schema_list_model_new( PgmObject *object )
{
	GtkListStore *store;

	store = gtk_list_store_new( 1, G_TYPE_STRING );

	pgm_schema_list_model_fill( store, object );

	return store;
}

void
pgm_schema_list_model_fill( GtkListStore *store, PgmObject *object )
{
	GtkTreeIter iter;
	PgmObject *database, *schema_list, *schema;
	gint i;

	g_return_if_fail( store != NULL );

	if( object == NULL || !PGM_IS_OBJECT( object ) )
		return;

	database = pgm_object_get_database( object );
	schema_list = PGM_DATABASE( database )->schema_list;
	for( i = 0; i < pgm_object_get_n_childs( schema_list ); i++ )
	{
		schema = pgm_object_get_nth_child( schema_list, i );
		gtk_list_store_append( store, &iter );
		gtk_list_store_set( store, &iter, 0, pgm_object_get_name( schema ), -1 );
	}
}
