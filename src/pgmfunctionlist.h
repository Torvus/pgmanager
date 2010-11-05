#ifndef PGMFUNCTIONLIST_H
#define PGMFUNCTIONLIST_H

#include "pgmobject.h"
#include "pgmschema.h"

#define PGM_TYPE_FUNCTION_LIST                    (pgm_function_list_get_type())
#define PGM_FUNCTION_LIST(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_FUNCTION_LIST, PgmFunctionList ))
#define PGM_IS_FUNCTION_LIST(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_FUNCTION_LIST) )
#define PGM_FUNCTION_LIST_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_FUNCTION_LIST, PgmFunctionListClass ))
#define PGM_IS_FUNCTION_LIST_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_FUNCTION_LIST ))
#define PGM_FUNCTION_LIST_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_FUNCTION_LIST, PgmFunctionListClass ))

G_BEGIN_DECLS

typedef struct _PgmFunctionList PgmFunctionList;
typedef struct _PgmFunctionListClass PgmFunctionListClass;

struct _PgmFunctionList
{
	PgmObject parent_instance;
};

struct _PgmFunctionListClass
{
	PgmObjectClass parent_class;
};

GType pgm_function_list_get_type();
PgmFunctionList* pgm_function_list_new( PgmSchema *schema );

G_END_DECLS

#endif /* PGMFUNCTIONLIST_H */
