#ifndef PGMSCHEMALISTMODEL_H
#define PGMSCHEMALISTMODEL_H

#include <gtk/gtk.h>
#include "pgmobject.h"

G_BEGIN_DECLS

GtkListStore* pgm_schema_list_model_new( PgmObject *object );
void pgm_schema_list_model_fill( GtkListStore *store, PgmObject *object );

G_END_DECLS

#endif /* PGMSCHEMALISTMODEL_H */
