#include "pgmschema.h"

#include "pgmstock.h"
#include "pgmtablelist.h"
#include "pgmfunctionlist.h"

G_DEFINE_TYPE( PgmSchema, pgm_schema, PGM_TYPE_OBJECT )

static void pgm_schema_init( PgmSchema *schema );
static void pgm_schema_class_init( PgmSchemaClass *klass );
static PgmObject* pgm_schema_get_schema( PgmObject *object );

static void
pgm_schema_init( PgmSchema *schema )
{}

static void
pgm_schema_class_init( PgmSchemaClass *klass )
{
	PgmObjectClass *object_class = PGM_OBJECT_CLASS( klass );

	object_class->get_schema = pgm_schema_get_schema;
}

static PgmObject*
pgm_schema_get_schema( PgmObject *object )
{
	g_return_val_if_fail( object != NULL, NULL );
	g_return_val_if_fail( PGM_IS_SCHEMA( object ), NULL );

	return object;
}

PgmSchema*
pgm_schema_new( PgmSchemaList *list, const gchar *name )
{
	PgmSchema *schema;
	PgmTableList *table_list;
	PgmFunctionList *function_list;

	schema = g_object_new( PGM_TYPE_SCHEMA,
	                       "parent", list,
						   "name", name,
						   "icon", PGM_STOCK_SCHEMA,
						   "show-child-count", FALSE,
						   NULL );

	table_list = pgm_table_list_new( schema );
	pgm_object_show( PGM_OBJECT( table_list ) );

	function_list = pgm_function_list_new( schema );
	pgm_object_show( PGM_OBJECT( function_list ) );

	return schema;
}
