#ifndef PGMQUERY_H
#define PGMQUERY_H

#include <gtk/gtk.h>
#include <libpq-fe.h>
#include "pgmdatabase.h"

#define PGM_TYPE_QUERY                    (pgm_query_get_type())
#define PGM_QUERY(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_QUERY, PgmQuery ))
#define PGM_IS_QUERY(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_QUERY) )
#define PGM_QUERY_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_QUERY, PgmQueryClass ))
#define PGM_IS_QUERY_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_QUERY ))
#define PGM_QUERY_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_QUERY, PgmQueryClass ))

G_BEGIN_DECLS

typedef struct _PgmQuery PgmQuery;
typedef struct _PgmQueryClass PgmQueryClass;

struct _PgmQuery
{
	GObject parent;

	PgmDatabase *database;
	PGresult *result;
	gchar *last_query;
	gboolean is_copy;
};

struct _PgmQueryClass
{
	GObjectClass parent;

	/*< signals >*/
	void (*executed) ( PgmQuery *query );
};

GType     pgm_query_get_type          ();
PgmQuery* pgm_query_new               ( PgmDatabase *db );
PgmQuery* pgm_query_new_with_query    ( PgmDatabase *db, const gchar *query, ... );
PgmQuery* pgm_query_copy              ( PgmQuery *query );
void      pgm_query_set_database      ( PgmQuery *query, PgmDatabase *db );
gboolean  pgm_query_exec              ( PgmQuery *query, const gchar *query_str, ... );
gboolean  pgm_query_reexec            ( PgmQuery *query );
gint      pgm_query_get_columns       ( PgmQuery *query );
gint      pgm_query_get_rows          ( PgmQuery *query );
gint      pgm_query_get_column_number ( PgmQuery *query, const gchar *column_name );
gchar*    pgm_query_get_column_name   ( PgmQuery *query, gint column_number );
gchar*    pgm_query_get_value         ( PgmQuery *query, gint row, gint column );
gboolean  pgm_query_get_gvalue        ( PgmQuery *query, gint row, gint column, GValue *value );
gchar*    pgm_query_get_last_query    ( PgmQuery *query );
GType     pgm_query_get_column_gtype  ( PgmQuery *query, gint column );
gboolean  pgm_query_get_is_null       ( PgmQuery *query, gint row, gint column );

G_END_DECLS

#endif /* PGMQUERY_H */
