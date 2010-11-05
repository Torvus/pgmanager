#include "pgmcellrendererspin.h"

G_DEFINE_TYPE( PgmCellRendererSpin, pgm_cell_renderer_spin, GTK_TYPE_CELL_RENDERER_SPIN )

static void pgm_cell_renderer_spin_init( PgmCellRendererSpin *renderer );
static void pgm_cell_renderer_spin_class_init( PgmCellRendererSpinClass *klass );
static void pgm_cell_renderer_spin_render( GtkCellRenderer          *cell,
										   GdkWindow                *window,
										   GtkWidget                *widget,
										   GdkRectangle             *background_area,
										   GdkRectangle             *cell_area,
										   GdkRectangle             *expose_area,
										   GtkCellRendererState      flags );
GtkCellEditable * pgm_cell_renderer_spin_start_editing( GtkCellRenderer      *cell,
														GdkEvent             *event,
														GtkWidget            *widget,
														const gchar          *path,
														GdkRectangle         *background_area,
														GdkRectangle         *cell_area,
														GtkCellRendererState  flags );


static void
pgm_cell_renderer_spin_init( PgmCellRendererSpin *renderer )
{}

static void
pgm_cell_renderer_spin_class_init( PgmCellRendererSpinClass *klass )
{
	GtkCellRendererClass *gclass = GTK_CELL_RENDERER_CLASS( klass );

	gclass->render = pgm_cell_renderer_spin_render;
	gclass->start_editing = pgm_cell_renderer_spin_start_editing;
}

GtkCellRenderer*
pgm_cell_renderer_spin_new()
{
	GtkCellRenderer *renderer;
	GtkObject *adjust;
	GValue val = {0};

	renderer = g_object_new( PGM_TYPE_CELL_RENDERER_SPIN, NULL );

	adjust = gtk_adjustment_new( 0, -999999, 999999, 1, 10, 0 );

	g_value_init( &val, G_TYPE_OBJECT );
	g_value_set_object( &val, adjust );
	g_object_set_property( G_OBJECT( renderer ), "adjustment", &val );

	g_object_unref( adjust );

	return renderer;
}

static void
pgm_cell_renderer_spin_render( GtkCellRenderer          *cell,
							   GdkWindow                *window,
							   GtkWidget                *widget,
							   GdkRectangle             *background_area,
							   GdkRectangle             *cell_area,
							   GdkRectangle             *expose_area,
							   GtkCellRendererState      flags )
{
	GValue val = {0};
	GValue fg = {0};
	const gchar *text;
	GtkStyle *style;
	gboolean set_text_to_null = FALSE;

	style = gtk_widget_get_style( widget );

	g_value_init( &val, G_TYPE_STRING );
	g_value_init( &fg, GDK_TYPE_COLOR );
	g_object_get_property( G_OBJECT( cell ), "text", &val );
	text = g_value_get_string( &val );

	if( g_strcmp0( text, "#<!-NULL->#" ) == 0 )
	{
		g_value_set_static_string( &val, "null" );
		g_object_set_property( G_OBJECT( cell ), "text", &val );
		g_value_set_boxed( &fg, &style->fg[ GTK_STATE_INSENSITIVE ] );
		g_object_set_property( G_OBJECT( cell ), "foreground-gdk", &fg );
		set_text_to_null = TRUE;
	}
	else
	{
		g_value_set_boxed( &fg, &style->fg[ GTK_STATE_NORMAL ] );
		g_object_set_property( G_OBJECT( cell ), "foreground-gdk", &fg );
	}
	GTK_CELL_RENDERER_CLASS( pgm_cell_renderer_spin_parent_class )->render( cell, window, widget, background_area, cell_area, expose_area, flags );
	if( set_text_to_null )
	{
		g_value_set_static_string( &val, "#<!-NULL->#" );
		g_object_set_property( G_OBJECT( cell ), "text", &val );
	}
}

GtkCellEditable *
pgm_cell_renderer_spin_start_editing( GtkCellRenderer      *cell,
									  GdkEvent             *event,
									  GtkWidget            *widget,
									  const gchar          *path,
									  GdkRectangle         *background_area,
									  GdkRectangle         *cell_area,
									  GtkCellRendererState  flags )
{
	const gchar *text;
	GValue text_val = {0};

	g_value_init( &text_val, G_TYPE_STRING );
	g_object_get_property( G_OBJECT( cell ), "text", &text_val );
	text = g_value_get_string( &text_val );

	if( g_strcmp0( text, "#<!-NULL->#" ) == 0 )
	{
		g_value_set_static_string( &text_val, "" );
		g_object_set_property( G_OBJECT( cell ), "text", &text_val );
	}
	return GTK_CELL_RENDERER_CLASS( pgm_cell_renderer_spin_parent_class )->start_editing( cell, event, widget, path, background_area, cell_area, flags );
}
