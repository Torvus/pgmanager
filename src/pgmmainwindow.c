#include "pgmmainwindow.h"

#include <stdarg.h>

#include <glib/gprintf.h>
#include <gconf/gconf-client.h>

#include "config.h"
#include "pgmobject.h"
#include "pgmtranslate.h"
#include "pgmserverlist.h"
#include "pgmserver.h"
#include "pgmdatabase.h"
#include "pgmstock.h"
#include "pgmqueryeditor.h"
#include "pgmpreferencesdialog.h"
#include "license.h"

static char menu_str[] =
            "<ui>"
            "   <menubar name='MenuMain'>"
            "       <menu name='MenuFile' action='ActionMenuFile'>"
            "           <menuitem name='Connect' action='ActionFileConnect' />"
            "           <separator />"
            "           <menuitem name='Quit' action='ActionExit' />"
            "       </menu>"
            "       <menu name='MenuSettings' action='ActionMenuSettings'>"
            "           <menuitem action='ActionSettingsPreferences' />"
            "           <menuitem action='ActionSettingsFont' />"
            "       </menu>"
            "       <menu name='MenuHelp' action='ActionMenuHelp'>"
            "           <menuitem name='About' action='ActionHelpAbout' />"
            "       </menu>"
            "   </menubar>"
            "   <toolbar name='ToolMain'>"
            "       <toolitem name='Connect' action='ActionFileConnect' />"
            "       <toolitem name='Sql' action='ActionSqlEditor' />"
            "       <separator />"
            "       <toolitem name='Quit' action='ActionExit' />"
            "   </toolbar>"
            "</ui>";

static void on_help_about_activated();
static void on_file_connect_activated();
static void on_open_sql_editor_activated();
static void on_select_font_activated();
static void on_preferences_activated();

static GtkActionEntry menu_actions[] =
{
    { "ActionMenuFile", NULL, "_File", NULL, "File menu", NULL },
    { "ActionFileConnect", GTK_STOCK_CONNECT, "_Connect", NULL, "Create connection", G_CALLBACK( on_file_connect_activated ) },
    { "ActionExit", GTK_STOCK_QUIT, "_Quit", "<Control>Q", "Close program", gtk_main_quit },
    { "ActionMenuSettings", NULL, "_Settings", NULL, "Settings menu", NULL },
    { "ActionSettingsPreferences", GTK_STOCK_PREFERENCES, "_Preferences", NULL, "Edit Preferences", G_CALLBACK( on_preferences_activated ) },
    { "ActionSettingsFont", GTK_STOCK_SELECT_FONT, "_Font", NULL, "Select font", G_CALLBACK( on_select_font_activated ) },
    { "ActionMenuHelp", NULL, "_Help", NULL, "Help menu", NULL },
    { "ActionHelpAbout", GTK_STOCK_ABOUT, "_About", NULL, "About program", G_CALLBACK( on_help_about_activated ) },
    { "ActionSqlEditor", PGM_STOCK_SQL_EDITOR, "_Sql", NULL, "Open sql editor", G_CALLBACK( on_open_sql_editor_activated ) }
};

static GtkUIManager *ui_manager;
static GtkActionGroup *action_group;

static GtkTreeStore *tree_store;
static GtkWidget *tree_view;
static GtkWidget *notebook;
static GtkWidget *main_window;
static GtkWidget *h_paned;
static PgmServerList *server_list;

static GdkCursor *wait_cursor;
static GFile *config_file;
static GKeyFile *config;

static void pgm_main_window_cleanup();
static void pgm_main_window_on_row_activated( GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data );
static void pgm_main_window_on_row_expanded( GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data );
static gboolean pgm_main_window_on_close_tab_pressed( GtkWidget *event_box, GdkEventButton *event, gpointer data );
static gboolean pgm_main_window_on_close_tab_released( GtkWidget *event_box, GdkEventButton *event, gpointer data );
static gboolean pgm_main_window_on_close_tab_enter( GtkWidget *event_box, GdkEventButton *event, gpointer data );
static gboolean pgm_main_window_on_close_tab_leave( GtkWidget *event_box, GdkEventButton *event, gpointer data );

static void pgm_main_window_load_config();

static void
menu_init()
{
    GError *error = NULL;

    static gboolean inited = FALSE;
    if( inited )
        return;
    ui_manager = gtk_ui_manager_new();
    action_group = gtk_action_group_new( "PgmActionGroup" );
    gtk_action_group_add_actions( action_group, menu_actions, G_N_ELEMENTS( menu_actions ), NULL );
    gtk_ui_manager_insert_action_group( ui_manager, action_group, 0 );
    gtk_ui_manager_add_ui_from_string( ui_manager, menu_str, -1, &error );
    gtk_ui_manager_ensure_update( ui_manager );
    gtk_window_add_accel_group( GTK_WINDOW( main_window ), gtk_ui_manager_get_accel_group( ui_manager ) );
}

