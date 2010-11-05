#ifndef PGMLANGUAGELIST_H
#define PGMLANGUAGELIST_H

#include <gtk/gtk.h>
#include "pgmobject.h"

G_BEGIN_DECLS

GtkListStore* pgm_language_list_model_new  ( PgmObject *object );
void          pgm_language_list_model_fill ( GtkListStore *store, PgmObject *object );

G_END_DECLS

#endif /* PGMLANGUAGELIST_H */
