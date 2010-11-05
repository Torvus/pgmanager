#include "pgmstock.h"

#include "pgmtranslate.h"

typedef struct _PgmStockIcon PgmStockIcon;

struct _PgmStockIcon
{
	const gchar *name;
	const gchar *file;
};

static PgmStockIcon stock_icons[] =
{
	{ PGM_STOCK_SERVERS,               "servers.png" },
	{ PGM_STOCK_SERVER,                "server.png" },
	{ PGM_STOCK_DATABASE,              "database.png" },
	{ PGM_STOCK_DATABASE_CLOSED,       "closeddatabase.png" },
	{ PGM_STOCK_FUNCTIONS,             "functions.png" },
	{ PGM_STOCK_FUNCTION,              "function.png" },
	{ PGM_STOCK_TABLES,                "tables.png" },
	{ PGM_STOCK_TABLE,                 "table.png" },
	{ PGM_STOCK_VIEWS,                 "views.png" },
	{ PGM_STOCK_VIEW,                  "view.png" },
	{ PGM_STOCK_SCHEMAS,               "schemas.png" },
	{ PGM_STOCK_SCHEMA,                "schema.png" },
	{ PGM_STOCK_KEY,                   "key.png" },
	{ PGM_STOCK_KEY_PRIMARY,           "primary_key.png" },
	{ PGM_STOCK_KEY_FOREIGN,           "foreign_key.png" },
	{ PGM_STOCK_LANGUAGES,             "languages.png" },
	{ PGM_STOCK_LANGUAGE,              "language.png" },
	{ PGM_STOCK_SQL_EDITOR,            "sql_editor.png" },
	{ PGM_STOCK_EXECUTE,               "execute.png" }
};

static GtkIconFactory *icon_factory;
static GList *pixmap_directories = NULL;

static gchar*
find_pixmap_file( const gchar *filename )
{
	GList *elem;

	elem = pixmap_directories;

	while( elem )
	{
		gchar *pathname = g_strdup_printf( "%s%s%s",
				(gchar *) elem->data,
				G_DIR_SEPARATOR_S,
				filename );
		if( g_file_test( pathname, G_FILE_TEST_EXISTS ) )
			return pathname;
		g_free( pathname );
		elem = elem->next;
	}
	return NULL;
}
/*
static GtkWidget*
create_pixmap (const gchar *filename)
{
	gchar *pathname = NULL;
	GtkWidget *pixmap;

	if (!filename || !filename[0])
		return gtk_image_new();

	pathname = find_pixmap_file (filename);

	if (!pathname)
	{
		g_warning (_("Couldn't find pixmap file: %s"), filename);
		return gtk_image_new();
	}

	pixmap = gtk_image_new_from_file (pathname);
	g_free (pathname);

	return pixmap;
}
*/
static GdkPixbuf*
create_pixbuf (const gchar *filename)
{
	gchar *pathname = NULL;
	GdkPixbuf *pixbuf;
	GError *error = NULL;

	if (!filename || !filename[0])
		return NULL;

	pathname = find_pixmap_file (filename);

	if (!pathname)
	{
		g_warning (_("Couldn't find pixmap file: %s"), filename);
		return NULL;
	}

	pixbuf = gdk_pixbuf_new_from_file (pathname, &error);

	if (!pixbuf)
	{
		g_warning (_("Failed to load pixbuf file: %s: %s"), pathname, error->message);
		g_error_free (error);
	}

	g_free (pathname);

	return pixbuf;
}

static void
factory_add( PgmStockIcon *stock_icons, gint n_elements )
{
	gint i;
	GdkPixbuf *pixbuf;
	GtkIconSet *iconset = NULL;

	g_return_if_fail( stock_icons != NULL );
	g_return_if_fail( n_elements > 0 );

	for( i = 0; i < n_elements; i++ )
	{
		pixbuf = create_pixbuf( stock_icons[i].file );
		if (pixbuf)
		{
			iconset = gtk_icon_set_new_from_pixbuf( pixbuf );

			g_object_unref( G_OBJECT( pixbuf ) );
			pixbuf = NULL;
		}
		else
		{
			g_warning( "can't open %s", stock_icons[i].file );
		}

		if( iconset )
		{
			gtk_icon_factory_add( icon_factory, stock_icons[i].name, iconset );
			gtk_icon_set_unref( iconset );
			iconset = NULL;
		}
	}
}

void pgm_stock_init()
{
	icon_factory = gtk_icon_factory_new();
	gtk_icon_factory_add_default( icon_factory );

	factory_add( stock_icons, G_N_ELEMENTS( stock_icons ) );
}

void pgm_stock_add_pixmap_directory( const gchar *path )
{
	pixmap_directories = g_list_prepend( pixmap_directories, g_strdup( path ) );
}
