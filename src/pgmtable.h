#ifndef PGMTABLE_H
#define PGMTABLE_H

#include "pgmobject.h"
#include "pgmtablelist.h"

#define PGM_TYPE_TABLE                    (pgm_table_get_type())
#define PGM_TABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_TABLE, PgmTable ))
#define PGM_IS_TABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_TABLE) )
#define PGM_TABLE_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_TABLE, PgmTableClass ))
#define PGM_IS_TABLE_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_TABLE ))
#define PGM_TABLE_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_TABLE, PgmTableClass ))

G_BEGIN_DECLS

typedef struct _PgmTable PgmTable;
typedef struct _PgmTableClass PgmTableClass;

struct _PgmTable
{
	PgmObject parent_instance;

	/*< private >*/
	GtkWidget *widget;
};

struct _PgmTableClass
{
	PgmObjectClass parent_instance;
};

GType pgm_table_get_type();
PgmTable* pgm_table_new( PgmTableList *list, const gchar *name );

G_END_DECLS

#endif /* PGMTABLE_H */
