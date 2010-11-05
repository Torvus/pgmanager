#ifndef PGMCHECKBUTTON_H
#define PGMCHECKBUTTON_H

#include <gtk/gtk.h>

#define PGM_TYPE_CHECK_BUTTON                    (pgm_check_button_get_type())
#define PGM_CHECK_BUTTON(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_CHECK_BUTTON, PgmCheckButton ))
#define PGM_IS_CHECK_BUTTON(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_CHECK_BUTTON) )
#define PGM_CHECK_BUTTON_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_CHECK_BUTTON, PgmCheckButtonClass ))
#define PGM_IS_CHECK_BUTTON_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_CHECK_BUTTON ))
#define PGM_CHECK_BUTTON_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_CHECK_BUTTON, PgmCheckButtonClass ))

typedef struct _PgmCheckButton           PgmCheckButton;
typedef struct _PgmCheckButtonClass      PgmCheckButtonClass;

G_BEGIN_DECLS

struct _PgmCheckButton
{
	GtkCheckButton parent;

	/*< private >*/
	gboolean editing_cancel;
};

struct _PgmCheckButtonClass
{
	GtkCheckButtonClass parent_class;
};

GType pgm_check_button_get_type();
GtkWidget * pgm_check_button_new();
GtkWidget * pgm_check_button_new_with_label( const gchar *label );

G_END_DECLS

#endif /* PGMCHECKBUTTON_H */
