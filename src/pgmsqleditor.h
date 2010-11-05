#ifndef PGMSQLEDITOR_H
#define PGMSQLEDITOR_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>

#define PGM_TYPE_SQL_EDITOR                    (pgm_sql_editor_get_type())
#define PGM_SQL_EDITOR(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_SQL_EDITOR, PgmSqlEditor ))
#define PGM_IS_SQL_EDITOR(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_SQL_EDITOR) )
#define PGM_SQL_EDITOR_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_SQL_EDITOR, PgmSqlEditorClass ))
#define PGM_IS_SQL_EDITOR_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_SQL_EDITOR ))
#define PGM_SQL_EDITOR_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_SQL_EDITOR, PgmSqlEditorClass ))
#define PGM_SQL_EDITOR_CONFIG_GROUP            ("SqlEditor")
G_BEGIN_DECLS

typedef struct _PgmSqlEditor           PgmSqlEditor;
typedef struct _PgmSqlEditorClass      PgmSqlEditorClass;
typedef struct _PgmSqlEditorPrivate    PgmSqlEditorPrivate;

struct _PgmSqlEditor
{
    GtkSourceView parent_instance;

    /*< private >*/
    PgmSqlEditorPrivate *priv;
};

struct _PgmSqlEditorClass
{
    GtkSourceViewClass parent_class;

    /*< signals >*/
    void (*can_undo_changed) ( PgmSqlEditor *editor );
    void (*can_redo_changed) ( PgmSqlEditor *editor );
};

GType       pgm_sql_editor_get_type          ();
GtkWidget * pgm_sql_editor_new               ();
void        pgm_sql_editor_set_text          ( PgmSqlEditor *editor, const gchar *text, gint len );
void        pgm_sql_editor_load_from_file    ( PgmSqlEditor *editor, const gchar *filename );
void        pgm_sql_editor_save_to_file      ( PgmSqlEditor *editor, const gchar *filename );
void        pgm_sql_editor_save              ( PgmSqlEditor *editor );
gboolean    pgm_sql_editor_can_undo          ( PgmSqlEditor *editor );
gboolean    pgm_sql_editor_can_redo          ( PgmSqlEditor *editor );
void        pgm_sql_editor_undo              ( PgmSqlEditor *editor );
void        pgm_sql_editor_redo              ( PgmSqlEditor *editor );
void        pgm_sql_editor_cut               ( PgmSqlEditor *editor );
void        pgm_sql_editor_copy              ( PgmSqlEditor *editor );
void        pgm_sql_editor_paste             ( PgmSqlEditor *editor );
gboolean    pgm_sql_editor_has_selection     ( PgmSqlEditor *editor );
gchar*      pgm_sql_editor_get_selected_text ( PgmSqlEditor *editor );
gchar*      pgm_sql_editor_get_text          ( PgmSqlEditor *editor );
void        pgm_sql_editor_clear             ( PgmSqlEditor *editor );
void        pgm_sql_editor_reload_config     ();

G_END_DECLS

#endif /* PGMSQLEDITOR_H */
