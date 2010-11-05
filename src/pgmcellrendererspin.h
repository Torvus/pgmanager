#ifndef PGMCELLRENDERERSPIN_H
#define PGMCELLRENDERERSPIN_H

#include <gtk/gtk.h>

#define PGM_TYPE_CELL_RENDERER_SPIN                    (pgm_cell_renderer_spin_get_type())
#define PGM_CELL_RENDERER_SPIN(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_CELL_RENDERER_SPIN, PgmCellRendererSpin ))
#define PGM_IS_CELL_RENDERER_SPIN(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_CELL_RENDERER_SPIN) )
#define PGM_CELL_RENDERER_SPIN_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_CELL_RENDERER_SPIN, PgmCellRendererSpinClass ))
#define PGM_IS_CELL_RENDERER_SPIN_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_CELL_RENDERER_SPIN ))
#define PGM_CELL_RENDERER_SPIN_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_CELL_RENDERER_SPIN, PgmCellRendererSpinClass ))

G_BEGIN_DECLS

typedef struct _PgmCellRendererSpin PgmCellRendererSpin;
typedef struct _PgmCellRendererSpinClass PgmCellRendererSpinClass;

struct _PgmCellRendererSpin
{
	GtkCellRendererSpin parent_instance;
};

struct _PgmCellRendererSpinClass
{
	GtkCellRendererSpinClass parent_class;
};

GType pgm_cell_renderer_spin_get_type();
GtkCellRenderer* pgm_cell_renderer_spin_new();

G_END_DECLS

#endif /* PGMCELLRENDERERSPIN_H */
