#include <gtk/gtk.h>

#include "pgmtranslate.h"
#include "pgmmainwindow.h"
#include "pgmstock.h"

int
main (int argc, char **argv)
{
	GtkWidget *window;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	g_thread_init( NULL );

	gtk_set_locale ();
	gtk_init (&argc, &argv);

	pgm_stock_add_pixmap_directory( PACKAGE_PIXMAPS_DIR );
	pgm_stock_add_pixmap_directory( DEBUG_PACKAGE_PIXMAPS_DIR );
	pgm_stock_init();

	window = pgm_main_window_new();

	g_signal_connect( window, "delete_event", gtk_main_quit, NULL );

	gtk_widget_show( GTK_WIDGET( window ) );

	gtk_main ();

	pgm_main_window_save_config();

	return 0;
}
