#ifndef PGMFUNCTIONVIEW_H
#define PGMFUNCTIONVIEW_H

#include <gtk/gtk.h>
#include "pgmfunction.h"

#define PGM_TYPE_FUNCTION_VIEW                    (pgm_function_view_get_type())
#define PGM_FUNCTION_VIEW(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_FUNCTION_VIEW, PgmFunctionView ))
#define PGM_IS_FUNCTION_VIEW(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_FUNCTION_VIEW) )
#define PGM_FUNCTION_VIEW_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_FUNCTION_VIEW, PgmFunctionViewClass ))
#define PGM_IS_FUNCTION_VIEW_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_FUNCTION_VIEW ))
#define PGM_FUNCTION_VIEW_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_FUNCTION_VIEW, PgmFunctionViewClass ))

G_BEGIN_DECLS

typedef struct _PgmFunctionView PgmFunctionView;
typedef struct _PgmFunctionViewClass PgmFunctionViewClass;

struct _PgmFunctionView
{
	GtkNotebook parent;

	/*< private >*/
	PgmFunction *function;
	GtkUIManager *ui_manager;
	GtkActionGroup *actions;
	GtkWidget *source_view_widget;
	GtkWidget *source_tab_bar;
	GtkWidget *source_editor_scrolled;
	GtkWidget *source_editor;
	GtkWidget *source_options_widget;
	GtkWidget *source_schema_edit;
	GtkWidget *source_name_edit;
	GtkWidget *source_ret_setof;
	GtkWidget *source_ret_type;
	GtkWidget *source_lang;
	GtkWidget *source_args;
	/*GtkWidget *source_ret_non;
	GtkWidget *source_ex_w_def_priv;*/
	GtkWidget *source_opt_frame;
	GtkWidget *source_immutable;
	GtkWidget *source_stable;
	GtkWidget *source_volatile;
	GtkTreeModel *type_model;
	GtkWidget *description_widget;
};

struct _PgmFunctionViewClass
{
	GtkNotebookClass parent_class;
};

GType pgm_function_view_get_type();
GtkWidget* pgm_function_view_new( PgmFunction *function );
void pgm_function_view_set_source( PgmFunctionView *view, const gchar *source );

G_END_DECLS

#endif /* PGMFUNCTIONVIEW_H */
