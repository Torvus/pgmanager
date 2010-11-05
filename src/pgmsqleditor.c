#include "pgmsqleditor.h"

#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourceundomanager.h>

#include <enca.h>

#include "pgmmainwindow.h"
#include "pgmtranslate.h"

G_DEFINE_TYPE( PgmSqlEditor, pgm_sql_editor, GTK_TYPE_SOURCE_VIEW )

struct _PgmSqlEditorPrivate
{
    GtkSourceLanguageManager *language_manager;
    GtkSourceLanguage *language;
    GtkSourceBuffer *buffer;
    GtkSourceUndoManager *undo_manager;
};

enum
{
    CAN_UNDO_CHANGED,
    CAN_REDO_CHANGED,
    LAST_SIGNAL
};

enum
{
    PROP_0,
    PROP_CAN_UNDO,
    PROP_CAN_REDO
};

static void pgm_sql_editor_init( PgmSqlEditor *editor );
static void pgm_sql_editor_class_init( PgmSqlEditorClass *klass );
static void pgm_sql_editor_dispose( GObject *gobject );
static void pgm_sql_editor_finalize( GObject *gobject );
static void pgm_sql_editor_load_finished( GObject *object, GAsyncResult *res, gpointer data );
/*static void pgm_sql_editor_save_finished( GObject *object, GAsyncResult *res, gpointer data );*/
static void pgm_sql_editor_can_undo_changed( GtkSourceUndoManager *manager, gpointer data );
static void pgm_sql_editor_can_redo_changed( GtkSourceUndoManager *manager, gpointer data );
static void pgm_sql_editor_set_property( GObject        *object,
                                         guint           property_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec );
static void pgm_sql_editor_get_property( GObject        *object,
                                         guint           property_id,
                                         GValue         *value,
                                         GParamSpec     *pspec );

static void pgm_sql_editor_load_config( PgmSqlEditor *editor );
/*static void pgm_sql_editor_save_config( PgmSqlEditor *editor );*/

static guint pgm_sql_editor_signals[ LAST_SIGNAL ] = {0};
static GList *sql_editor_list = NULL;

static void
pgm_sql_editor_init( PgmSqlEditor *editor )
{
    editor->priv = g_new( PgmSqlEditorPrivate, 1 );

    sql_editor_list = g_list_append( sql_editor_list, editor );
    pgm_sql_editor_load_config( editor );
}

static void
pgm_sql_editor_class_init( PgmSqlEditorClass *klass )
{
    GObjectClass *gclass = G_OBJECT_CLASS( klass );

    gclass->dispose = pgm_sql_editor_dispose;
    gclass->finalize = pgm_sql_editor_finalize;
    gclass->set_property = pgm_sql_editor_set_property;
    gclass->get_property = pgm_sql_editor_get_property;

    pgm_sql_editor_signals[ CAN_UNDO_CHANGED ] =
                g_signal_new( "can-undo-changed",
                              G_OBJECT_CLASS_TYPE( gclass ),
                              G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                              G_STRUCT_OFFSET( PgmSqlEditorClass, can_undo_changed ),
                              NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0 );

    pgm_sql_editor_signals[ CAN_REDO_CHANGED ] =
                g_signal_new( "can-redo-changed",
                              G_OBJECT_CLASS_TYPE( gclass ),
                              G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                              G_STRUCT_OFFSET( PgmSqlEditorClass, can_redo_changed ),
                              NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0 );

    g_object_class_install_property( gclass,
                                     PROP_CAN_UNDO,
                                     g_param_spec_boolean( "can-undo",
                                                           "Can undo",
                                                           "Can undo",
                                                           FALSE,
                                                           G_PARAM_STATIC_STRINGS | G_PARAM_READABLE ) );

    g_object_class_install_property( gclass,
                                     PROP_CAN_REDO,
                                     g_param_spec_boolean( "can-redo",
                                                           "Can redo",
                                                           "Can redo",
                                                           FALSE,
                                                           G_PARAM_STATIC_STRINGS | G_PARAM_READABLE ) );
}

