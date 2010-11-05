#include "pgmschemalist.h"

#include "pgmstock.h"
#include "pgmtranslate.h"
#include "pgmquery.h"
#include "pgmschema.h"

G_DEFINE_TYPE( PgmSchemaList, pgm_schema_list, PGM_TYPE_OBJECT )

static void pgm_schema_list_init( PgmSchemaList *list );
static void pgm_schema_list_class_init( PgmSchemaListClass *klass );
static void pgm_schema_list_real_shown( PgmObject *object );

static void
pgm_schema_list_init( PgmSchemaList *list )
{}

static void
pgm_schema_list_class_init( PgmSchemaListClass *klass )
{
	PgmObjectClass *object_class = PGM_OBJECT_CLASS( klass );

	object_class->real_shown = pgm_schema_list_real_shown;
}

PgmSchemaList*
pgm_schema_list_new( PgmDatabase *db )
{
	return g_object_new( PGM_TYPE_SCHEMA_LIST,
	                     "parent", db,
						 "name", _("Schemas"),
						 "icon", PGM_STOCK_SCHEMAS,
						 NULL );
}

static void
pgm_schema_list_real_shown( PgmObject *object )
{
	PgmSchemaList *list;
	PgmDatabase *database;
	PgmQuery *query;
	PgmSchema *schema;
	gint row;

	g_return_if_fail( object != NULL );
	g_return_if_fail( PGM_IS_SCHEMA_LIST( object ) );

	list = PGM_SCHEMA_LIST( object );
	database = PGM_DATABASE( pgm_object_get_database( object ) );

	query = pgm_query_new( database );

	if( pgm_query_exec( query, "select nspname from pg_catalog.pg_namespace;" ) )
	{
		for( row = 0; row < pgm_query_get_rows( query ); row++ )
		{
			schema = pgm_schema_new( list, pgm_query_get_value( query, row, 0 ) );
			pgm_object_show( PGM_OBJECT( schema ) );
		}
	}

	g_object_unref( query );
}
