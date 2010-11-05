#include "pgmpreferencesdialog.h"

#include "pgmtranslate.h"
#include "pgmsqleditor.h"
#include "pgmmainwindow.h"

static GtkWidget *auto_indent;
static GtkWidget *indent_on_tab;
static GtkWidget *show_line_numbers;
static GtkWidget *smart_home_end;
static GtkWidget *indent_width_editor;
static GtkWidget *insert_spaces_instead_of_tabs;
static GtkWidget *highlight_current_line;
static GtkWidget *tab_width_editor;
static GtkWidget *draw_spaces_editor;
static GtkWidget *font_editor;

static GtkWidget* get_widget()
{
    GtkWidget *widget;
    GtkWidget *table;
    GtkWidget *spacer;
    GtkWidget *indent_width;
    GtkWidget *tab_width;
    GtkWidget *tab_width_label;
    GtkWidget *indent_width_label;
    GtkWidget *draw_spaces;
    GtkWidget *draw_spaces_label;
    GtkWidget *font;
    GtkWidget *font_label;

    widget = gtk_vbox_new( FALSE, 0 );

    table = gtk_table_new( 6, 2, FALSE );
    gtk_box_pack_start( GTK_BOX( widget ), table, FALSE, FALSE, 0 );

    spacer = gtk_vbox_new( FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( widget ), spacer, TRUE, TRUE, 0 );

    font = gtk_hbox_new( FALSE, 0 );
    gtk_table_attach_defaults( GTK_TABLE( table ), font, 0, 2, 0, 1 );

    font_label = gtk_label_new( _("Font") );
    gtk_box_pack_start( GTK_BOX( font ), font_label, FALSE, FALSE, 0 );

    font_editor = gtk_font_button_new();
    gtk_box_pack_start( GTK_BOX( font ), font_editor, TRUE, TRUE, 1 );

    auto_indent = gtk_check_button_new_with_label( _("Auto indent") );
    gtk_table_attach_defaults( GTK_TABLE( table ), auto_indent, 0, 1, 1, 2 );

    indent_on_tab = gtk_check_button_new_with_label( _("Indent on tab") );
    gtk_table_attach_defaults( GTK_TABLE( table ), indent_on_tab, 1, 2, 1, 2 );

    show_line_numbers = gtk_check_button_new_with_label( _("Show line numbers") );
    gtk_table_attach_defaults( GTK_TABLE( table ), show_line_numbers, 0, 1, 2, 3 );

    smart_home_end = gtk_check_button_new_with_label( _("Smart home/end") );
    gtk_table_attach_defaults( GTK_TABLE( table ), smart_home_end, 1, 2, 2, 3 );

    highlight_current_line = gtk_check_button_new_with_label( _("Highlight current line") );
    gtk_table_attach_defaults( GTK_TABLE( table ), highlight_current_line, 0, 1, 3, 4 );

    insert_spaces_instead_of_tabs = gtk_check_button_new_with_label( _("Insert spaces instead of tabs") );
    gtk_table_attach_defaults( GTK_TABLE( table ), insert_spaces_instead_of_tabs, 1, 2, 3, 4 );

    indent_width = gtk_hbox_new( FALSE, 0 );
    gtk_table_attach_defaults( GTK_TABLE( table ), indent_width, 0, 1, 4, 5 );

    indent_width_label = gtk_label_new( _("Indent width") );
    gtk_box_pack_start( GTK_BOX( indent_width ), indent_width_label, FALSE, FALSE, 0 );

    indent_width_editor = gtk_spin_button_new_with_range( 0, 999999999, 1 );
    gtk_box_pack_start( GTK_BOX( indent_width ), indent_width_editor, TRUE, TRUE, 1 );

    tab_width = gtk_hbox_new( FALSE, 0 );
    gtk_table_attach_defaults( GTK_TABLE( table ), tab_width, 1, 2, 4, 5 );

    tab_width_label = gtk_label_new( _("Tab width") );
    gtk_box_pack_start( GTK_BOX( tab_width ), tab_width_label, FALSE, FALSE, 0 );

    tab_width_editor = gtk_spin_button_new_with_range( 0, 999999999, 1 );
    gtk_box_pack_start( GTK_BOX( tab_width ), tab_width_editor, TRUE, TRUE, 1 );

    draw_spaces = gtk_hbox_new( FALSE, 0 );
    gtk_table_attach_defaults( GTK_TABLE( table ), draw_spaces, 0, 1, 5, 6 );

    draw_spaces_label = gtk_label_new( _("Draw spaces mode") );
    gtk_box_pack_start( GTK_BOX( draw_spaces ), draw_spaces_label, FALSE, FALSE, 0 );

    draw_spaces_editor = gtk_combo_box_new_text();
    gtk_combo_box_append_text( GTK_COMBO_BOX( draw_spaces_editor ), _("No") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( draw_spaces_editor ), _("Space") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( draw_spaces_editor ), _("Tab") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( draw_spaces_editor ), _("New line") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( draw_spaces_editor ), _("Non-breaking") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( draw_spaces_editor ), _("Leading") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( draw_spaces_editor ), _("Inside text") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( draw_spaces_editor ), _("Trailing") );
    gtk_combo_box_append_text( GTK_COMBO_BOX( draw_spaces_editor ), _("All") );
    gtk_combo_box_set_active( GTK_COMBO_BOX( draw_spaces_editor ), 0 );
    gtk_box_pack_start( GTK_BOX( draw_spaces ), draw_spaces_editor, TRUE, TRUE, 0 );

    return widget;
}

