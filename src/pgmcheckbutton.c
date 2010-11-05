#include "pgmcheckbutton.h"

#include <gdk/gdkkeysyms.h>

static void pgm_check_button_cell_editable_init   ( GtkCellEditableIface *iface );
static void pgm_check_button_init                 ( PgmCheckButton       *button );
static void pgm_check_button_class_init           ( PgmCheckButtonClass  *klass );
static void pgm_check_button_start_editing        ( GtkCellEditable      *cell_editable,
										            GdkEvent             *event );
static gboolean pgm_cell_editable_key_press_event ( PgmCheckButton       *button,
                                                    GdkEventKey          *key_event,
                                                    gpointer              data );
static void pgm_check_button_set_property( GObject                  *object,
										   guint                     param_id,
									       const GValue             *value,
									       GParamSpec               *pspec );
static void pgm_check_button_get_property( GObject                  *object,
									       guint                     param_id,
									       GValue                   *value,
									       GParamSpec               *pspec );

G_DEFINE_TYPE_WITH_CODE( PgmCheckButton, pgm_check_button, GTK_TYPE_CHECK_BUTTON,
                         G_IMPLEMENT_INTERFACE( GTK_TYPE_CELL_EDITABLE,
                                 pgm_check_button_cell_editable_init ) )

enum
{
	PROP_0,
	PROP_EDITING_CANCELED
};

static void
pgm_check_button_cell_editable_init( GtkCellEditableIface *iface )
{
	iface->start_editing = pgm_check_button_start_editing;
}

static void
pgm_check_button_init( PgmCheckButton *button )
{
	button->editing_cancel = FALSE;
}

static void
pgm_check_button_class_init( PgmCheckButtonClass *klass )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( klass );

	gobject_class->set_property = pgm_check_button_set_property;
	gobject_class->get_property = pgm_check_button_get_property;

	g_object_class_override_property( gobject_class,
                                      PROP_EDITING_CANCELED,
                                      "editing-canceled" );
}

static void
pgm_check_button_set_property( GObject                  *object,
							   guint                     param_id,
						       const GValue             *value,
						       GParamSpec               *pspec )
{
	PgmCheckButton *button = PGM_CHECK_BUTTON( object );

	switch( param_id )
	{
		case PROP_EDITING_CANCELED:
			button->editing_cancel = g_value_get_boolean( value );
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID( object, param_id, pspec );
			break;
	}
}

static void
pgm_check_button_get_property( GObject                  *object,
						       guint                     param_id,
						       GValue                   *value,
						       GParamSpec               *pspec )
{
	PgmCheckButton *button = PGM_CHECK_BUTTON( object );

	switch( param_id )
	{
		case PROP_EDITING_CANCELED:
			g_value_set_boolean( value, button->editing_cancel );
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID( object, param_id, pspec );
			break;
	}
}

static void
pgm_check_button_start_editing( GtkCellEditable *cell_editable,
                                GdkEvent        *event )
{
	g_signal_connect( cell_editable, "key-press-event",
	                  G_CALLBACK( pgm_cell_editable_key_press_event ), NULL );
}

static gboolean
pgm_cell_editable_key_press_event( PgmCheckButton    *button,
                                   GdkEventKey       *key_event,
                                   gpointer          data )
{
	if( key_event->keyval == GDK_Escape )
	{
		button->editing_cancel = TRUE;
		gtk_cell_editable_editing_done( GTK_CELL_EDITABLE( button ) );
		gtk_cell_editable_remove_widget( GTK_CELL_EDITABLE( button ) );

		return TRUE;
	}

	/* override focus */
	if( key_event->keyval == GDK_Up || key_event->keyval == GDK_Down )
	{
		gtk_cell_editable_editing_done( GTK_CELL_EDITABLE( button ) );
		gtk_cell_editable_remove_widget( GTK_CELL_EDITABLE( button ) );

		return TRUE;
	}

	return FALSE;
}

GtkWidget *
pgm_check_button_new()
{
	return g_object_new( PGM_TYPE_CHECK_BUTTON, NULL );
}

GtkWidget *
pgm_check_button_new_with_label( const gchar *label )
{
	return g_object_new( PGM_TYPE_CHECK_BUTTON, "label", label, NULL );
}