static GtkWidget *
create_menu_bar ()
{
    menu_init();
    return gtk_ui_manager_get_widget( ui_manager, "/MenuMain" );
}

static GtkWidget *
create_tool_bar ()
{
    menu_init();
    return gtk_ui_manager_get_widget( ui_manager, "/ToolMain" );
}

GtkWidget *
pgm_main_window_new()
{
    GtkWidget *v_box;
    GtkWidget *menu_bar;
    GtkWidget *tool_bar;
    GtkWidget *scrolled;
    GtkWidget *status_bar;
    GtkCellRenderer *icon_renderer;
    GtkCellRenderer *name_renderer;
    GtkCellRenderer *count_renderer;
    PangoFontDescription *font_desc;
    GtkTreeViewColumn *column;
    GValue val = {0};
    gchar *config_data;
    gsize config_data_len;

    config_file = g_file_parse_name( "~/.pgmanagerrc" );
    g_file_load_contents( config_file, NULL, &config_data, &config_data_len, NULL, NULL );
    config = g_key_file_new();
    g_key_file_load_from_data( config, config_data, config_data_len, G_KEY_FILE_KEEP_COMMENTS, NULL );
    g_free( config_data );

    wait_cursor = gdk_cursor_new( GDK_WATCH );

    main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    v_box = gtk_vbox_new( FALSE, 0 );

    gtk_container_add( GTK_CONTAINER( main_window ), v_box );

    menu_bar = create_menu_bar();
    gtk_box_pack_start( GTK_BOX( v_box ), menu_bar, FALSE, FALSE, 0 );

    tool_bar = create_tool_bar();
    gtk_box_pack_start( GTK_BOX( v_box ), tool_bar, FALSE, FALSE, 0 );

    h_paned = gtk_hpaned_new();
    gtk_box_pack_start( GTK_BOX( v_box ), h_paned, TRUE, TRUE, 0 );

    scrolled = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    tree_store = gtk_tree_store_new( 3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING );
    gtk_tree_sortable_set_sort_column_id( GTK_TREE_SORTABLE( tree_store ), 1, GTK_SORT_ASCENDING );

    tree_view = gtk_tree_view_new_with_model( GTK_TREE_MODEL( tree_store ) );
    g_object_unref( tree_store );
    gtk_tree_view_set_tooltip_column( GTK_TREE_VIEW( tree_view ), 1 );
    gtk_container_add( GTK_CONTAINER( scrolled ), tree_view );

    icon_renderer = gtk_cell_renderer_pixbuf_new();
    name_renderer = gtk_cell_renderer_text_new();
    count_renderer = gtk_cell_renderer_text_new();

    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title( column, _("Objects") );
    gtk_tree_view_column_set_sort_column_id( column, 1 );
    gtk_tree_view_column_pack_start( column, icon_renderer, FALSE );
    gtk_tree_view_column_set_attributes( column,
                                         icon_renderer,
                                         "pixbuf",
                                         0,
                                         NULL );
    gtk_tree_view_column_pack_start( column, name_renderer, TRUE );
    gtk_tree_view_column_set_attributes( column,
                                         name_renderer,
                                         "text",
                                         1,
                                         NULL );
    gtk_tree_view_column_pack_start( column, count_renderer, FALSE );
    gtk_tree_view_column_set_attributes( column,
                                         count_renderer,
                                         "text",
                                         2,
                                         NULL );
    g_value_init( &val, PANGO_TYPE_FONT_DESCRIPTION );
    g_object_get_property( G_OBJECT( count_renderer ), "font-desc", &val );
    font_desc = g_value_get_boxed( &val );
    pango_font_description_set_weight( font_desc, PANGO_WEIGHT_BOLD );
    g_value_set_boxed( &val, font_desc );
    g_object_set_property( G_OBJECT( count_renderer ), "font-desc", &val );

    gtk_tree_view_append_column( GTK_TREE_VIEW( tree_view ), column );

    gtk_paned_add1( GTK_PANED( h_paned ), scrolled );

    notebook = gtk_notebook_new();
    gtk_paned_add2( GTK_PANED( h_paned ), notebook );

    status_bar = gtk_statusbar_new();
    gtk_box_pack_start( GTK_BOX( v_box ), status_bar, FALSE, FALSE, 0 );

    server_list = pgm_server_list_new( NULL );
    pgm_object_show( PGM_OBJECT( server_list ) );

    pgm_main_window_load_config();
    gtk_widget_show_all( v_box );

    g_signal_connect_after( main_window, "delete_event", pgm_main_window_cleanup, NULL );
    g_signal_connect( tree_view, "row-activated", G_CALLBACK( pgm_main_window_on_row_activated ), NULL );
    g_signal_connect( tree_view, "row-expanded", G_CALLBACK( pgm_main_window_on_row_expanded ), NULL );

    return main_window;
}

