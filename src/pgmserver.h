#ifndef PGMSERVER_H
#define PGMSERVER_H

#include "pgmobject.h"
#include "pgmserverlist.h"

#define PGM_TYPE_SERVER                    (pgm_server_get_type())
#define PGM_SERVER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_SERVER, PgmServer ))
#define PGM_IS_SERVER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_SERVER) )
#define PGM_SERVER_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_SERVER, PgmServerClass ))
#define PGM_IS_SERVER_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_SERVER ))
#define PGM_SERVER_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_SERVER, PgmServerClass ))

typedef struct _PgmServer         PgmServer;
typedef struct _PgmServerClass    PgmServerClass;

G_BEGIN_DECLS

typedef enum
{
	PGM_SSL_DISABLE,
	PGM_SSL_ALLOW,
	PGM_SSL_PREFER,
	PGM_SSL_REQUIRE
} PgmSslMode;

struct _PgmServer
{
	PgmObject parent_instance;

	gchar *host;
	gint port;
	gchar *login;
	PgmSslMode ssl_mode;
	gboolean password_saved;
};

struct _PgmServerClass
{
	PgmObjectClass parent_instance;
};

GType         pgm_server_get_type       ();
PgmServer *   pgm_server_new            ( PgmServerList *list, const gchar *host, gint port, const gchar *login, PgmSslMode ssl_mode, gboolean password_saved );
gchar *       pgm_server_get_host       ( PgmServer *server );
gint          pgm_server_get_port       ( PgmServer *server );
gchar *       pgm_server_get_login      ( PgmServer *server );
PgmSslMode    pgm_server_get_ssl_mode   ( PgmServer *server );
gboolean      pgm_server_read_databases ( PgmServer *server );

G_END_DECLS

#endif /* PGMSERVER_H */
