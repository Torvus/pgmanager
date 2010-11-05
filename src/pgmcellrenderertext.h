#ifndef PGMCELLRENDERERTEXT_H
#define PGMCELLRENDERERTEXT_H

#include <gtk/gtk.h>

#define PGM_TYPE_CELL_RENDERER_TEXT                    (pgm_cell_renderer_text_get_type())
#define PGM_CELL_RENDERER_TEXT(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_CELL_RENDERER_TEXT, PgmCellRendererText ))
#define PGM_IS_CELL_RENDERER_TEXT(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_CELL_RENDERER_TEXT) )
#define PGM_CELL_RENDERER_TEXT_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_CELL_RENDERER_TEXT, PgmCellRendererTextClass ))
#define PGM_IS_CELL_RENDERER_TEXT_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_CELL_RENDERER_TEXT ))
#define PGM_CELL_RENDERER_TEXT_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_CELL_RENDERER_TEXT, PgmCellRendererTextClass ))

G_BEGIN_DECLS

typedef struct _PgmCellRendererText PgmCellRendererText;
typedef struct _PgmCellRendererTextClass PgmCellRendererTextClass;

struct _PgmCellRendererText
{
	GtkCellRendererText parent_instance;
};

struct _PgmCellRendererTextClass
{
	GtkCellRendererTextClass parent_class;
};

GType pgm_cell_renderer_text_get_type();
GtkCellRenderer* pgm_cell_renderer_text_new();

G_END_DECLS

#endif /* PGMCELLRENDERERTEXT_H */