static void
load_config( GKeyFile *file )
{
    GKeyFile *config = pgm_main_window_get_config();

    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "AutoIndent", NULL ) )
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( auto_indent ),
                                      g_key_file_get_boolean( config,
                                                              PGM_SQL_EDITOR_CONFIG_GROUP,
                                                              "AutoIndent",
                                                              NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "IndentOnTab", NULL ) )
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( indent_on_tab ),
                                      g_key_file_get_boolean( config,
                                                              PGM_SQL_EDITOR_CONFIG_GROUP,
                                                              "IndentOnTab",
                                                              NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "ShowLineNumbers", NULL ) )
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( show_line_numbers ),
                                      g_key_file_get_boolean( config,
                                                              PGM_SQL_EDITOR_CONFIG_GROUP,
                                                              "ShowLineNumbers",
                                                              NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "SmartHomeEnd", NULL ) )
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( smart_home_end ),
                                      g_key_file_get_boolean( config,
                                                              PGM_SQL_EDITOR_CONFIG_GROUP,
                                                              "SmartHomeEnd",
                                                              NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "InsertSpacesInsteadOfTab", NULL ) )
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( insert_spaces_instead_of_tabs ),
                                      g_key_file_get_boolean( config,
                                                              PGM_SQL_EDITOR_CONFIG_GROUP,
                                                              "InsertSpacesInsteadOfTab",
                                                              NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "HighlightCurrentLine", NULL ) )
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( highlight_current_line ),
                                      g_key_file_get_boolean( config,
                                                              PGM_SQL_EDITOR_CONFIG_GROUP,
                                                              "HighlightCurrentLine",
                                                              NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "IndentWidth", NULL ) )
        gtk_spin_button_set_value( GTK_SPIN_BUTTON( indent_width_editor ),
                                   g_key_file_get_integer( config,
                                                           PGM_SQL_EDITOR_CONFIG_GROUP,
                                                           "IndentWidth",
                                                           NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "TabWidth", NULL ) )
        gtk_spin_button_set_value( GTK_SPIN_BUTTON( tab_width_editor ),
                                   g_key_file_get_integer( config,
                                                           PGM_SQL_EDITOR_CONFIG_GROUP,
                                                           "TabWidth",
                                                           NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "Font", NULL ) )
        gtk_font_button_set_font_name( GTK_FONT_BUTTON( font_editor ),
                                       g_key_file_get_string( config,
                                                              PGM_SQL_EDITOR_CONFIG_GROUP,
                                                              "Font",
                                                              NULL ) );
    if( g_key_file_has_key( config, PGM_SQL_EDITOR_CONFIG_GROUP, "DrawSpaces", NULL ) )
        gtk_combo_box_set_active( GTK_COMBO_BOX( draw_spaces_editor ),
                                  g_key_file_get_integer( config,
                                                          PGM_SQL_EDITOR_CONFIG_GROUP,
                                                          "DrawSpaces",
                                                          NULL ) );
}

static void
save_config( GKeyFile *file )
{
    GKeyFile *config = pgm_main_window_get_config();

    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "AutoIndent",
                            gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( auto_indent ) ) );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "IndentOnTab",
                            gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( indent_on_tab ) ) );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "ShowLineNumbers",
                            gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( show_line_numbers ) ) );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "SmartHomeEnd",
                            gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( smart_home_end ) ) );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "InsertSpacesInsteadOfTab",
                            gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( insert_spaces_instead_of_tabs ) ) );
    g_key_file_set_boolean( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "HighlightCurrentLine",
                            gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( highlight_current_line ) ) );
    g_key_file_set_integer( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "IndentWidth",
                            gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON( indent_width_editor ) ) );
    g_key_file_set_integer( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "TabWidth",
                            gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON( tab_width_editor ) ) );
    g_key_file_set_string( config,
                           PGM_SQL_EDITOR_CONFIG_GROUP,
                           "Font",
                           gtk_font_button_get_font_name( GTK_FONT_BUTTON( font_editor ) ) );
    g_key_file_set_integer( config,
                            PGM_SQL_EDITOR_CONFIG_GROUP,
                            "DrawSpaces",
                            gtk_combo_box_get_active( GTK_COMBO_BOX( draw_spaces_editor ) ) );
    pgm_sql_editor_reload_config();
}

PGM_REGISTER_PREFERENCE( _("SQL editor"), get_widget, load_config, save_config )
