#include "pgmserver.h"

#include "pgmstock.h"
#include "pgmdatabase.h"
#include "pgmquery.h"

G_DEFINE_TYPE( PgmServer, pgm_server, PGM_TYPE_OBJECT )

static void pgm_server_init( PgmServer *server );
static void pgm_server_class_init( PgmServerClass *klass );
static void pgm_server_finalize( GObject *gobject );

static void
pgm_server_init( PgmServer *server )
{
	server->host = NULL;
	server->port = -1;
	server->login = NULL;
}

static void
pgm_server_class_init( PgmServerClass *klass )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( klass );

	gobject_class->finalize = pgm_server_finalize;
}

static void
pgm_server_finalize( GObject *gobject )
{
	PgmServer *server = PGM_SERVER( gobject );

	if( server->host )
		g_free( server->host );
	if( server->login )
		g_free( server->login );

	G_OBJECT_CLASS( pgm_server_parent_class )->finalize( gobject );
}

PgmServer*
pgm_server_new( PgmServerList *list, const gchar *host, gint port, const gchar *login, PgmSslMode ssl_mode, gboolean password_saved )
{
	PgmServer *server;

	server = g_object_new( PGM_TYPE_SERVER,
	                       "parent", list,
						   "name", host,
						   "icon", PGM_STOCK_SERVER,
	                       NULL );

	server->host = g_strdup( host );
	server->port = port;
	server->login = g_strdup( login );
	server->ssl_mode = ssl_mode;
	server->password_saved = password_saved;

	return server;
}

gchar*
pgm_server_get_host( PgmServer *server )
{
	g_return_val_if_fail( server != NULL, NULL );
	return server->host;
}

gint
pgm_server_get_port( PgmServer *server )
{
	g_return_val_if_fail( server != NULL, -1 );
	return server->port;
}

gchar*
pgm_server_get_login( PgmServer *server )
{
	g_return_val_if_fail( server != NULL, NULL );
	return server->login;
}

PgmSslMode
pgm_server_get_ssl_mode( PgmServer *server )
{
	g_return_val_if_fail( server != NULL, PGM_SSL_PREFER );
	return server->ssl_mode;
}

gboolean
pgm_server_read_databases( PgmServer *server )
{
	PgmDatabase *db;
	PgmDatabase *db1;
	PgmQuery *query;
	int row;
	gboolean ret = TRUE;;

	g_return_val_if_fail( server != NULL, FALSE );

	db = pgm_database_new( server, "template1" );
	query = pgm_query_new( db );

	if( pgm_query_exec( query, "select datname from pg_catalog.pg_database;" ) )
	{
		for( row = 0; row < pgm_query_get_rows( query ); row++ )
		{
			db1 = pgm_database_new( server, pgm_query_get_value( query, row, 0 ) );
			pgm_object_show( PGM_OBJECT( db1 ) );
		}
	}
	else
		ret = FALSE;

	g_object_unref( G_OBJECT( query ) );
	g_object_unref( G_OBJECT( db ) );

	return ret;
}
