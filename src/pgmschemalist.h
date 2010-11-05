#ifndef PGMSCHEMALIST_H
#define PGMSCHEMALIST_H

#include "pgmobject.h"
#include "pgmdatabase.h"

#define PGM_TYPE_SCHEMA_LIST                    (pgm_schema_list_get_type())
#define PGM_SCHEMA_LIST(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_SCHEMA_LIST, PgmSchemaList ))
#define PGM_IS_SCHEMA_LIST(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_SCHEMA_LIST) )
#define PGM_SCHEMA_LIST_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_SCHEMA_LIST, PgmSchemaListClass ))
#define PGM_IS_SCHEMA_LIST_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_SCHEMA_LIST ))
#define PGM_SCHEMA_LIST_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_SCHEMA_LIST, PgmSchemaListClass ))

G_BEGIN_DECLS

typedef struct _PgmSchemaList PgmSchemaList;
typedef struct _PgmSchemaListClass PgmSchemaListClass;

struct _PgmSchemaList
{
	PgmObject parent;
};

struct _PgmSchemaListClass
{
	PgmObjectClass parent;
};

GType pgm_schema_list_get_type();
PgmSchemaList* pgm_schema_list_new( PgmDatabase *db );

G_END_DECLS

#endif /* PGMSCHEMALIST_H */