GdkPixbuf*
pgm_main_window_render_icon( const gchar *stock_id )
{
    return gtk_widget_render_icon( main_window, stock_id, GTK_ICON_SIZE_MENU, NULL );
}

GtkTreeStore *
pgm_main_window_get_tree_store()
{
    return tree_store;
}

static void
on_help_about_activated()
{
    GtkWidget *dialog;

    dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name( GTK_ABOUT_DIALOG( dialog ), PACKAGE_NAME );
    gtk_about_dialog_set_version( GTK_ABOUT_DIALOG( dialog ), PACKAGE_VERSION );
    gtk_about_dialog_set_license( GTK_ABOUT_DIALOG( dialog ), license );
    gtk_about_dialog_set_wrap_license( GTK_ABOUT_DIALOG( dialog ), TRUE );

    gtk_dialog_run( GTK_DIALOG( dialog ) );
    gtk_widget_destroy( GTK_WIDGET( dialog ) );
}

static void
on_file_connect_activated()
{
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *table;
    GtkWidget *host_label;      GtkWidget *host_entry;
    GtkWidget *port_label;      GtkWidget *port_entry;
    GtkWidget *login_label;     GtkWidget *login_entry;
    GtkWidget *password_label;  GtkWidget *password_entry;
    GtkWidget *ssl_label;       GtkWidget *ssl_entry;
    PgmServer *server;
    PgmSslMode ssl_mode;
    GFile *pgpass_file;

    dialog = gtk_dialog_new_with_buttons( _("New connection"),
                                          GTK_WINDOW( main_window ),
                                          GTK_DIALOG_MODAL,
                                          GTK_STOCK_OK,
                                          GTK_RESPONSE_ACCEPT,
                                          GTK_STOCK_CANCEL,
                                          GTK_RESPONSE_REJECT,
                                          NULL );

    content_area = gtk_dialog_get_content_area( GTK_DIALOG( dialog ) );

    table = gtk_table_new( 5, 2, FALSE );
    gtk_table_set_col_spacings( GTK_TABLE( table ), 10 );
    gtk_container_add( GTK_CONTAINER( content_area ), table );

    host_label = gtk_label_new( _("Host:") );
    gtk_misc_set_alignment( GTK_MISC( host_label ), 0, 0.5 );
    gtk_table_attach_defaults( GTK_TABLE( table ), host_label, 0, 1, 0, 1 );

    host_entry = gtk_entry_new();
    gtk_table_attach_defaults( GTK_TABLE( table ), host_entry, 1, 2, 0, 1 );

    port_label = gtk_label_new( _("Port:") );
    gtk_misc_set_alignment( GTK_MISC( port_label ), 0, 0.5 );
    gtk_table_attach_defaults( GTK_TABLE( table ), port_label, 0, 1, 1, 2 );

    port_entry = gtk_spin_button_new_with_range( 0, 9999999, 1 );
    gtk_spin_button_set_value( GTK_SPIN_BUTTON( port_entry ), 5432 );
    gtk_table_attach_defaults( GTK_TABLE( table ), port_entry, 1, 2, 1, 2 );

    login_label = gtk_label_new( _("Login:") );
    gtk_misc_set_alignment( GTK_MISC( login_label ), 0, 0.5 );
    gtk_table_attach_defaults( GTK_TABLE( table ), login_label, 0, 1, 2, 3 );

    login_entry = gtk_entry_new();
    gtk_table_attach_defaults( GTK_TABLE( table ), login_entry, 1, 2, 2, 3 );

    password_label = gtk_label_new( _("Password:") );
    gtk_misc_set_alignment( GTK_MISC( password_label ), 0, 0.5 );
    gtk_table_attach_defaults( GTK_TABLE( table ), password_label, 0, 1, 3, 4 );

    password_entry = gtk_entry_new();
    gtk_entry_set_visibility( GTK_ENTRY( password_entry ), FALSE );
    gtk_table_attach_defaults( GTK_TABLE( table ), password_entry, 1, 2, 3, 4 );

    ssl_label = gtk_label_new( _("SSL mode:") );
    gtk_misc_set_alignment( GTK_MISC( ssl_label ), 0, 0.5 );
    gtk_table_attach_defaults( GTK_TABLE( table ), ssl_label, 0, 1, 4, 5 );

    ssl_entry = gtk_combo_box_new_text();
    gtk_combo_box_append_text( GTK_COMBO_BOX( ssl_entry ), _("Disable") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( ssl_entry ), _("Allow") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( ssl_entry ), _("Prefer") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( ssl_entry ), _("Require") );
    gtk_combo_box_set_active( GTK_COMBO_BOX( ssl_entry ), 2);
    gtk_table_attach_defaults( GTK_TABLE( table ), ssl_entry, 1, 2, 4, 5 );

    gtk_widget_show_all( table );

    if( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_ACCEPT )
    {
        gchar *pgpass_str;
        gchar *pgpass_content;
        gsize pgpass_content_length;
        GFileOutputStream *pgpass_stream;

        switch( gtk_combo_box_get_active( GTK_COMBO_BOX( ssl_entry ) ) )
        {
            case 0:
                     ssl_mode = PGM_SSL_DISABLE;
                     break;
            case 1:
                     ssl_mode = PGM_SSL_ALLOW;
                     break;
            case 2:
                     ssl_mode = PGM_SSL_PREFER;
                     break;
            case 3:
                     ssl_mode = PGM_SSL_REQUIRE;
                     break;
        }
        pgpass_file = g_file_parse_name( "~/.pgpass" );
        g_file_load_contents( pgpass_file, NULL, &pgpass_content, &pgpass_content_length, NULL, NULL );

        pgpass_str = g_strdup_printf( "%s:%d:*:%s:%s", gtk_entry_get_text( GTK_ENTRY( host_entry ) ),
                                                         gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON( port_entry ) ),
                                                         gtk_entry_get_text( GTK_ENTRY( login_entry ) ),
                                                         gtk_entry_get_text( GTK_ENTRY( password_entry ) ) );
        if( g_strrstr( pgpass_str, pgpass_content ) == NULL )
        {
            pgpass_stream = g_file_append_to( pgpass_file, G_FILE_CREATE_PRIVATE, NULL, NULL );
            g_output_stream_write( G_OUTPUT_STREAM( pgpass_stream ), pgpass_str, g_utf8_strlen( pgpass_str, -1 ), NULL, NULL );
            g_output_stream_close( G_OUTPUT_STREAM( pgpass_stream ), NULL, NULL );
        }
        g_free( pgpass_str );

        server = pgm_server_new( server_list,
                                 gtk_entry_get_text( GTK_ENTRY( host_entry ) ),
                                 gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON( port_entry ) ),
                                 gtk_entry_get_text( GTK_ENTRY( login_entry ) ),
                                 ssl_mode,
                                 TRUE );
        pgm_object_show( PGM_OBJECT( server ) );
        pgm_server_read_databases( server );
    }

    gtk_widget_destroy( dialog );
}

