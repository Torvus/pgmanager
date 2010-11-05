#include "pgmquery.h"

#include "pgmmainwindow.h"

#include <glib.h>
#include <glib/gprintf.h>

#define BOOLOID			16
#define INT8OID			20
#define INT2OID			21
#define INT4OID			23
#define TEXTOID			25
#define FLOAT4OID		700
#define FLOAT8OID		701
#define NUMERICOID		1700

G_DEFINE_TYPE( PgmQuery, pgm_query, G_TYPE_OBJECT )

static void pgm_query_init( PgmQuery *query );
static void pgm_query_class_init( PgmQueryClass *klass );
static void pgm_query_finalize( GObject *gobject );
static gboolean pgm_query_vexec( PgmQuery *query, const gchar *format, va_list va );

enum {
	SIGNAL_EXECUTED,
	LAST_SIGNAL
};

static guint pgm_query_signals[ LAST_SIGNAL ] = {0};

static void
pgm_query_init( PgmQuery *query )
{
	query->database = NULL;
	query->last_query = NULL;
	query->result = NULL;
	query->is_copy = FALSE;
}

static void
pgm_query_class_init( PgmQueryClass *klass )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( klass );
	gobject_class->finalize = pgm_query_finalize;

	pgm_query_signals[ SIGNAL_EXECUTED ] =
			g_signal_new( "executed",
						  G_OBJECT_CLASS_TYPE( gobject_class ),
						  G_SIGNAL_RUN_LAST,
						  G_STRUCT_OFFSET( PgmQueryClass, executed ),
						  NULL, NULL,
						  g_cclosure_marshal_VOID__VOID,
						  G_TYPE_NONE, 0 );
}

static void
pgm_query_finalize( GObject *gobject )
{
	PgmQuery *query = PGM_QUERY( gobject );

	if( query->last_query )
		g_free( query->last_query );
	if( query->result )
		PQclear( query->result );

	g_object_unref( G_OBJECT( query->database ) );

	G_OBJECT_CLASS( pgm_query_parent_class )->finalize( gobject );
}

