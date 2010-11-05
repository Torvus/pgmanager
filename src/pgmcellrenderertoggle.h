#ifndef PGMCELLRENDERERTOGGLE_H
#define PGMCELLRENDERERTOGGLE_H

#include <gtk/gtk.h>
#include "pgmcellrenderertext.h"

#define PGM_TYPE_CELL_RENDERER_TOGGLE                    (pgm_cell_renderer_toggle_get_type())
#define PGM_CELL_RENDERER_TOGGLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_CELL_RENDERER_TOGGLE, PgmCellRendererToggle ))
#define PGM_IS_CELL_RENDERER_TOGGLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_CELL_RENDERER_TOGGLE) )
#define PGM_CELL_RENDERER_TOGGLE_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_CELL_RENDERER_TOGGLE, PgmCellRendererToggleClass ))
#define PGM_IS_CELL_RENDERER_TOGGLE_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_CELL_RENDERER_TOGGLE ))
#define PGM_CELL_RENDERER_TOGGLE_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_CELL_RENDERER_TOGGLE, PgmCellRendererToggleClass ))

G_BEGIN_DECLS

typedef struct _PgmCellRendererToggle PgmCellRendererToggle;
typedef struct _PgmCellRendererToggleClass PgmCellRendererToggleClass;

struct _PgmCellRendererToggle
{
	PgmCellRendererText parent_instance;

	gchar *GSEAL( text );
	GtkWidget *editor;
	gulong focus_out_id;
	gulong toggled_id;
	gboolean in_editing;
};

struct _PgmCellRendererToggleClass
{
	PgmCellRendererTextClass parent_class;
};

GType pgm_cell_renderer_toggle_get_type();
GtkCellRenderer* pgm_cell_renderer_toggle_new();

G_END_DECLS

#endif /* PGMCELLRENDERERTOGGLE_H */
