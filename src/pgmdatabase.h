#ifndef PGMDATABASE_H
#define PGMDATABASE_H

#include "pgmobject.h"
#include "pgmserver.h"
#include "libpq-fe.h"

#define PGM_TYPE_DATABASE                  (pgm_database_get_type())
#define PGM_DATABASE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_DATABASE, PgmDatabase ))
#define PGM_IS_DATABASE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_DATABASE) )
#define PGM_DATABASE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_DATABASE, PgmDatabaseClass ))
#define PGM_IS_DATABASE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_DATABASE ))
#define PGM_DATABASE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_DATABASE, PgmDatabaseClass ))

G_BEGIN_DECLS

typedef struct _PgmDatabase PgmDatabase;
typedef struct _PgmDatabaseClass PgmDatabaseClass;

struct _PgmDatabase
{
	PgmObject parent_instance;

	/* private */
	gboolean connected;
	PGconn *connection;
	PgmObject *schema_list;
	PgmObject *language_list;
};

struct _PgmDatabaseClass
{
	PgmObjectClass parent_instance;
};

GType        pgm_database_get_type     ();
PgmDatabase* pgm_database_new          ( PgmServer *server, const gchar *name );
gboolean     pgm_database_connect      ( PgmDatabase *db );
void         pgm_database_disconnect   ( PgmDatabase *db );
gboolean     pgm_database_is_connected ( PgmDatabase *db );

G_END_DECLS

#endif /* PGMDATABASE_H */
