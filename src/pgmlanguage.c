#include "pgmlanguage.h"

#include "pgmstock.h"

G_DEFINE_TYPE( PgmLanguage, pgm_language, PGM_TYPE_OBJECT )

static void pgm_language_init( PgmLanguage *lang );
static void pgm_language_class_init( PgmLanguageClass *klass );

static void
pgm_language_init( PgmLanguage *lang )
{}

static void
pgm_language_class_init( PgmLanguageClass *klass )
{}

PgmLanguage*
pgm_language_new( PgmLanguageList *list, const gchar *name )
{
	return g_object_new( PGM_TYPE_LANGUAGE,
	                     "parent", list,
						 "name", name,
						 "icon", PGM_STOCK_LANGUAGE,
						 NULL );
}
