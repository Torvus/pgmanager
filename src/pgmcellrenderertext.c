#include "pgmcellrenderertext.h"

#include <glib/gprintf.h>

G_DEFINE_TYPE( PgmCellRendererText, pgm_cell_renderer_text, GTK_TYPE_CELL_RENDERER_TEXT )

static void pgm_cell_renderer_text_init( PgmCellRendererText *renderer );
static void pgm_cell_renderer_text_class_init( PgmCellRendererTextClass *klass );
static void pgm_cell_renderer_text_render( GtkCellRenderer          *cell,
										   GdkWindow                *window,
										   GtkWidget                *widget,
										   GdkRectangle             *background_area,
										   GdkRectangle             *cell_area,
										   GdkRectangle             *expose_area,
										   GtkCellRendererState      flags );
GtkCellEditable * pgm_cell_renderer_text_start_editing( GtkCellRenderer      *cell,
														GdkEvent             *event,
														GtkWidget            *widget,
														const gchar          *path,
														GdkRectangle         *background_area,
														GdkRectangle         *cell_area,
														GtkCellRendererState  flags );

static void
pgm_cell_renderer_text_init( PgmCellRendererText *renderer )
{}

static void
pgm_cell_renderer_text_class_init( PgmCellRendererTextClass *klass )
{
	GtkCellRendererClass *gcrt_class = GTK_CELL_RENDERER_CLASS( klass );

	gcrt_class->render = pgm_cell_renderer_text_render;
	gcrt_class->start_editing = pgm_cell_renderer_text_start_editing;
}

static void
pgm_cell_renderer_text_render( GtkCellRenderer          *cell,
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
	gboolean set_text_to_null = FALSE;
	GtkStyle *style;

	g_value_init( &val, G_TYPE_STRING );
	g_value_init( &fg, GDK_TYPE_COLOR );
	g_object_get_property( G_OBJECT( cell ), "text", &val );
	text = g_value_get_string( &val );
	style = gtk_widget_get_style( widget );

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
	GTK_CELL_RENDERER_CLASS( pgm_cell_renderer_text_parent_class )->render( cell, window, widget, background_area, cell_area, expose_area, flags );
	if( set_text_to_null )
	{
		g_value_set_static_string( &val, "#<!-NULL->#" );
		g_object_set_property( G_OBJECT( cell ), "text", &val );
	}
}

GtkCellEditable *
pgm_cell_renderer_text_start_editing( GtkCellRenderer      *cell,
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
	return GTK_CELL_RENDERER_CLASS( pgm_cell_renderer_text_parent_class )->start_editing( cell, event, widget, path, background_area, cell_area, flags );
}

GtkCellRenderer*
pgm_cell_renderer_text_new()
{
	return g_object_new( PGM_TYPE_CELL_RENDERER_TEXT, NULL );
}
