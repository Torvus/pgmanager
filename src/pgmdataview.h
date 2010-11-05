#ifndef PGMDATAVIEW_H
#define PGMDATAVIEW_H

#include <gtk/gtk.h>
#include "pgmdatastore.h"

#define PGM_TYPE_DATA_VIEW             (pgm_data_view_get_type ())
#define PGM_DATA_VIEW(obj)	           (G_TYPE_CHECK_INSTANCE_CAST ((obj), PGM_TYPE_DATA_VIEW, PgmDataView))
#define PGM_DATA_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), PGM_TYPE_DATA_VIEW, PgmDataViewClass))
#define PGM_IS_DATA_VIEW(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PGM_TYPE_DATA_VIEW))
#define PGM_IS_DATA_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), PGM_TYPE_DATA_VIEW))
#define PGM_DATA_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), PGM_TYPE_DATA_VIEW, PgmDataViewClass))

G_BEGIN_DECLS

typedef struct _PgmDataView       PgmDataView;
typedef struct _PgmDataViewClass  PgmDataViewClass;

struct _PgmDataView
{
	GtkTreeView parent_instance;

	/*< private >*/
	gboolean editable;
	GList *list;
};

struct _PgmDataViewClass
{
	GtkTreeViewClass parent_class;

	/*< signals >*/
	gboolean (*edited) ( PgmDataView *view, gint row, gint column, gchar *new_text );
};

GType      pgm_data_view_get_type       ();
GtkWidget* pgm_data_view_new            ();
GtkWidget* pgm_data_view_new_with_store ( PgmDataStore *store );
void       pgm_data_view_set_store      ( PgmDataView *view, PgmDataStore *store );
void       pgm_data_view_set_editable   ( PgmDataView *view, gboolean editable );
gboolean   pgm_data_view_get_editable   ( PgmDataView *view );
gboolean   pgm_data_view_edited         ( PgmDataView *view, gint row, gint column, gchar *new_text );

G_END_DECLS

#endif /* PGMDATAVIEW_H */
