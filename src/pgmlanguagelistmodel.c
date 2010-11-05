#include "pgmlanguagelistmodel.h"

#include "pgmdatabase.h"

GtkListStore*
pgm_language_list_model_new( PgmObject *object )
{
	GtkListStore *model;

	model = gtk_list_store_new( 1, G_TYPE_STRING );

	pgm_language_list_model_fill( model, object );

	return model;
}

void
pgm_language_list_model_fill( GtkListStore *store, PgmObject *object )
{
	GtkTreeIter iter;
	PgmObject *database;
	PgmObject *language;
	PgmObject *language_list;
	gint i;

	g_return_if_fail( store != NULL );

	if( object == NULL || !PGM_IS_OBJECT( object ) )
		return;

	database = pgm_object_get_database( object );
	language_list = PGM_DATABASE( database )->language_list;
	for( i = 0; i < pgm_object_get_n_childs( language_list ); i++ )
	{
		language = pgm_object_get_nth_child( language_list, i );
		gtk_list_store_append( store, &iter );
		gtk_list_store_set( store, &iter, 0, pgm_object_get_name( language ), -1 );
	}
}
