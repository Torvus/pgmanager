#include "pgmfunction.h"

#include "pgmstock.h"
#include "pgmfunctionview.h"
#include "pgmmainwindow.h"
#include "pgmquery.h"

G_DEFINE_TYPE( PgmFunction, pgm_function, PGM_TYPE_OBJECT )

static void pgm_function_init( PgmFunction *func );
static void pgm_function_class_init( PgmFunctionClass *klass );

static void pgm_function_activated( PgmObject *object );

static void
pgm_function_init( PgmFunction *func )
{
	func->name = NULL;
	func->args = NULL;
	func->function_view = NULL;
}

static void
pgm_function_class_init( PgmFunctionClass *klass )
{
	PgmObjectClass *pclass = PGM_OBJECT_CLASS( klass );

	pclass->activate = pgm_function_activated;
}

PgmFunction*
pgm_function_new( PgmFunctionList *list, const gchar *name, const gchar *args )
{
	PgmFunction *func;
	gchar *full_name;

	full_name = g_strdup_printf( "%s( %s )", name, args );

	func = g_object_new( PGM_TYPE_FUNCTION,
	                     "parent", list,
						 "name", full_name,
						 "icon", PGM_STOCK_FUNCTION,
	                     NULL );

	func->name = g_strdup( name );
	func->args = g_strdup( args );

	g_free( full_name );

	return func;
}

static void
pgm_function_activated( PgmObject *object )
{
	PgmFunction *func = PGM_FUNCTION( object );

	if( !func->function_view || !GTK_IS_WIDGET( func->function_view ) )
	{
		func->function_view = pgm_function_view_new( func );
	}
	pgm_main_window_append_widget( func->function_view, func->name, PGM_STOCK_FUNCTION );
}