static void
pgm_sql_editor_dispose( GObject *gobject )
{
    /*pgm_sql_editor_save_config( PGM_SQL_EDITOR( gobject ) );*/
    sql_editor_list = g_list_remove( sql_editor_list, PGM_SQL_EDITOR( gobject ) );
    G_OBJECT_CLASS( pgm_sql_editor_parent_class )->dispose( gobject );
}

static void
pgm_sql_editor_finalize( GObject *gobject )
{
    PgmSqlEditor *editor = PGM_SQL_EDITOR( gobject );

    g_free( editor->priv );

    G_OBJECT_CLASS( pgm_sql_editor_parent_class )->finalize( gobject );
}

static void
pgm_sql_editor_set_property( GObject        *object,
                             guint           property_id,
                             const GValue   *value,
                             GParamSpec     *pspec )
{
    /*PgmSqlEditor *editor = PGM_SQL_EDITOR( object );*/

    switch( property_id )
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
pgm_sql_editor_get_property( GObject        *object,
                             guint           property_id,
                             GValue         *value,
                             GParamSpec     *pspec )
{
    PgmSqlEditor *editor = PGM_SQL_EDITOR( object );

    switch( property_id )
    {
        case PROP_CAN_UNDO:
            g_object_get_property( G_OBJECT( editor->priv->buffer ), "can-undo", value );
            break;

        case PROP_CAN_REDO:
            g_object_get_property( G_OBJECT( editor->priv->buffer ), "can-redo", value );
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

GtkWidget *
pgm_sql_editor_new()
{
    PgmSqlEditor *editor;
    GtkSourceLanguageManager *language_manager;
    GtkSourceLanguage *language;
    GtkSourceBuffer *buffer;
    GtkSourceUndoManager *undo_manager;
    GValue val = {0};

    language_manager = gtk_source_language_manager_get_default();
    language = gtk_source_language_manager_get_language( language_manager, "sql" );
    buffer = gtk_source_buffer_new_with_language( language );

    editor = g_object_new( PGM_TYPE_SQL_EDITOR, NULL );

    gtk_text_view_set_buffer( GTK_TEXT_VIEW( editor ), GTK_TEXT_BUFFER( buffer ) );

    /*gtk_source_view_set_show_line_numbers( GTK_SOURCE_VIEW( editor ), TRUE );*/

    editor->priv->buffer = buffer;
    editor->priv->language = language;
    editor->priv->language_manager = language_manager;

    g_value_init( &val, GTK_TYPE_SOURCE_UNDO_MANAGER );
    g_object_get_property( G_OBJECT( buffer ), "undo-manager", &val );

    gtk_widget_modify_font( GTK_WIDGET( editor ), pango_font_description_from_string( "Monospace 10" ) );

    undo_manager = g_value_get_object( &val );
    editor->priv->undo_manager = undo_manager;

    g_signal_connect( G_OBJECT( undo_manager ), "can-undo-changed", G_CALLBACK( pgm_sql_editor_can_undo_changed ), editor );
    g_signal_connect( G_OBJECT( undo_manager ), "can-redo-changed", G_CALLBACK( pgm_sql_editor_can_redo_changed ), editor );

    return GTK_WIDGET( editor );
}

void
pgm_sql_editor_set_text( PgmSqlEditor *editor, const gchar *text, gint len )
{
    EncaAnalyser analyser;
    EncaEncoding encoding;
    EncaEncoding utf_encoding;
    gchar *encoded_text;
    gsize encoded_len;

    g_return_if_fail( editor != NULL );

    if( len < 0 )
        len = g_utf8_strlen( text, -1 );

    analyser = enca_analyser_alloc( "ru" );
    encoding = enca_analyse_const( analyser, (const unsigned char*)text, len );
    utf_encoding = enca_parse_encoding_name( "UTF-8" );

    gtk_source_undo_manager_begin_not_undoable_action( editor->priv->undo_manager );

    if( encoding.charset != utf_encoding.charset || encoding.surface != utf_encoding.surface )
    {
        encoded_text = g_convert( text, len,
                                  "UTF-8", enca_charset_name( encoding.charset, ENCA_NAME_STYLE_ICONV ),
                                  NULL, &encoded_len, NULL );
        gtk_text_buffer_set_text( GTK_TEXT_BUFFER( editor->priv->buffer ), encoded_text, encoded_len );
        g_free( encoded_text );
    }
    else
    {
        gtk_text_buffer_set_text( GTK_TEXT_BUFFER( editor->priv->buffer ), text, len );
    }
    gtk_text_buffer_set_modified( GTK_TEXT_BUFFER( editor->priv->buffer ), FALSE );
    gtk_source_undo_manager_end_not_undoable_action( editor->priv->undo_manager );
    enca_analyser_free( analyser );
}

void
pgm_sql_editor_load_from_file( PgmSqlEditor *editor, const gchar *filename )
{
    GFile *file;
    GCancellable *cancel;

    pgm_main_window_start_wait();

    file = g_file_new_for_path( filename );
    cancel = g_cancellable_new();
    g_file_load_contents_async( file, cancel, pgm_sql_editor_load_finished, editor );
}

static void
pgm_sql_editor_load_finished( GObject *object, GAsyncResult *res, gpointer data )
{
    PgmSqlEditor *editor;
    GFile *file;
    gchar *file_contents;
    gsize file_length;

    editor = PGM_SQL_EDITOR( data );
    file = G_FILE( object );

    if( g_file_load_contents_finish( file, res, &file_contents, &file_length, NULL, NULL ) )
    {
        pgm_sql_editor_set_text( editor, file_contents, file_length );
        g_free( file_contents );
    }

    pgm_main_window_stop_wait();
}

void
pgm_sql_editor_save_to_file( PgmSqlEditor *editor, const gchar *filename )
{
    GFile *file;
    gchar *text;
    gchar *locale_text;
    gsize locale_len;
    GValue val = {0};

    g_value_init( &val, G_TYPE_STRING );

    /*pgm_main_window_start_wait();*/

    file = g_file_new_for_path( filename );
    g_object_get_property( G_OBJECT( editor->priv->buffer ), "text", &val );
    text = pgm_sql_editor_get_text( editor );
    locale_text = g_locale_from_utf8( text, g_utf8_strlen( text, -1 ), NULL, &locale_len, NULL );
    if( g_file_replace_contents( file, locale_text, locale_len, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, NULL ) )
        gtk_text_buffer_set_modified( GTK_TEXT_BUFFER( editor->priv->buffer ), FALSE );
    g_free( locale_text );
    g_free( text );

}
/*
static void
pgm_sql_editor_save_finished( GObject *object, GAsyncResult *res, gpointer data )
{
    GFile *file = G_FILE( object );
    g_file_replace_contents_finish( file, res, NULL, NULL );
    pgm_main_window_stop_wait();
}
*/
void
pgm_sql_editor_save( PgmSqlEditor *editor )
{
    GtkWidget *dialog;
    GtkFileFilter *filter;
    gchar *filename;
    gint response;

    dialog = pgm_main_window_get_file_chooser_dialog( "Save File",
                      GTK_FILE_CHOOSER_ACTION_SAVE,
                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                      NULL);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name( filter, "Query (*.sql)" );
    gtk_file_filter_add_pattern( filter, "*.sql" );
    gtk_file_chooser_add_filter( GTK_FILE_CHOOSER( dialog ), filter );

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name( filter, "All Files" );
    gtk_file_filter_add_pattern( filter, "*" );
    gtk_file_chooser_add_filter( GTK_FILE_CHOOSER( dialog ), filter );

    gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER( dialog ), TRUE );
    gtk_file_chooser_set_current_name( GTK_FILE_CHOOSER( dialog ), "script.sql" );

    response = gtk_dialog_run( GTK_DIALOG( dialog ) );
    filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER( dialog ) );
    gtk_widget_destroy( dialog );
    if( response == GTK_RESPONSE_ACCEPT )
        pgm_sql_editor_save_to_file( PGM_SQL_EDITOR( editor ), filename );
    g_free( filename );
}

gboolean
pgm_sql_editor_can_undo( PgmSqlEditor *editor )
{
    return gtk_source_buffer_can_undo( editor->priv->buffer );
}

gboolean
pgm_sql_editor_can_redo( PgmSqlEditor *editor )
{
    return gtk_source_buffer_can_redo( editor->priv->buffer );
}

void
pgm_sql_editor_undo( PgmSqlEditor *editor )
{
    gtk_source_buffer_undo( editor->priv->buffer );
}

void
pgm_sql_editor_redo( PgmSqlEditor *editor )
{
    gtk_source_buffer_redo( editor->priv->buffer );
}

void
pgm_sql_editor_cut( PgmSqlEditor *editor )
{
    GtkClipboard *clipboard;
    GdkAtom atom;

    atom = gdk_atom_intern( "CLIPBOARD", TRUE );
    if( atom == GDK_NONE )
        return;

    clipboard = gtk_clipboard_get( atom );
    gtk_text_buffer_cut_clipboard( GTK_TEXT_BUFFER( editor->priv->buffer ), clipboard, TRUE );
}

void
pgm_sql_editor_copy( PgmSqlEditor *editor )
{
    GtkClipboard *clipboard;
    GdkAtom atom;

    atom = gdk_atom_intern( "CLIPBOARD", TRUE );
    if( atom == GDK_NONE )
        return;

    clipboard = gtk_clipboard_get( atom );
    gtk_text_buffer_copy_clipboard( GTK_TEXT_BUFFER( editor->priv->buffer ), clipboard );
}

void
pgm_sql_editor_paste( PgmSqlEditor *editor )
{
    GtkClipboard *clipboard;
    GdkAtom atom;

    atom = gdk_atom_intern( "CLIPBOARD", TRUE );
    if( atom == GDK_NONE )
        return;

    clipboard = gtk_clipboard_get( atom );
    gtk_text_buffer_paste_clipboard( GTK_TEXT_BUFFER( editor->priv->buffer ), clipboard, NULL, TRUE );
}

gboolean
pgm_sql_editor_has_selection( PgmSqlEditor *editor )
{
    return gtk_text_buffer_get_has_selection( GTK_TEXT_BUFFER( editor->priv->buffer ) );
}

gchar*
pgm_sql_editor_get_selected_text( PgmSqlEditor *editor )
{
    GtkTextIter start;
    GtkTextIter end;

    gtk_text_buffer_get_selection_bounds( GTK_TEXT_BUFFER( editor->priv->buffer ), &start, &end );
    return gtk_text_buffer_get_text( GTK_TEXT_BUFFER( editor->priv->buffer ), &start, &end, FALSE );
}

gchar*
pgm_sql_editor_get_text( PgmSqlEditor *editor )
{
    GtkTextIter start;
    GtkTextIter end;

    gtk_text_buffer_get_start_iter( GTK_TEXT_BUFFER( editor->priv->buffer ), &start );
    gtk_text_buffer_get_end_iter( GTK_TEXT_BUFFER( editor->priv->buffer ), &end );
    return gtk_text_buffer_get_text( GTK_TEXT_BUFFER( editor->priv->buffer ), &start, &end, FALSE );
}

void
pgm_sql_editor_clear( PgmSqlEditor *editor )
{
    GtkWidget *dialog;
    gint response;

    if( gtk_text_buffer_get_modified( GTK_TEXT_BUFFER( editor->priv->buffer ) ) )
    {
        dialog = gtk_message_dialog_new( NULL,
                                         GTK_DIALOG_MODAL,
                                         GTK_MESSAGE_QUESTION,
                                         GTK_BUTTONS_YES_NO,
                                         _("Text is changed. Save the changes?") );
        gtk_dialog_add_button( GTK_DIALOG( dialog ), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL );
        response = gtk_dialog_run( GTK_DIALOG( dialog ) );
        if( response == GTK_RESPONSE_CANCEL )
            return;
        if( response == GTK_RESPONSE_YES )
        {
        }
        gtk_widget_destroy( dialog );
    }
    gtk_text_buffer_set_text( GTK_TEXT_BUFFER( editor->priv->buffer ), "", -1 );
}

static void
pgm_sql_editor_can_undo_changed( GtkSourceUndoManager *manager, gpointer data )
{
    g_signal_emit( data, pgm_sql_editor_signals[ CAN_UNDO_CHANGED ], 0 );
}

static void
pgm_sql_editor_can_redo_changed( GtkSourceUndoManager *manager, gpointer data )
{
    g_signal_emit( data, pgm_sql_editor_signals[ CAN_REDO_CHANGED ], 0 );
}

static void
pgm_sql_editor_load_config( PgmSqlEditor *editor )
{
    GKeyFile *config = pgm_main_window_get_config();

    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "AutoIndent", NULL ) )
        gtk_source_view_set_auto_indent( GTK_SOURCE_VIEW( editor ),
                                         g_key_file_get_boolean( config,
                                                                 PGM_SQL_EDITOR_CONFIG_GROUP,
                                                                 "AutoIndent",
                                                                 NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "IndentOnTab", NULL ) )
        gtk_source_view_set_indent_on_tab( GTK_SOURCE_VIEW( editor ),
                                           g_key_file_get_boolean( config,
                                                                   PGM_SQL_EDITOR_CONFIG_GROUP,
                                                                   "IndentOnTab",
                                                                   NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "ShowLineNumbers", NULL ) )
        gtk_source_view_set_show_line_numbers( GTK_SOURCE_VIEW( editor ),
                                               g_key_file_get_boolean( config,
                                                                       PGM_SQL_EDITOR_CONFIG_GROUP,
                                                                       "ShowLineNumbers",
                                                                       NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "SmartHomeEnd", NULL ) )
        gtk_source_view_set_smart_home_end( GTK_SOURCE_VIEW( editor ),
                                            g_key_file_get_boolean( config,
                                                                    PGM_SQL_EDITOR_CONFIG_GROUP,
                                                                    "SmartHomeEnd",
                                                                    NULL ) ? GTK_SOURCE_SMART_HOME_END_BEFORE : GTK_SOURCE_SMART_HOME_END_DISABLED );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "InsertSpacesInsteadOfTab", NULL ) )
        gtk_source_view_set_insert_spaces_instead_of_tabs( GTK_SOURCE_VIEW( editor ),
                                                           g_key_file_get_boolean( config,
                                                                                   PGM_SQL_EDITOR_CONFIG_GROUP,
                                                                                   "InsertSpacesInsteadOfTab",
                                                                                   NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "HighlightCurrentLine", NULL ) )
        gtk_source_view_set_highlight_current_line( GTK_SOURCE_VIEW( editor ),
                                                    g_key_file_get_boolean( config,
                                                                            PGM_SQL_EDITOR_CONFIG_GROUP,
                                                                            "HighlightCurrentLine",
                                                                            NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "IndentWidth", NULL ) )
        gtk_source_view_set_indent_width( GTK_SOURCE_VIEW( editor ),
                                          g_key_file_get_integer( config,
                                                                  PGM_SQL_EDITOR_CONFIG_GROUP,
                                                                  "IndentWidth",
                                                                  NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "TabWidth", NULL ) )
        gtk_source_view_set_tab_width( GTK_SOURCE_VIEW( editor ),
                                       g_key_file_get_integer( config,
                                                               PGM_SQL_EDITOR_CONFIG_GROUP,
                                                               "TabWidth",
                                                               NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "Font", NULL ) )
        gtk_widget_modify_font( GTK_WIDGET( editor ),
                                pango_font_description_from_string( g_key_file_get_string( config,
                                                                                           PGM_SQL_EDITOR_CONFIG_GROUP,
                                                                                           "Font",
                                                                                           NULL ) ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "DrawSpaces", NULL ) )
        switch( g_key_file_get_integer( config,
                                        PGM_SQL_EDITOR_CONFIG_GROUP,
                                        "DrawSpaces",
                                        NULL ) )
        {
            case 0:
                gtk_source_view_set_draw_spaces( GTK_SOURCE_VIEW( editor ), 0 );
                break;
            case 1:
                gtk_source_view_set_draw_spaces( GTK_SOURCE_VIEW( editor ), GTK_SOURCE_DRAW_SPACES_SPACE );
                break;
            case 2:
                gtk_source_view_set_draw_spaces( GTK_SOURCE_VIEW( editor ), GTK_SOURCE_DRAW_SPACES_TAB );
                break;
            case 3:
                gtk_source_view_set_draw_spaces( GTK_SOURCE_VIEW( editor ), GTK_SOURCE_DRAW_SPACES_NEWLINE );
                break;
            case 4:
                gtk_source_view_set_draw_spaces( GTK_SOURCE_VIEW( editor ), GTK_SOURCE_DRAW_SPACES_NBSP );
                break;
            case 5:
                gtk_source_view_set_draw_spaces( GTK_SOURCE_VIEW( editor ), GTK_SOURCE_DRAW_SPACES_LEADING );
                break;
            case 6:
                gtk_source_view_set_draw_spaces( GTK_SOURCE_VIEW( editor ), GTK_SOURCE_DRAW_SPACES_TEXT );
                break;
            case 7:
                gtk_source_view_set_draw_spaces( GTK_SOURCE_VIEW( editor ), GTK_SOURCE_DRAW_SPACES_TRAILING );
                break;
            case 8:
                gtk_source_view_set_draw_spaces( GTK_SOURCE_VIEW( editor ), GTK_SOURCE_DRAW_SPACES_ALL );
                break;
        }
}
/*
static void
pgm_sql_editor_save_config( PgmSqlEditor *editor )
{
    GKeyFile *config = pgm_main_window_get_config();

    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "AutoIndent",
                            gtk_source_view_get_auto_indent( GTK_SOURCE_VIEW( editor ) ) );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "IndentOnTab",
                            gtk_source_view_get_indent_on_tab( GTK_SOURCE_VIEW( editor ) ) );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "ShowLineNumbers",
                            gtk_source_view_get_show_line_numbers( GTK_SOURCE_VIEW( editor ) ) );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "SmartHomeEnd",
                            gtk_source_view_get_smart_home_end( GTK_SOURCE_VIEW( editor ) ) == GTK_SOURCE_SMART_HOME_END_BEFORE );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "InsertSpacesInsteadOfTab",
                            gtk_source_view_get_insert_spaces_instead_of_tabs( GTK_SOURCE_VIEW( editor ) ) );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "HighlightCurrentLine",
                            gtk_source_view_get_highlight_current_line( GTK_SOURCE_VIEW( editor ) ) );
    g_key_file_set_integer( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "IndentWidth",
                            gtk_source_view_get_indent_width( GTK_SOURCE_VIEW( editor ) ) );
    g_key_file_set_integer( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "TabWidth",
                            gtk_source_view_get_tab_width( GTK_SOURCE_VIEW( editor ) ) );
}
*/
void
pgm_sql_editor_reload_config()
{
    GList *cur;

    cur = g_list_first( sql_editor_list );

    while( cur )
    {
        pgm_sql_editor_load_config( PGM_SQL_EDITOR( cur->data ) );
        cur = g_list_next( cur );
    }
}
