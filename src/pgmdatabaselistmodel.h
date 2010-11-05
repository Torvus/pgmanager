#ifndef PGMDATABASELISTMODEL_H
#define PGMDATABASELISTMODEL_H

#include <gtk/gtk.h>

#include "pgmserverlist.h"

#define PGM_TYPE_DATABASE_LIST_MODEL                    (pgm_database_list_model_get_type())
#define PGM_DATABASE_LIST_MODEL(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_DATABASE_LIST_MODEL, PgmDatabaseListModel ))
#define PGM_IS_DATABASE_LIST_MODEL(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_DATABASE_LIST_MODEL) )
#define PGM_DATABASE_LIST_MODEL_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_DATABASE_LIST_MODEL, PgmDatabaseListModelClass ))
#define PGM_IS_DATABASE_LIST_MODEL_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_DATABASE_LIST_MODEL ))
#define PGM_DATABASE_LIST_MODEL_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_DATABASE_LIST_MODEL, PgmDatabaseListModelClass ))

G_BEGIN_DECLS

typedef struct _PgmDatabaseListModel           PgmDatabaseListModel;
typedef struct _PgmDatabaseListModelClass      PgmDatabaseListModelClass;

struct _PgmDatabaseListModel
{
	GObject parent_instance;

	/*< private >*/
	gint stamp;
	GList *list;
};

struct _PgmDatabaseListModelClass
{
	GObjectClass parent_class;
};

GType pgm_database_list_model_get_type();
PgmDatabaseListModel* pgm_database_list_model_new( PgmServerList *list );
PgmObject* pgm_database_list_model_get_database( PgmDatabaseListModel *model, GtkTreeIter *iter );
gboolean pgm_database_list_model_get_database_iter( PgmDatabaseListModel *model, PgmObject *database, GtkTreeIter *iter );

G_END_DECLS

#endif /* PGMDATABASELISTMODEL_H */
