#include "pgmtablelist.h"

#include "pgmstock.h"
#include "pgmtranslate.h"
#include "pgmdatabase.h"
#include "pgmquery.h"
#include "pgmtable.h"

G_DEFINE_TYPE( PgmTableList, pgm_table_list, PGM_TYPE_OBJECT )

static void pgm_table_list_real_shown( PgmObject *object );

static void
pgm_table_list_init( PgmTableList *list )
{}

static void
pgm_table_list_class_init( PgmTableListClass *klass )
{
	PgmObjectClass *object_class = PGM_OBJECT_CLASS( klass );

	object_class->real_shown = pgm_table_list_real_shown;
}

PgmTableList*
pgm_table_list_new( PgmSchema *schema )
{
	return g_object_new( PGM_TYPE_TABLE_LIST,
	                     "parent", schema,
						 "name", _("Tables"),
						 "icon", PGM_STOCK_TABLES,
						 NULL );
}

static void
pgm_table_list_real_shown( PgmObject *object )
{
	PgmTableList *list;
	PgmObject *schema;
	PgmDatabase *database;
	PgmQuery *query;
	PgmTable *table;
	gint row;

	g_return_if_fail( object != NULL );
	g_return_if_fail( PGM_IS_TABLE_LIST( object ) );

	list = PGM_TABLE_LIST( object );
	database = PGM_DATABASE( pgm_object_get_database( object ) );
	schema = pgm_object_get_parent( object );

	query = pgm_query_new( database );

	if( pgm_query_exec( query, "select c.relname as tablename "
								 "from pg_class c "
							"left join pg_namespace n on n.oid = c.relnamespace "
								"where c.relkind = 'r'::\"char\""
								  "and n.nspname = '%s';", pgm_object_get_name( schema ) ) )
	{
		for( row = 0; row < pgm_query_get_rows( query ); row++ )
		{
			table = pgm_table_new( list, pgm_query_get_value( query, row, 0 ) );
			pgm_object_show( PGM_OBJECT( table ) );
		}
	}

	g_object_unref( G_OBJECT( query ) );
}
