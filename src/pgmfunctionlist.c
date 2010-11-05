#include "pgmfunctionlist.h"

#include "pgmstock.h"
#include "pgmtranslate.h"
#include "pgmquery.h"
#include "pgmfunction.h"

G_DEFINE_TYPE( PgmFunctionList, pgm_function_list, PGM_TYPE_OBJECT )

static void pgm_function_list_init( PgmFunctionList *list );
static void pgm_function_list_class_init( PgmFunctionListClass *klass );
static void pgm_function_list_real_shown( PgmObject *object );

static void
pgm_function_list_init( PgmFunctionList *list )
{}

static void
pgm_function_list_class_init( PgmFunctionListClass *klass )
{
	PgmObjectClass *object_class = PGM_OBJECT_CLASS( klass );

	object_class->real_shown = pgm_function_list_real_shown;
}

PgmFunctionList*
pgm_function_list_new( PgmSchema *schema )
{
	return g_object_new( PGM_TYPE_FUNCTION_LIST,
	                     "parent", schema,
						 "name", _("Functions"),
						 "icon", PGM_STOCK_FUNCTIONS,
	                     NULL );
}

static void
pgm_function_list_real_shown( PgmObject *object )
{
	PgmFunctionList *list;
	PgmSchema *schema;
	PgmQuery *query;
	PgmFunction *function;

	list = PGM_FUNCTION_LIST( object );
	schema = PGM_SCHEMA( pgm_object_get_schema( object ) );

	query = pgm_query_new( PGM_DATABASE( pgm_object_get_database( object ) ) );

	if( pgm_query_exec( query, "SELECT proname, "
									  "oidvectortypes( proargtypes ) as proargtypes "
								 "FROM pg_proc pr, "
                                      "pg_type tp, "
                                      "pg_namespace ns "
								"WHERE tp.oid = pr.prorettype "
                                  "AND pr.proisagg = FALSE "
                                  "AND tp.typname <> 'trigger' "
                                  "AND ns.oid = pr.pronamespace "
                                  "AND ns.nspname = '%s';", pgm_object_get_name( PGM_OBJECT( schema ) ) ) )
	{
		gint row;
		for( row = 0; row < pgm_query_get_rows( query ); row++ )
		{
			function = pgm_function_new( list,
										 pgm_query_get_value( query, row, 0 ),
										 pgm_query_get_value( query, row, 1 ) );
			pgm_object_show( PGM_OBJECT( function ) );
		}
	}

	g_object_unref( query );
}
