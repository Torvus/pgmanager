#ifndef PGMFUNCTION_H
#define PGMFUNCTION_H

#include "pgmobject.h"
#include "pgmfunctionlist.h"

#define PGM_TYPE_FUNCTION                    (pgm_function_get_type())
#define PGM_FUNCTION(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_FUNCTION, PgmFunction ))
#define PGM_IS_FUNCTION(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_FUNCTION) )
#define PGM_FUNCTION_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_FUNCTION, PgmFunctionClass ))
#define PGM_IS_FUNCTION_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_FUNCTION ))
#define PGM_FUNCTION_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_FUNCTION, PgmFunctionClass ))

typedef struct _PgmFunction           PgmFunction;
typedef struct _PgmFunctionClass      PgmFunctionClass;

G_BEGIN_DECLS

struct _PgmFunction
{
	PgmObject parent;

	/*< private >*/
	gchar *name;
	gchar *args;
	GtkWidget *function_view;
};

struct _PgmFunctionClass
{
	PgmObjectClass parent_class;
};

GType pgm_function_get_type();
PgmFunction* pgm_function_new( PgmFunctionList *list, const gchar *name, const gchar *args );

G_END_DECLS

#endif /* PGMFUNCTION_H */
