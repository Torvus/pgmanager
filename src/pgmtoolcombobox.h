#ifndef PGMTOOLCOMBOBOX_H
#define PGMTOOLCOMBOBOX_H

#include <gtk/gtk.h>

#define PGM_TYPE_TOOL_COMBO_BOX                    (pgm_tool_combo_box_get_type())
#define PGM_TOOL_COMBO_BOX(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_TOOL_COMBO_BOX, PgmToolComboBox ))
#define PGM_IS_TOOL_COMBO_BOX(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_TOOL_COMBO_BOX) )
#define PGM_TOOL_COMBO_BOX_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_TOOL_COMBO_BOX, PgmToolComboBoxClass ))
#define PGM_IS_TOOL_COMBO_BOX_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_TOOL_COMBO_BOX ))
#define PGM_TOOL_COMBO_BOX_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_TOOL_COMBO_BOX, PgmToolComboBoxClass ))

G_BEGIN_DECLS

typedef struct _PgmToolComboBox           PgmToolComboBox;
typedef struct _PgmToolComboBoxClass      PgmToolComboBoxClass;

struct _PgmToolComboBox
{
	GtkToolItem parent_instance;

	/*< private >*/
	GtkWidget *combo_box;
};

struct _PgmToolComboBoxClass
{
	GtkToolItemClass parent_class;
};

GType pgm_tool_combo_box_get_type();
GtkToolItem* pgm_tool_combo_box_new();
GtkToolItem* pgm_tool_combo_box_new_with_model( GtkTreeModel *model, gint text_column );
GtkToolItem* pgm_tool_combo_box_new_text();
GtkWidget* pgm_tool_combo_box_get_combo_box( PgmToolComboBox *item );
void pgm_tool_combo_box_set_active( PgmToolComboBox *item, GtkTreeIter *iter );

G_END_DECLS

#endif /* PGMTOOLCOMBOBOX_H */
