#ifndef PGMSCHEMA_H
#define PGMSCHEMA_H

#include "pgmobject.h"
#include "pgmschemalist.h"

#define PGM_TYPE_SCHEMA                    (pgm_schema_get_type())
#define PGM_SCHEMA(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_SCHEMA, PgmSchema ))
#define PGM_IS_SCHEMA(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_SCHEMA) )
#define PGM_SCHEMA_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_SCHEMA, PgmSchemaClass ))
#define PGM_IS_SCHEMA_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_SCHEMA ))
#define PGM_SCHEMA_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_SCHEMA, PgmSchemaClass ))

G_BEGIN_DECLS

typedef struct _PgmSchema PgmSchema;
typedef struct _PgmSchemaClass PgmSchemaClass;

struct _PgmSchema
{
	PgmObject parent_instance;
};

struct _PgmSchemaClass
{
	PgmObjectClass parent_instance;
};

GType pgm_schema_get_type();
PgmSchema* pgm_schema_new( PgmSchemaList *list, const gchar *name );

G_END_DECLS

#endif /* PGMSCHEMA_H */
