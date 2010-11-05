#ifndef PGMDATASTORE_H
#define PGMDATASTORE_H

#include <gtk/gtk.h>
#include "pgmquery.h"
#include "pgmdatabase.h"

#define PGM_TYPE_DATA_STORE            (pgm_data_store_get_type ())
#define PGM_DATA_STORE(obj)	           (G_TYPE_CHECK_INSTANCE_CAST ((obj), PGM_TYPE_DATA_STORE, PgmDataStore))
#define PGM_DATA_STORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PGM_TYPE_DATA_STORE, PgmDataStoreClass))
#define PGM_IS_DATA_STORE(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PGM_TYPE_DATA_STORE))
#define PGM_IS_DATA_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PGM_TYPE_DATA_STORE))
#define PGM_DATA_STORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PGM_TYPE_DATA_STORE, PgmDataStoreClass))

G_BEGIN_DECLS

typedef struct _PgmDataStore       PgmDataStore;
typedef struct _PgmDataStoreClass  PgmDataStoreClass;

struct _PgmDataStore
{
	GObject parent;

	/*< private >*/
	gint stamp;
	gint n_rows;
	gint sort_column_id;
	GtkSortType order;
	PgmQuery *query;
	gchar *query_str;
};

struct _PgmDataStoreClass
{
	GObjectClass parent_class;
};

GType pgm_data_store_get_type();
/*PgmDataStore* pgm_data_store_new( PgmQuery *query );*/
PgmDataStore* pgm_data_store_new( PgmDatabase *db, const gchar *query_str );
void pgm_data_store_set_database( PgmDataStore *store, PgmDatabase *db );
void pgm_data_store_set_query( PgmDataStore *store, const gchar *query_str );
gchar* pgm_data_store_get_column_title( PgmDataStore *store, gint column );
void pgm_data_store_row_changed( PgmDataStore *store, gint row );
void pgm_data_store_update( PgmDataStore *store );

G_END_DECLS

#endif /* PGMDATASTORE_H */