static void
pgm_main_window_cleanup()
{
    gchar *config_data;
    gsize config_data_len;

    g_object_unref( G_OBJECT( server_list ) );
    config_data = g_key_file_to_data( config, &config_data_len, NULL );
    g_key_file_free( config );
    g_free( config_data );
}

static void
pgm_main_window_on_row_activated( GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data )
{
    gint depth;
    gint *indices;
    gint i;
    PgmObject *object;

    depth = gtk_tree_path_get_depth( path );
    indices = gtk_tree_path_get_indices( path );
    object = PGM_OBJECT( server_list );

    if( indices[ 0 ] != 0 )
        return;

    for( i = 1; i < depth; i++ )
    {
        object = pgm_object_get_nth_child( object, indices[ i ] );
        if( object == NULL )
            break;
    }

    if( object != NULL )
        pgm_object_activate( object );
}

static void
pgm_main_window_on_row_expanded( GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data )
{
    gint depth;
    gint *indices;
    gint i;
    PgmObject *object;

    depth = gtk_tree_path_get_depth( path );
    indices = gtk_tree_path_get_indices( path );
    object = PGM_OBJECT( server_list );

    if( indices[ 0 ] != 0 )
        return;

    for( i = 1; i < depth; i++ )
    {
        object = pgm_object_get_nth_child( object, indices[ i ] );
        if( object == NULL )
            break;
    }

    if( object != NULL )
        pgm_object_expanded( object );
}

gboolean
pgm_main_window_row_expanded( GtkTreePath *path )
{
    return gtk_tree_view_row_expanded( GTK_TREE_VIEW( tree_view ), path );
}

gboolean
pgm_main_window_expand_row( GtkTreePath *path, gboolean expand_all )
{
    return gtk_tree_view_expand_row( GTK_TREE_VIEW( tree_view ), path, expand_all );
}

gboolean
pgm_main_window_collapse_row( GtkTreePath *path )
{
    return gtk_tree_view_collapse_row( GTK_TREE_VIEW( tree_view ), path );
}

