#include "pgmtoolcombobox.h"

G_DEFINE_TYPE( PgmToolComboBox, pgm_tool_combo_box, GTK_TYPE_TOOL_ITEM )

static void pgm_tool_combo_box_init( PgmToolComboBox *item );
static void pgm_tool_combo_box_class_init( PgmToolComboBoxClass *klass );
static void pgm_tool_combo_box_dispose( GObject *gobject );

static void
pgm_tool_combo_box_init( PgmToolComboBox *item )
{}

static void
pgm_tool_combo_box_class_init( PgmToolComboBoxClass *klass )
{
	GObjectClass *gclass = G_OBJECT_CLASS( klass );

	gclass->dispose = pgm_tool_combo_box_dispose;
}

static void
pgm_tool_combo_box_dispose( GObject *gobject )
{
	PgmToolComboBox *item = PGM_TOOL_COMBO_BOX( gobject );

	g_object_unref( item->combo_box );

	G_OBJECT_CLASS( pgm_tool_combo_box_parent_class )->dispose( gobject );
}

GtkToolItem*
pgm_tool_combo_box_new()
{
	PgmToolComboBox *item;

	item = g_object_new( PGM_TYPE_TOOL_COMBO_BOX, NULL );

	item->combo_box = gtk_combo_box_entry_new();
	gtk_container_add( GTK_CONTAINER( item ), item->combo_box );

	return GTK_TOOL_ITEM( item );
}

GtkToolItem*
pgm_tool_combo_box_new_with_model( GtkTreeModel *model, gint text_column )
{
	PgmToolComboBox *item;
	GtkWidget *entry;

	item = g_object_new( PGM_TYPE_TOOL_COMBO_BOX, NULL );

	item->combo_box = gtk_combo_box_entry_new_with_model( model, text_column );
	entry = gtk_bin_get_child( GTK_BIN( item->combo_box ) );
	gtk_editable_set_editable( GTK_EDITABLE( entry ), FALSE );
	gtk_container_add( GTK_CONTAINER( item ), item->combo_box );

	return GTK_TOOL_ITEM( item );
}

GtkToolItem*
pgm_tool_combo_box_new_text()
{
	PgmToolComboBox *item;

	item = g_object_new( PGM_TYPE_TOOL_COMBO_BOX, NULL );

	item->combo_box = gtk_combo_box_new_text();
	gtk_container_add( GTK_CONTAINER( item ), item->combo_box );

	return GTK_TOOL_ITEM( item );
}

GtkWidget*
pgm_tool_combo_box_get_combo_box( PgmToolComboBox *item )
{
	g_return_val_if_fail( item != NULL, NULL );

	return item->combo_box;
}

void
pgm_tool_combo_box_set_active( PgmToolComboBox *item, GtkTreeIter *iter )
{
	g_return_if_fail( item != NULL );

	gtk_combo_box_set_active_iter( GTK_COMBO_BOX( item->combo_box ), iter );
}
