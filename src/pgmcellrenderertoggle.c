#include "pgmcellrenderertoggle.h"

#include "pgmcheckbutton.h"

#define PGM_PARAM_READWRITE G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
#define GTK_CELL_RENDERER_TEXT_PATH "gtk-cell-renderer-text-path"
#define BOOL_OUT(val) (val ? "true" : "false")

G_DEFINE_TYPE( PgmCellRendererToggle, pgm_cell_renderer_toggle, PGM_TYPE_CELL_RENDERER_TEXT )

static void pgm_cell_renderer_toggle_init( PgmCellRendererToggle *renderer );
static void pgm_cell_renderer_toggle_class_init( PgmCellRendererToggleClass *klass );
static void pgm_cell_renderer_toggle_render( GtkCellRenderer          *cell,
										     GdkWindow                *window,
										     GtkWidget                *widget,
										     GdkRectangle             *background_area,
										     GdkRectangle             *cell_area,
										     GdkRectangle             *expose_area,
										     GtkCellRendererState      flags );
GtkCellEditable * pgm_cell_renderer_toggle_start_editing( GtkCellRenderer      *cell,
														  GdkEvent             *event,
														  GtkWidget            *widget,
														  const gchar          *path,
														  GdkRectangle         *background_area,
														  GdkRectangle         *cell_area,
														  GtkCellRendererState  flags );
static gboolean pgm_cell_renderer_toggle_focus_out_event( GtkWidget *editor,
														  GdkEvent  *event,
														  gpointer   data );
static void pgm_cell_renderer_toggle_editing_done( GtkCellEditable *button,
				                                   gpointer         data );
static void pgm_cell_renderer_toggle_editor_toggled( GtkToggleButton *button,
                                                     gpointer         data );

enum {
	PROP_0,
	PROP_TEXT,
	PROP_EDITABLE
};

static void
pgm_cell_renderer_toggle_init( PgmCellRendererToggle *renderer )
{
	renderer->editor = NULL,
	renderer->in_editing = FALSE;
}

static void
pgm_cell_renderer_toggle_class_init( PgmCellRendererToggleClass *klass )
{
	GtkCellRendererClass *gcrt_class = GTK_CELL_RENDERER_CLASS( klass );

	gcrt_class->render = pgm_cell_renderer_toggle_render;
	gcrt_class->start_editing = pgm_cell_renderer_toggle_start_editing;
	/*GObjectClass *object_class = G_OBJECT_CLASS( klass );

	object_class->set_property = pgm_cell_renderer_toggle_set_property;
	object_class->get_property = pgm_cell_renderer_toggle_get_property;

	g_object_class_install_property( object_class,
                                     PROP_TEXT,
                                     g_param_spec_string( "text",
                                                          "Text",
                                                          "Text to render",
                                                          NULL,
                                                          PGM_PARAM_READWRITE ) );
	g_object_class_install_property( object_class,
                                     PROP_EDITABLE,
                                     g_param_spec_boolean( "editable",
                                                          "Editable",
                                                          "Whether the text can be modified by the user",
                                                          FALSE,
                                                          PGM_PARAM_READWRITE ) );*/
}
/*
static void
pgm_cell_renderer_toggle_set_property( GObject                  *object,
									   guint                     param_id,
									   const GValue             *value,
									   GParamSpec               *pspec )
{
	PgmCellRendererToggle *renderer = PGM_CELL_RENDERER_TOGGLE( object );

	switch( param_id )
	{
		case PROP_TEXT:
		{
			GValue inconsistent_val = {0};
			GValue activate_val = {0};
			g_value_init( &inconsistent_val, G_TYPE_BOOLEAN );
			g_value_init( &activate_val, G_TYPE_BOOLEAN );

			g_free( renderer->text );
			renderer->text = g_value_dup_string( value );
			if( g_strcmp0( renderer->text, "#<!-NULL->#" ) == 0 )
				g_value_set_boolean( &inconsistent_val, TRUE );
			else
			{
				g_value_set_boolean( &inconsistent_val, FALSE );
				g_value_set_boolean( &activate_val, g_ascii_tolower( renderer->text[ 0 ] ) == 't' );
				g_object_set_property( object, "active", &activate_val );
			}
			g_object_set_property( object, "inconsistent", &inconsistent_val );
			break;
		}
		case PROP_EDITABLE:
		{
			gtk_cell_renderer_toggle_set_activatable( GTK_CELL_RENDERER_TOGGLE( object ), g_value_get_boolean( value ) );
			break;
		}
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
			break;
	}
}

static void
pgm_cell_renderer_toggle_get_property( GObject                  *object,
									   guint                     param_id,
									   GValue                   *value,
									   GParamSpec               *pspec )
{
	PgmCellRendererToggle *renderer = PGM_CELL_RENDERER_TOGGLE( object );

	switch( param_id )
	{
		case PROP_TEXT:
			g_value_set_string( value, renderer->text );
			break;

		case PROP_EDITABLE:
			g_object_get_property( object, "activatable", value );
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
			break;
	}
}
*/
GtkCellRenderer*
pgm_cell_renderer_toggle_new()
{
	return g_object_new( PGM_TYPE_CELL_RENDERER_TOGGLE, NULL );
}

