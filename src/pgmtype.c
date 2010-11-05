#include "pgmtype.h"

#include "pgmquery.h"

#define BOOLOID			16
#define CHAROID         18
#define INT8OID			20
#define INT2OID			21
#define INT4OID			23
#define TEXTOID			25
#define OIDOID          26
#define FLOAT4OID		700
#define FLOAT8OID		701
#define NUMERICOID		1700

static GType
pgm_type_get_gtype_for_type_size( gint size, gboolean floated )
{
	if( floated )
	{
		if( size == sizeof( float ) )
			return G_TYPE_FLOAT;
		return G_TYPE_DOUBLE;
	}
	switch( size )
	{
		case sizeof( char ):      return G_TYPE_CHAR;
		case sizeof( short int ): return G_TYPE_UCHAR;
		case sizeof( int ):       return G_TYPE_INT;
		case sizeof( long int ):  return G_TYPE_LONG;
	}
	return G_TYPE_INVALID;
}

GType
pgm_type_get_gtype( guint oid )
{
	GType type = G_TYPE_INVALID;
	switch( oid )
	{
		case BOOLOID: return G_TYPE_BOOLEAN;
		case CHAROID:
			type = pgm_type_get_gtype_for_type_size( 1, FALSE );
			break;
		case INT2OID:
			type = pgm_type_get_gtype_for_type_size( 2, FALSE );
			break;
		case OIDOID:
		case INT4OID:
			type = pgm_type_get_gtype_for_type_size( 4, FALSE );
			break;
		case INT8OID:
			type = pgm_type_get_gtype_for_type_size( 8, FALSE );
			break;
		case FLOAT4OID:
			type = pgm_type_get_gtype_for_type_size( 4, TRUE );
			break;
		case FLOAT8OID:
			type = pgm_type_get_gtype_for_type_size( 8, TRUE );
			break;
	}
	if( type == G_TYPE_INVALID )
		return G_TYPE_STRING;
	return type;
}
