#ifndef PGMTABLELIST_H
#define PGMTABLELIST_H

#include "pgmobject.h"
#include "pgmschema.h"

#define PGM_TYPE_TABLE_LIST                    (pgm_table_list_get_type())
#define PGM_TABLE_LIST(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_TABLE_LIST, PgmTableList ))
#define PGM_IS_TABLE_LIST(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_TABLE_LIST) )
#define PGM_TABLE_LIST_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_TABLE_LIST, PgmTableListClass ))
#define PGM_IS_TABLE_LIST_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_TABLE_LIST ))
#define PGM_TABLE_LIST_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_TABLE_LIST, PgmTableListClass ))

G_BEGIN_DECLS

typedef struct _PgmTableList PgmTableList;
typedef struct _PgmTableListClass PgmTableListClass;

struct _PgmTableList
{
	PgmObject parent_instance;
};

struct _PgmTableListClass
{
	PgmObjectClass parent_instance;
};

GType         pgm_table_list_get_type ();
PgmTableList* pgm_table_list_new      ( PgmSchema *schema );

G_END_DECLS

#endif /* PGMTABLELIST_H */
