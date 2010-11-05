#include "pgmserverlist.h"

#include "pgmtranslate.h"
#include "pgmstock.h"

G_DEFINE_TYPE( PgmServerList, pgm_server_list, PGM_TYPE_OBJECT )

static void pgm_server_list_init( PgmServerList *list );
static void pgm_server_list_class_init( PgmServerListClass *klass );

static void
pgm_server_list_init( PgmServerList *list )
{}

static void
pgm_server_list_class_init( PgmServerListClass *klass )
{}

PgmServerList*
pgm_server_list_new()
{
	return g_object_new( PGM_TYPE_SERVER_LIST,
	                     "name", _("Servers"),
						 "icon", PGM_STOCK_SERVERS,
	                     NULL );
}