gint
pgm_main_window_show_message_dialog( GtkMessageType type, GtkButtonsType buttons, const gchar *message_format, ... )
{
    GtkWidget *dialog;
    gint ret_val;
    gchar *message = NULL;

    va_list ap;
    va_start( ap, message_format );
    g_vasprintf( &message, message_format, ap );
    va_end( ap );

    dialog = gtk_message_dialog_new( GTK_WINDOW( main_window ), GTK_DIALOG_MODAL, type, buttons, "%s", message );

    ret_val = gtk_dialog_run( GTK_DIALOG( dialog ) );
    gtk_widget_destroy( dialog );

    return ret_val;
}

void
pgm_main_window_append_widget( GtkWidget *widget, const gchar *name, const gchar *stock_id )
{
    GtkWidget *tab_widget;
    GtkWidget *tab_icon;
    GtkWidget *tab_label;
    GtkWidget *tab_close_event_box;
    GtkWidget *tab_close_button;
    gint page_num;

    page_num = gtk_notebook_page_num( GTK_NOTEBOOK( notebook ), widget );
    if( page_num >= 0 )
    {
        gtk_notebook_set_current_page( GTK_NOTEBOOK( notebook ), page_num );
        return;
    }

    tab_widget = gtk_hbox_new( FALSE, 0 );

    tab_icon = gtk_image_new_from_stock( stock_id, GTK_ICON_SIZE_MENU );
    gtk_box_pack_start( GTK_BOX( tab_widget ), tab_icon, FALSE, FALSE, 2 );

    tab_label = gtk_label_new( name );
    gtk_box_pack_start( GTK_BOX( tab_widget ), tab_label, TRUE, TRUE, 2 );

    tab_close_event_box = gtk_event_box_new();

    tab_close_button = gtk_image_new_from_stock( GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU );
    gtk_misc_set_alignment( GTK_MISC( tab_close_button ), 0, 0 );
    gtk_misc_set_padding( GTK_MISC( tab_close_button ), 0, 0 );
    gtk_container_add( GTK_CONTAINER( tab_close_event_box ), tab_close_button );
    gtk_box_pack_start( GTK_BOX( tab_widget ), tab_close_event_box, FALSE, FALSE, 0 );

    page_num = gtk_notebook_append_page( GTK_NOTEBOOK( notebook ), widget, tab_widget );
    gtk_widget_show_all( tab_widget );
    gtk_widget_show_all( widget );
    gtk_notebook_set_current_page( GTK_NOTEBOOK( notebook ), page_num );

    /*g_object_set_data( G_OBJECT( widget ), "page_widget", widget );*/
    g_object_set_data( G_OBJECT( widget ), "tab_pressed", GINT_TO_POINTER( 0 ) );
    g_object_set_data( G_OBJECT( widget ), "tab_close_button", tab_close_event_box );

    g_signal_connect( G_OBJECT( tab_close_event_box ), "button-press-event", G_CALLBACK( pgm_main_window_on_close_tab_pressed ), widget );
    g_signal_connect( G_OBJECT( tab_close_event_box ), "button-release-event", G_CALLBACK( pgm_main_window_on_close_tab_released ), widget );
    g_signal_connect( G_OBJECT( tab_close_event_box ), "enter-notify-event", G_CALLBACK( pgm_main_window_on_close_tab_enter ), widget );
    g_signal_connect( G_OBJECT( tab_close_event_box ), "leave-notify-event", G_CALLBACK( pgm_main_window_on_close_tab_leave ), widget );
}

static gboolean
pgm_main_window_on_close_tab_pressed( GtkWidget *event_box, GdkEventButton *event, gpointer data )
{
    GtkWidget *close_button;
    GtkWidget *widget = GTK_WIDGET( data );

    close_button = GTK_WIDGET( g_object_get_data( G_OBJECT( widget ), "tab_close_button" ) );
    gtk_widget_set_state( close_button, GTK_STATE_ACTIVE );
    g_object_set_data( G_OBJECT( widget ), "tab_pressed", GINT_TO_POINTER( 1 ) );
    return TRUE;
}

