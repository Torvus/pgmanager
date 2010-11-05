#ifndef PGMTABLEVIEW_H
#define PGMTABLEVIEW_H

#include <gtk/gtk.h>
#include "pgmtable.h"
#include "pgmquery.h"

#define PGM_TYPE_TABLE_VIEW                    (pgm_table_view_get_type())
#define PGM_TABLE_VIEW(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_TABLE_VIEW, PgmTableView ))
#define PGM_IS_TABLE_VIEW(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_TABLE_VIEW) )
#define PGM_TABLE_VIEW_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_TABLE_VIEW, PgmTableViewClass ))
#define PGM_IS_TABLE_VIEW_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_TABLE_VIEW ))
#define PGM_TABLE_VIEW_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_TABLE_VIEW, PgmTableViewClass ))

G_BEGIN_DECLS

typedef struct _PgmTableView           PgmTableView;
typedef struct _PgmTableViewClass      PgmTableViewClass;

struct _PgmTableView
{
	GtkVBox parent;

	/*< private >*/
	GtkUIManager *ui_manager;
	GtkActionGroup *action_group;
	GtkWidget *tool_bar;
	GtkWidget *data_scrolled;
	GtkWidget *data_view;
	GtkTreeModel *data_store;
	/*PgmQuery *query;*/
	gchar *table_name;
	gchar *schema_name;
};

struct _PgmTableViewClass
{
	GtkVBoxClass parent_class;
};

GType pgm_table_view_get_type();
GtkWidget* pgm_table_view_new( PgmTable *table );

G_END_DECLS

#endif /* PGMTABLEVIEW_H */
