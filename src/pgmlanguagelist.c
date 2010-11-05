#include "pgmlanguagelist.h"

#include "pgmstock.h"
#include "pgmtranslate.h"
#include "pgmquery.h"
#include "pgmlanguage.h"

G_DEFINE_TYPE( PgmLanguageList, pgm_language_list, PGM_TYPE_OBJECT )

static void pgm_language_list_init( PgmLanguageList *list );
static void pgm_language_list_class_init( PgmLanguageListClass *klass );
static void pgm_language_list_real_shown( PgmObject *object );

static void
pgm_language_list_init( PgmLanguageList *list )
{}

static void
pgm_language_list_class_init( PgmLanguageListClass *klass )
{
	PgmObjectClass *pclass = PGM_OBJECT_CLASS( klass );

	pclass->real_shown = pgm_language_list_real_shown;
}

PgmLanguageList*
pgm_language_list_new( PgmDatabase *db )
{
	return g_object_new( PGM_TYPE_LANGUAGE_LIST,
	                     "parent", db,
						 "name", _("Languages"),
						 "icon", PGM_STOCK_LANGUAGES,
	                     NULL );
}

static void
pgm_language_list_real_shown( PgmObject *object )
{
	PgmLanguageList *list;
	PgmQuery *query;

	list = PGM_LANGUAGE_LIST( object );
	query = pgm_query_new( PGM_DATABASE( pgm_object_get_database( object ) ) );

	if( pgm_query_exec( query, "select lanname from pg_catalog.pg_language order by 1;" ) )
	{
		gint row = 0;
		PgmLanguage *lang;

		for( row = 0; row < pgm_query_get_rows( query ); row++ )
		{
			lang = pgm_language_new( list, pgm_query_get_value( query, row, 0 ) );
			pgm_object_show( PGM_OBJECT( lang ) );
		}
	}

	g_object_unref( query );
}