static gboolean
pgm_main_window_on_close_tab_released( GtkWidget *event_box, GdkEventButton *event, gpointer data )
{
    GtkWidget *widget;
    GtkWidget *close_button;
    GtkAllocation allocation;
    gint is_pressed;
    gint page_num;

    widget = GTK_WIDGET( data );
    close_button = GTK_WIDGET( g_object_get_data( G_OBJECT( widget ), "tab_close_button" ) );
    is_pressed = GPOINTER_TO_INT( g_object_get_data( G_OBJECT( widget ), "tab_pressed" ) );
    page_num = gtk_notebook_page_num( GTK_NOTEBOOK( notebook ), widget ) ;

    gtk_widget_get_allocation( close_button, &allocation );

    g_object_set_data( G_OBJECT( widget ), "tab_pressed", GINT_TO_POINTER( 0 ) );
    gtk_widget_set_state( close_button, GTK_STATE_NORMAL );

    if( is_pressed == 1 && event->x >= 0 && event->y >= 0 && event->x < allocation.width && event->y < allocation.height )
        gtk_notebook_remove_page( GTK_NOTEBOOK( notebook ), page_num );

    return TRUE;
}

static gboolean
pgm_main_window_on_close_tab_enter( GtkWidget *event_box, GdkEventButton *event, gpointer data )
{
    GtkWidget *close_button;
    GtkWidget *widget;
    gint is_pressed;

    widget = GTK_WIDGET( data );
    close_button = GTK_WIDGET( g_object_get_data( G_OBJECT( widget ), "tab_close_button" ) );
    is_pressed = GPOINTER_TO_INT( g_object_get_data( G_OBJECT( widget ), "tab_pressed" ) );

    g_object_set_data( G_OBJECT( widget ), "tab_state", GINT_TO_POINTER( gtk_widget_get_state( close_button ) ) );

    if( is_pressed == 1 )
        gtk_widget_set_state( close_button, GTK_STATE_ACTIVE );
    else
        gtk_widget_set_state( close_button, GTK_STATE_PRELIGHT );

    return TRUE;
}

static gboolean
pgm_main_window_on_close_tab_leave( GtkWidget *event_box, GdkEventButton *event, gpointer data )
{
    GtkWidget *close_button;
    GtkWidget *widget = GTK_WIDGET( data );

    g_object_get_data( G_OBJECT( widget ), "tab_state" );

    close_button = GTK_WIDGET( g_object_get_data( G_OBJECT( widget ), "tab_close_button" ) );
    gtk_widget_set_state( close_button, GPOINTER_TO_INT( g_object_get_data( G_OBJECT( widget ), "tab_state" ) ) );

    return TRUE;
}

static void
on_open_sql_editor_activated()
{
    GtkWidget *editor;
    GtkTreePath *path;
    gint depth;
    gint *indices;
    gint i;
    PgmObject *object = NULL;

    gtk_tree_view_get_cursor( GTK_TREE_VIEW( tree_view ), &path, NULL );

    if( path )
    {
        depth = gtk_tree_path_get_depth( path );
        indices = gtk_tree_path_get_indices( path );
        object = PGM_OBJECT( server_list );

        if( indices[ 0 ] != 0 )
            return;

        for( i = 1; i < depth; i++ )
        {
            object = pgm_object_get_nth_child( object, indices[ i ] );
            if( object == NULL )
                break;
        }
    }

    editor = pgm_query_editor_new( server_list, pgm_object_get_database( object ) );

    pgm_main_window_append_widget( editor, "Sql editor", PGM_STOCK_SQL_EDITOR );
}

static void
on_select_font_activated()
{
    GtkWidget *dialog;

    dialog = gtk_font_selection_dialog_new( "Select font..." );

    if( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_OK )
    {
        g_debug( "font: %s", gtk_font_selection_dialog_get_font_name( GTK_FONT_SELECTION_DIALOG( dialog ) ) );
    }
    gtk_widget_destroy( dialog );
}

static void
on_preferences_activated()
{
    pgm_preferences_dialog_show( GTK_WINDOW( main_window ), config );
}

GtkWidget*
pgm_main_window_get_file_chooser_dialog( const gchar *title, GtkFileChooserAction action, const gchar *first_button_text, ... )
{
    GtkWidget *dialog;
    const gchar *button_text = first_button_text;
    gint response_id;
    va_list ap;

    dialog = g_object_new( GTK_TYPE_FILE_CHOOSER_DIALOG,
                           "title", title,
                           "action", action,
                           NULL );
    gtk_window_set_transient_for( GTK_WINDOW( dialog ), GTK_WINDOW( main_window ) );

    va_start( ap, first_button_text );
    while( button_text )
    {
        response_id = va_arg( ap, gint );
        gtk_dialog_add_button( GTK_DIALOG( dialog ), button_text, response_id );
        button_text = va_arg( ap, const gchar * );
    }
    va_end( ap );

    return dialog;
}

void
pgm_main_window_add_accel_group( GtkAccelGroup *accel_group )
{
    g_return_if_fail( accel_group != NULL );

    gtk_window_add_accel_group( GTK_WINDOW( main_window ), accel_group );
}

