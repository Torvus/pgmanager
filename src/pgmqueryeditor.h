#ifndef PGMQUERYEDITOR_H
#define PGMQUERYEDITOR_H

#include "pgmsqleditor.h"

#include "pgmquery.h"
#include "pgmserverlist.h"

#define PGM_TYPE_QUERY_EDITOR                    (pgm_query_editor_get_type())
#define PGM_QUERY_EDITOR(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_QUERY_EDITOR, PgmQueryEditor ))
#define PGM_IS_QUERY_EDITOR(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_QUERY_EDITOR) )
#define PGM_QUERY_EDITOR_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_QUERY_EDITOR, PgmQueryEditorClass ))
#define PGM_IS_QUERY_EDITOR_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_QUERY_EDITOR ))
#define PGM_QUERY_EDITOR_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_QUERY_EDITOR, PgmQueryEditorClass ))

G_BEGIN_DECLS

typedef struct _PgmQueryEditor           PgmQueryEditor;
typedef struct _PgmQueryEditorClass      PgmQueryEditorClass;
typedef struct _PgmQueryEditorPrivate    PgmQueryEditorPrivate;

struct _PgmQueryEditor
{
	GtkVBox parent;

	/*< private >*/
	PgmServerList *server_list;
	GtkUIManager *ui_manager;
	GtkActionGroup *actions;
	GtkWidget *tool_bar;
	GtkToolItem *db_combo_box;
	GtkWidget *paned;
	GtkWidget *source_view;
	GtkWidget *result_view;
	GtkTreeModel *result_store;
	GtkTreeModel *db_list_model;
};

struct _PgmQueryEditorClass
{
	GtkVBoxClass parent_class;
};

GType pgm_query_editor_get_type();
GtkWidget* pgm_query_editor_new( PgmServerList *list, PgmObject *current_database );

G_END_DECLS

#endif /* PGMQUERYEDITOR_H */