static void
pgm_cell_renderer_toggle_render( GtkCellRenderer          *cell,
							     GdkWindow                *window,
							     GtkWidget                *widget,
							     GdkRectangle             *background_area,
							     GdkRectangle             *cell_area,
							     GdkRectangle             *expose_area,
							     GtkCellRendererState      flags )
{
	/*PgmCellRendererToggle *cell_toggle = PGM_CELL_RENDERER_TOGGLE( cell );
	if( !cell_toggle->in_editing && g_object_get_data( G_OBJECT( cell_toggle ), "cell_area" ) != cell_area )*/
		GTK_CELL_RENDERER_CLASS( pgm_cell_renderer_toggle_parent_class )->render( cell, window, widget, background_area, cell_area, expose_area, flags );
}

GtkCellEditable *
pgm_cell_renderer_toggle_start_editing( GtkCellRenderer      *cell,
									    GdkEvent             *event,
										GtkWidget            *widget,
										const gchar          *path,
										GdkRectangle         *background_area,
										GdkRectangle         *cell_area,
										GtkCellRendererState  flags )
{
	PgmCellRendererToggle *cell_toggle = PGM_CELL_RENDERER_TOGGLE( cell );
	const gchar *text;
	GValue val = {0};

	/*cell_toggle->in_editing = TRUE;
	g_object_set_data( G_OBJECT( cell_toggle ), "cell_area", cell_area );*/
	cell_toggle->editor = pgm_check_button_new_with_label( NULL );

	g_value_init( &val, G_TYPE_STRING );
	g_object_get_property( G_OBJECT( cell_toggle ), "text", &val );
	text = g_value_get_string( &val );

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( cell_toggle->editor ), (text[0] == 't' || text[0] == 'T') );
	gtk_toggle_button_set_inconsistent( GTK_TOGGLE_BUTTON( cell_toggle->editor ), (g_strcmp0( text, "#<!-NULL->#" ) == 0) );

	g_object_set_data_full( G_OBJECT( cell_toggle->editor ), GTK_CELL_RENDERER_TEXT_PATH, g_strdup( path ), g_free );
	g_signal_connect( cell_toggle->editor,
		              "editing-done",
		              G_CALLBACK( pgm_cell_renderer_toggle_editing_done ),
		              cell_toggle );
	cell_toggle->toggled_id = g_signal_connect( cell_toggle->editor,
											    "clicked",
												G_CALLBACK( pgm_cell_renderer_toggle_editor_toggled ),
												cell_toggle );
	cell_toggle->focus_out_id = g_signal_connect_after( cell_toggle->editor, "focus-out-event",
														G_CALLBACK( pgm_cell_renderer_toggle_focus_out_event ),
														cell_toggle );

	gtk_widget_show( cell_toggle->editor );
	return GTK_CELL_EDITABLE( cell_toggle->editor );
}

static gboolean
pgm_cell_renderer_toggle_focus_out_event( GtkWidget *editor,
		                                  GdkEvent  *event,
					                      gpointer   data )
{
	g_debug( "%s()", G_STRFUNC );

	g_object_set( editor,
				  "editing-canceled", TRUE,
                  NULL );
	gtk_cell_editable_editing_done( GTK_CELL_EDITABLE( editor ) );
	gtk_cell_editable_remove_widget( GTK_CELL_EDITABLE( editor ) );

	return FALSE;
}

static void
pgm_cell_renderer_toggle_editing_done( GtkCellEditable *button,
				                       gpointer         data )
{
	const gchar *path;
	const gchar *new_text;
	gboolean canceled;
	PgmCellRendererToggle *cell_toggle = PGM_CELL_RENDERER_TOGGLE( data );

	cell_toggle->editor = NULL;

	if( cell_toggle->focus_out_id > 0 )
    {
		g_signal_handler_disconnect( button, cell_toggle->focus_out_id );
		cell_toggle->focus_out_id = 0;
    }
	if( cell_toggle->toggled_id > 0 )
    {
		g_signal_handler_disconnect( button, cell_toggle->toggled_id );
		cell_toggle->toggled_id = 0;
    }
	g_object_get( button,
                  "editing-canceled", &canceled,
				  NULL );
    gtk_cell_renderer_stop_editing( GTK_CELL_RENDERER( data ), canceled );

	if( canceled )
		return;

	path = g_object_get_data( G_OBJECT( button ), GTK_CELL_RENDERER_TEXT_PATH );
	if( gtk_toggle_button_get_inconsistent( GTK_TOGGLE_BUTTON( button ) ) )
		new_text = "";
	else
		new_text = (gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( button ) ) ? "true" : "false");

	g_signal_emit_by_name( data, "edited", path, new_text );
}

static void
pgm_cell_renderer_toggle_editor_toggled( GtkToggleButton *button,
                                         gpointer         data )
{
	PgmCellRendererToggle *cell_toggle = PGM_CELL_RENDERER_TOGGLE( data );

	if( gtk_toggle_button_get_inconsistent( button ) )
	{
		gtk_toggle_button_set_inconsistent( button, FALSE );

		g_signal_handler_disconnect( button, cell_toggle->toggled_id );
		gtk_toggle_button_set_active( button, !gtk_toggle_button_get_active( button ) );
		cell_toggle->toggled_id = g_signal_connect( cell_toggle->editor,
													"clicked",
													G_CALLBACK( pgm_cell_renderer_toggle_editor_toggled ),
													cell_toggle );
	}
	else if( gtk_toggle_button_get_active( button ) )
	{
		gtk_toggle_button_set_inconsistent( button, TRUE );
	}
}