void
pgm_main_window_start_wait()
{
    gdk_window_set_cursor( gtk_widget_get_window( main_window ), wait_cursor );
    /*gtk_widget_set_sensitive( main_window, FALSE );*/
    while( gdk_events_pending() )
        gtk_main_iteration();
}

void
pgm_main_window_stop_wait()
{
    gdk_window_set_cursor( gtk_widget_get_window( main_window ), NULL );
    /*gtk_widget_set_sensitive( main_window, TRUE );*/
    while( gdk_events_pending() )
        gtk_main_iteration();
}

void
pgm_main_window_start_wait_for_widget( GtkWidget *widget )
{
    gdk_window_set_cursor( gtk_widget_get_window( widget ), wait_cursor );
    gtk_widget_set_sensitive( widget, FALSE );
}

void
pgm_main_window_stop_wait_for_widget( GtkWidget *widget )
{
    gdk_window_set_cursor( gtk_widget_get_window( widget ), NULL );
    gtk_widget_set_sensitive( widget, TRUE );
}

GKeyFile*
pgm_main_window_get_config()
{
    return config;
}

static void
pgm_main_window_load_config()
{
    GConfClient *conf_client;
    gint x = -1, y = -1, width = 800, height = 600;
    GConfValue *value;
    GSList *list;
    GError *err = NULL;
    GtkTreePath *path;

    conf_client = gconf_client_get_default();

    value = gconf_client_get_without_default( conf_client, CONFIG_STR "/ui/window_x", NULL );
    if( value )
        x = gconf_value_get_int( value );
    value = gconf_client_get_without_default( conf_client, CONFIG_STR "/ui/window_y", NULL );
    if( value )
        y = gconf_value_get_int( value );
    value = gconf_client_get_without_default( conf_client, CONFIG_STR "/ui/window_width", NULL );
    if( value )
        width = gconf_value_get_int( value );
    value = gconf_client_get_without_default( conf_client, CONFIG_STR "/ui/window_height", NULL );
    if( value )
        height = gconf_value_get_int( value );
    if( x >= 0 && y >= 0 )
        gtk_window_move( GTK_WINDOW( main_window ), x, y );
    gtk_window_set_default_size( GTK_WINDOW( main_window ), width, height );

    value = gconf_client_get_without_default( conf_client, CONFIG_STR "/ui/window_hpaned_pos", NULL );
    if( value )
        gtk_paned_set_position( GTK_PANED( h_paned ), gconf_value_get_int( value ) );
    value = gconf_client_get_without_default( conf_client, CONFIG_STR "/ui/sort_order", NULL );
    if( value )
    {
        const gchar *order_str = gconf_value_get_string( value );
        if( order_str[0] == 'a' )
            gtk_tree_sortable_set_sort_column_id( GTK_TREE_SORTABLE( tree_store ), 1, GTK_SORT_ASCENDING );
        else
            gtk_tree_sortable_set_sort_column_id( GTK_TREE_SORTABLE( tree_store ), 1, GTK_SORT_DESCENDING );
    }

    list = gconf_client_all_dirs( conf_client, CONFIG_STR "/servers", &err );
    if( list )
    {
        GSList *cur = list;
        while( cur )
        {
            GConfValue *value;
            gchar *key;
            const gchar *host;
            gint port;
            const gchar *login;
            PgmServer *server;
            PgmSslMode ssl_mode = PGM_SSL_PREFER;

            key = g_strdup_printf( "%s/host", (gchar*)cur->data );
            value = gconf_client_get_without_default( conf_client, key, NULL );
            if( value )
                host = gconf_value_get_string( value );
            g_free( key );
            key = g_strdup_printf( "%s/port", (gchar*)cur->data );
            value = gconf_client_get_without_default( conf_client, key, NULL );
            if( value )
                port = gconf_value_get_int( value );
            g_free( key );
            key = g_strdup_printf( "%s/login", (gchar*)cur->data );
            value = gconf_client_get_without_default( conf_client, key, NULL );
            if( value )
                login = gconf_value_get_string( value );
            g_free( key );
            key = g_strdup_printf( "%s/ssl", (gchar*)cur->data );
            value = gconf_client_get_without_default( conf_client, key, NULL );
            if( value )
            {
                const gchar *ssl_str = gconf_value_get_string( value );
                if( g_strcmp0( ssl_str, "allow" ) == 0 )
                    ssl_mode = PGM_SSL_ALLOW;
                else if( g_strcmp0( ssl_str, "disable" ) == 0 )
                    ssl_mode = PGM_SSL_DISABLE;
                else if( g_strcmp0( ssl_str, "prefer" ) == 0 )
                    ssl_mode = PGM_SSL_PREFER;
                else if( g_strcmp0( ssl_str, "require" ) == 0 )
                    ssl_mode = PGM_SSL_REQUIRE;
            }
            g_free( key );

            server = pgm_server_new( server_list, host, port, login, ssl_mode, TRUE );
            pgm_object_show( PGM_OBJECT( server ) );
            key = g_strdup_printf( "%s/databases", (gchar*)cur->data );
            value = gconf_client_get_without_default( conf_client, key, NULL );
            if( value )
            {
                GSList *list = gconf_value_get_list( value );
                GSList *cur = list;
                while( cur )
                {
                    GConfValue *value = (GConfValue*)cur->data;
                    PgmDatabase *db = pgm_database_new( server, gconf_value_get_string( value ) );
                    pgm_object_show( PGM_OBJECT( db ) );
                    cur = cur->next;
                }
                g_slist_free( list );
            }
            g_free( key );

            g_free( cur->data );
            cur = cur->next;
        }
        g_slist_free( list );
    }
    path = gtk_tree_path_new_first();
    gtk_tree_view_expand_row( GTK_TREE_VIEW( tree_view ), path, FALSE );
    gtk_tree_path_free( path );
}