static gboolean
pgm_query_vexec( PgmQuery *query, const gchar *format, va_list va )
{
	gchar *query_str;

	g_return_val_if_fail( query != NULL, FALSE );
	g_return_val_if_fail( query->database != NULL, FALSE );

	if( !pgm_database_connect( query->database ) )
		return FALSE;

	g_vasprintf( &query_str, format, va );

	if( query->is_copy )
	{
		query->is_copy = FALSE;
		query->result = NULL;
	}

	if( query->result )
	{
		PQclear( query->result );
		query->result = NULL;
	}
	if( format == NULL || *format == 0 )
		return TRUE;

	pgm_main_window_start_wait();

	query->result = PQexec( query->database->connection, query_str );

	pgm_main_window_stop_wait();

	g_signal_emit( query, pgm_query_signals[ SIGNAL_EXECUTED ], 0 );

	if( PQresultStatus( query->result ) != PGRES_COMMAND_OK && PQresultStatus( query->result ) != PGRES_TUPLES_OK )
	{
		PQclear( query->result );
		query->result = NULL;
		pgm_main_window_show_message_dialog( GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error executing query:\n%s", PQerrorMessage( query->database->connection ) );
		return FALSE;
	}
	if( query->last_query )
		g_free( query->last_query );
	query->last_query = g_strdup( query_str );
	return TRUE;
}

PgmQuery*
pgm_query_new( PgmDatabase *db )
{
	PgmQuery *query;

	query = g_object_new( PGM_TYPE_QUERY, NULL );

	query->database = db;

	if( db )
		g_object_ref( db );

	return query;
}

PgmQuery*
pgm_query_new_with_query( PgmDatabase *db, const gchar *query_str, ... )
{
	PgmQuery *query;
	va_list ap;

	query = g_object_new( PGM_TYPE_QUERY, NULL );

	query->database = db;

	if( db )
		g_object_ref( db );

	va_start( ap, query_str );
	pgm_query_vexec( query, query_str, ap );
	va_end( ap );

	return query;
}

PgmQuery*
pgm_query_copy( PgmQuery *query )
{
	PgmQuery *new_query;

	new_query = g_object_new( PGM_TYPE_QUERY, NULL );

	new_query->database = query->database;
	new_query->last_query = g_strdup( query->last_query );
	new_query->result = query->result;
	new_query->is_copy = TRUE;

	return new_query;
}

void
pgm_query_set_database( PgmQuery *query, PgmDatabase *db )
{
	g_return_if_fail( query != NULL );

	if( query->database )
		g_object_unref( query->database );
	query->database = db;
	if( query->database )
		g_object_ref( query->database );
}

gboolean
pgm_query_exec( PgmQuery *query, const gchar *query_str, ... )
{
	gboolean ret_val;
	va_list ap;

	va_start( ap, query_str );
	ret_val = pgm_query_vexec( query, query_str, ap );
	va_end( ap );
	return ret_val;
}

gboolean
pgm_query_reexec( PgmQuery *query )
{
	return pgm_query_exec( query, query->last_query );
}

gint
pgm_query_get_columns( PgmQuery *query )
{
	g_return_val_if_fail( query != NULL, 0 );
	return PQnfields( query->result );
}

gint
pgm_query_get_rows( PgmQuery *query )
{
	g_return_val_if_fail( query != NULL, 0 );
	return PQntuples( query->result );
}

gint
pgm_query_get_column_number( PgmQuery *query, const gchar *column_name )
{
	g_return_val_if_fail( query != NULL, -1 );
	g_return_val_if_fail( column_name != NULL, -1 );

	return PQfnumber( query->result, column_name );
}

gchar*
pgm_query_get_column_name( PgmQuery *query, gint column_number )
{
	g_return_val_if_fail( query != NULL, NULL );
	g_return_val_if_fail( column_number >= 0, NULL );
	g_return_val_if_fail( column_number < pgm_query_get_columns( query ), NULL );

	return PQfname( query->result, column_number );
}

gchar*
pgm_query_get_value( PgmQuery *query, gint row, gint column )
{
	g_return_val_if_fail( query != NULL, NULL );
	g_return_val_if_fail( column >= 0, NULL );
	g_return_val_if_fail( column < pgm_query_get_columns( query ), NULL );
	g_return_val_if_fail( row >= 0, NULL );
	g_return_val_if_fail( row < pgm_query_get_rows( query ), NULL );

	return PQgetvalue( query->result, row, column );
}

gboolean
pgm_query_get_gvalue( PgmQuery *query, gint row, gint column, GValue *gvalue )
{
	GType type;
	gchar *value;

	g_return_val_if_fail( query != NULL, FALSE );
	g_return_val_if_fail( column >= 0, FALSE );
	g_return_val_if_fail( column < pgm_query_get_columns( query ), FALSE );
	g_return_val_if_fail( row >= 0, FALSE );
	g_return_val_if_fail( row < pgm_query_get_rows( query ), FALSE );

	type = pgm_query_get_column_gtype( query, column );
	g_value_init( gvalue, type );
	value = PQgetvalue( query->result, row, column );

	if( PQgetisnull( query->result, row, column ) )
		return TRUE;

	switch( type )
	{
		case G_TYPE_BOOLEAN:
		{
			if( value[ 0 ] == 't' )
				g_value_set_boolean( gvalue, TRUE );
			else
				g_value_set_boolean( gvalue, FALSE );
			break;
		}
		case G_TYPE_INT:
		{
			int val = (int)g_strtod( value, NULL );
			g_value_set_int( gvalue, val );
			break;
		}
		case G_TYPE_LONG:
		{
			long val = (long)g_strtod( value, NULL );
			g_value_set_long( gvalue, val );
			break;
		}
		case G_TYPE_ULONG:
		{
			unsigned long val = (unsigned long)g_strtod( value, NULL );
			g_value_set_ulong( gvalue, val );
			break;
		}
		case G_TYPE_DOUBLE:
		{
			double val = g_strtod( value, NULL );
			g_value_set_double( gvalue, val );
			break;
		}
		case G_TYPE_FLOAT:
		{
			float val = (float)g_strtod( value, NULL );
			g_value_set_float( gvalue, val );
		}
		default:
		{
			g_value_set_string( gvalue, value );
		}
	}

	return TRUE;
}

gchar*
pgm_query_get_last_query( PgmQuery *query )
{
	g_return_val_if_fail( query != NULL, NULL );
	return query->last_query;
}

GType
pgm_query_get_column_gtype( PgmQuery *query, gint column )
{
	g_return_val_if_fail( query != NULL, G_TYPE_INVALID );
	g_return_val_if_fail( column >= 0, G_TYPE_INVALID );
	g_return_val_if_fail( column < pgm_query_get_columns( query ), G_TYPE_INVALID );

	if( query->result == NULL )
		return G_TYPE_NONE;

	switch( PQftype( query->result, column ) )
	{
		case BOOLOID:   return G_TYPE_BOOLEAN;
		case INT8OID:   return G_TYPE_INT;
		case NUMERICOID:
		case INT2OID:   return G_TYPE_LONG;
		case INT4OID:   return G_TYPE_ULONG;
		case FLOAT4OID: return G_TYPE_DOUBLE;
		case FLOAT8OID: return G_TYPE_FLOAT;
	}
	return G_TYPE_STRING;
}

gboolean
pgm_query_get_is_null( PgmQuery *query, gint row, gint column )
{
	g_return_val_if_fail( query != NULL, FALSE );
	g_return_val_if_fail( column >= 0, FALSE );
	g_return_val_if_fail( column < pgm_query_get_columns( query ), FALSE );
	g_return_val_if_fail( row >= 0, FALSE );
	g_return_val_if_fail( row < pgm_query_get_rows( query ), FALSE );

	return PQgetisnull( query->result, row, column );
}
