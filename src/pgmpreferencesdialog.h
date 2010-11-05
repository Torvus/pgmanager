#ifndef PGMPREFERENCESDIALOG_H
#define PGMPREFERENCESDIALOG_H

#include <gtk/gtk.h>

#define PGM_REGISTER_PREFERENCE( title, widget_func, load_func, save_func ) \
				static const gchar* widget_func##__get_title() { return title; } \
				static PgmPreference widget_func##__preference = { widget_func##__get_title, widget_func, load_func, save_func }; \
				static void __attribute__ ((constructor)) widget_func##_registered() { pgm_preferences_dialog_register( &widget_func##__preference ); }

G_BEGIN_DECLS

typedef struct _PgmPreference PgmPreference;

struct _PgmPreference
{
	const gchar* (*get_title)   ();
	GtkWidget*   (*get_widget)  ();
	void         (*load_config) ( GKeyFile *file );
	void         (*save_config) ( GKeyFile *file );
};

void pgm_preferences_dialog_show( GtkWindow *parent, GKeyFile *file );
void pgm_preferences_dialog_register( PgmPreference *pref );

G_END_DECLS

#endif /* PGMPREFERENCESDIALOG_H */
