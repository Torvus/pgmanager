#include "pgmtable.h"

#include "pgmstock.h"
#include "pgmmainwindow.h"
#include "pgmtableview.h"

G_DEFINE_TYPE( PgmTable, pgm_table, PGM_TYPE_OBJECT )

static void pgm_table_init( PgmTable *table );
static void pgm_table_class_init( PgmTableClass *klass );
static void pgm_table_activate( PgmObject *object );

static void
pgm_table_init( PgmTable *table )
{}

static void
pgm_table_class_init( PgmTableClass *klass )
{
	PgmObjectClass *object_class = PGM_OBJECT_CLASS( klass );

	object_class->activate = pgm_table_activate;
}

static void
pgm_table_activate( PgmObject *object )
{
	PgmTable *table;

	table = PGM_TABLE( object );

	if( !table->widget || !GTK_IS_WIDGET( table->widget ) )
		table->widget = pgm_table_view_new( table );

	pgm_main_window_append_widget( table->widget, pgm_object_get_name( PGM_OBJECT( table ) ), PGM_STOCK_TABLE );
}

PgmTable*
pgm_table_new( PgmTableList *list, const gchar *name )
{
	return g_object_new( PGM_TYPE_TABLE,
						 "parent", list,
						 "name", name,
						 "icon", PGM_STOCK_TABLE,
	                     NULL );
}
