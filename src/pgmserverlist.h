#ifndef PGMSERVERLIST_H
#define PGMSERVERLIST_H

#include "pgmobject.h"

#define PGM_TYPE_SERVER_LIST                    (pgm_server_list_get_type())
#define PGM_SERVER_LIST(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_SERVER_LIST, PgmServerList ))
#define PGM_IS_SERVER_LIST(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_SERVER_LIST) )
#define PGM_SERVER_LIST_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_SERVER_LIST, PgmServerListClass ))
#define PGM_IS_SERVER_LIST_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_SERVER_LIST ))
#define PGM_SERVER_LIST_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_SERVER_LIST, PgmServerListClass ))

G_BEGIN_DECLS

typedef struct _PgmServerList           PgmServerList;
typedef struct _PgmServerListClass      PgmServerListClass;

struct _PgmServerList
{
	PgmObject parent;
};

struct _PgmServerListClass
{
	PgmObjectClass parent;
};

GType pgm_server_list_get_type();
PgmServerList* pgm_server_list_new();

G_END_DECLS

#endif /* PGMSERVERLIST_H */
