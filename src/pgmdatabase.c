#include "pgmdatabase.h"

#include "pgmstock.h"
#include "pgmmainwindow.h"
#include "pgmquery.h"
#include "pgmschemalist.h"
#include "pgmlanguagelist.h"

G_DEFINE_TYPE( PgmDatabase, pgm_database, PGM_TYPE_OBJECT )

static void pgm_database_init( PgmDatabase *db );
static void pgm_database_class_init( PgmDatabaseClass *klass );
static void pgm_database_dispose( GObject *gobject );
static void pgm_database_activate( PgmObject *object );
static PgmObject* pgm_database_on_get_database( PgmObject *object );

static void
pgm_database_init( PgmDatabase *db )
{
	db->connected = FALSE;
	db->connection = NULL;
	db->schema_list = NULL;
	db->language_list = NULL;
}

static void
pgm_database_class_init( PgmDatabaseClass *klass )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( klass );
	PgmObjectClass *object_class = PGM_OBJECT_CLASS( klass );

	object_class->activate = pgm_database_activate;
	object_class->get_database = pgm_database_on_get_database;

	gobject_class->dispose = pgm_database_dispose;
}

static void
pgm_database_dispose( GObject *gobject )
{
	PgmDatabase *db = PGM_DATABASE( gobject );

	if( db->connected )
		PQfinish( db->connection );

	G_OBJECT_CLASS( pgm_database_parent_class )->dispose( gobject );
}

PgmDatabase*
pgm_database_new( PgmServer *server, const gchar *name )
{
	return g_object_new( PGM_TYPE_DATABASE,
	                     "parent", server,
						 "name", name,
						 "icon", PGM_STOCK_DATABASE_CLOSED,
						 "show-child-count", FALSE,
						 NULL );
}

gboolean
pgm_database_connect( PgmDatabase *db )
{
	GString *connection_str;
	gchar *host;
	gint port;
	gchar *dbname;
	gchar *user;
	gchar *sslmode;
	PgmObject *parent;
	PGresult *result;

	g_return_val_if_fail( db != NULL, FALSE );

	if( db->connected )
		return TRUE;

	pgm_main_window_start_wait();

	parent = pgm_object_get_parent( PGM_OBJECT( db ) );

	g_return_val_if_fail( PGM_IS_SERVER( parent ), FALSE );

	host = pgm_server_get_host( PGM_SERVER( parent ) );
	port = pgm_server_get_port( PGM_SERVER( parent ) );
	dbname = pgm_object_get_name( PGM_OBJECT( db ) );
	user = pgm_server_get_login( PGM_SERVER( parent ) );
	switch( pgm_server_get_ssl_mode( PGM_SERVER( parent ) ) )
	{
		case PGM_SSL_ALLOW:
							sslmode = g_strdup( "allow" );
							break;
		case PGM_SSL_DISABLE:
							sslmode = g_strdup( "disable" );
							break;
		case PGM_SSL_PREFER:
							sslmode = g_strdup( "prefer" );
							break;
		case PGM_SSL_REQUIRE:
							sslmode = g_strdup( "require" );
							break;
	}
	connection_str = g_string_new( NULL );
	g_string_printf( connection_str, "host='%s' port=%d dbname='%s' user='%s' sslmode=%s",
					 host, port, dbname, user, sslmode );

	db->connection = PQconnectdb( connection_str->str );

	g_string_free( connection_str, TRUE );
	g_free( sslmode );

	if( PQstatus( db->connection ) == CONNECTION_BAD )
	{
		pgm_main_window_show_message_dialog( GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Unable to connect to daatabase:\n%s", PQerrorMessage( db->connection ) );
		pgm_main_window_stop_wait();
		return FALSE;
	}

	result = PQexec( db->connection, "set client_encoding to 'unicode';" );
	PQclear( result );
	db->connected = TRUE;
	pgm_main_window_stop_wait();
	return TRUE;
}

void
pgm_database_disconnect( PgmDatabase *db )
{
	g_return_if_fail( db != NULL );

	if( !db->connected )
		return;

	PQfinish( db->connection );
}

gboolean
pgm_database_is_connected( PgmDatabase *db )
{
	g_return_val_if_fail( db != NULL, FALSE );

	return db->connected;
}

static void
pgm_database_activate( PgmObject *object )
{
	PgmSchemaList *schema_list;
	PgmLanguageList *language_list;
	PgmDatabase *db;

	g_return_if_fail( object != NULL );

	db = PGM_DATABASE( object );

	if( !pgm_database_connect( db ) )
		return;

	if( db->schema_list == NULL )
	{
		schema_list = pgm_schema_list_new( db );
		db->schema_list = PGM_OBJECT( schema_list );
		pgm_object_show( db->schema_list );
	}
	if( db->language_list == NULL )
	{
		language_list = pgm_language_list_new( db );
		db->language_list = PGM_OBJECT( language_list );
		pgm_object_show( db->language_list );
	}

	pgm_object_set_icon( object, PGM_STOCK_DATABASE );
	PGM_OBJECT_CLASS( pgm_database_parent_class )->activate( object );
}

static PgmObject*
pgm_database_on_get_database( PgmObject *object )
{
	g_return_val_if_fail( object != NULL, NULL );
	return object;
}