void
pgm_main_window_save_config()
{
    GConfClient *conf_client;
    gint x, y, width, height;
    gint i;
    gint sort_column_id;
    GtkSortType sort_order;

    conf_client = gconf_client_get_default();

    gtk_window_get_position( GTK_WINDOW( main_window ), &x, &y );
    gtk_window_get_size( GTK_WINDOW( main_window ), &width, &height );
    gtk_tree_sortable_get_sort_column_id( GTK_TREE_SORTABLE( tree_store ), &sort_column_id, &sort_order );

    gconf_client_set_int( conf_client, CONFIG_STR "/ui/window_x", x, NULL );
    gconf_client_set_int( conf_client, CONFIG_STR "/ui/window_y", y, NULL );
    gconf_client_set_int( conf_client, CONFIG_STR "/ui/window_width", width, NULL );
    gconf_client_set_int( conf_client, CONFIG_STR "/ui/window_height", height, NULL );
    gconf_client_set_int( conf_client, CONFIG_STR "/ui/window_hpaned_pos", gtk_paned_get_position( GTK_PANED( h_paned ) ), NULL );
    if( sort_order == GTK_SORT_ASCENDING )
        gconf_client_set_string( conf_client, CONFIG_STR "/ui/sort_order", "a", NULL );
    else
        gconf_client_set_string( conf_client, CONFIG_STR "/ui/sort_order", "d", NULL);

    for( i = 0; i < pgm_object_get_n_childs( PGM_OBJECT( server_list ) ); i++ )
    {
        PgmServer *server = PGM_SERVER( pgm_object_get_nth_child( PGM_OBJECT( server_list ), i ) );
        PgmObject *database;
        gint p;
        gchar *key;
        gchar *key1;
        GSList *list = NULL;

        key = g_strdup_printf( CONFIG_STR "/servers/%s", pgm_server_get_host( server ) );

        key1 = g_strdup_printf( "%s/host", key );
        gconf_client_set_string( conf_client, key1, pgm_server_get_host( server ), NULL );
        g_free( key1 );
        key1 = g_strdup_printf( "%s/port", key );
        gconf_client_set_int( conf_client, key1, pgm_server_get_port( server ), NULL );
        g_free( key1 );
        key1 = g_strdup_printf( "%s/login", key );
        gconf_client_set_string( conf_client, key1, pgm_server_get_login( server ), NULL );
        g_free( key1 );
        key1 = g_strdup_printf( "%s/ssl", key );
        switch( pgm_server_get_ssl_mode( server ) )
        {
            case PGM_SSL_ALLOW:
                gconf_client_set_string( conf_client, key1, "allow", NULL );
                break;
            case PGM_SSL_DISABLE:
                gconf_client_set_string( conf_client, key1, "disable", NULL );
                break;
            case PGM_SSL_PREFER:
                gconf_client_set_string( conf_client, key1, "prefer", NULL );
                break;
            case PGM_SSL_REQUIRE:
                gconf_client_set_string( conf_client, key1, "require", NULL );
                break;
        }
        g_free( key1 );
        key1 = g_strdup_printf( "%s/databases", key );
        for( p = 0; p < pgm_object_get_n_childs( PGM_OBJECT( server ) ); p++ )
        {
            database = pgm_object_get_nth_child( PGM_OBJECT( server ), p );
            list = g_slist_prepend( list, pgm_object_get_name( database ) );
        }
        gconf_client_set_list( conf_client, key1, GCONF_VALUE_STRING, list, NULL );
        g_free( key1 );

        g_free( key );
        g_slist_free( list );
    }
    pgm_main_window_cleanup();
}
