#include "pgmobject.h"

#include "pgmmainwindow.h"

#define PGM_PARAM_READWRITE G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS

G_DEFINE_TYPE( PgmObject, pgm_object, G_TYPE_INITIALLY_UNOWNED )

static void pgm_object_unref_child( gpointer data, gpointer user_data );
static void pgm_object_dispose( GObject *object );
static void pgm_object_finalize( GObject *object );
static void pgm_object_remove_child( PgmObject *object, PgmObject *child );
static void pgm_object_set_show_child_count( PgmObject *object, gboolean show );
static void pgm_object_on_activate( PgmObject *object );
static PgmObject* pgm_object_on_get_database( PgmObject *object );
static PgmObject* pgm_object_on_get_schema( PgmObject *object );
static void pgm_object_set_property( GObject                  *object,
									 guint                     param_id,
									 const GValue             *value,
									 GParamSpec               *pspec );
static void pgm_object_get_property( GObject                  *object,
									 guint                     param_id,
									 GValue                   *value,
									 GParamSpec               *pspec );
static gboolean pgm_object_expanded_idle( gpointer data );
static gchar * gen_title( PgmObject *object )
{
	gint n_childs = pgm_object_get_n_childs( object );
	if( n_childs == 0 )
		return g_strdup( "" );
	return g_strdup_printf( "(%d)", n_childs );
}

enum
{
	PROP_0,
	PROP_NAME,
	PROP_ICON,
	PROP_PARENT,
	PROP_SHOW_CHILD_COUNT
};

enum
{
	EXPANDED,
	LAST_SIGNAL
};

static guint signals[ LAST_SIGNAL ] = {0};

static void
pgm_object_class_init( PgmObjectClass *klass )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( klass );

	klass->activate = pgm_object_on_activate;
	klass->get_database = pgm_object_on_get_database;
	klass->get_schema = pgm_object_on_get_schema;
	klass->real_shown = NULL;

	gobject_class->dispose = pgm_object_dispose;
	gobject_class->finalize = pgm_object_finalize;
	gobject_class->set_property = pgm_object_set_property;
	gobject_class->get_property = pgm_object_get_property;

	signals[ EXPANDED ] =
			g_signal_new( "expanded",
						  G_OBJECT_CLASS_TYPE( gobject_class ),
						  G_SIGNAL_RUN_LAST,
						  G_STRUCT_OFFSET( PgmObjectClass, expanded ),
						  NULL, NULL,
						  g_cclosure_marshal_VOID__VOID,
						  G_TYPE_NONE, 0 );

	g_object_class_install_property( gobject_class,
                                     PROP_NAME,
                                     g_param_spec_string( "name",
                                                          "Object name",
                                                          "Name for the object",
                                                          NULL,
                                                          PGM_PARAM_READWRITE ) );

	g_object_class_install_property( gobject_class,
                                     PROP_ICON,
                                     g_param_spec_string( "icon",
                                                          "Object icon",
                                                          "Icon for the object",
                                                          NULL,
                                                          PGM_PARAM_READWRITE ) );

	g_object_class_install_property( gobject_class,
                                     PROP_PARENT,
                                     g_param_spec_object( "parent",
                                                          "Object parent",
                                                          "Parent object for the object",
                                                          PGM_TYPE_OBJECT,
                                                          PGM_PARAM_READWRITE ) );
	g_object_class_install_property( gobject_class,
                                     PROP_SHOW_CHILD_COUNT,
                                     g_param_spec_boolean( "show-child-count",
                                                           "Show child count",
                                                           "Show child count",
                                                           TRUE,
                                                           PGM_PARAM_READWRITE ) );
}

static void
pgm_object_init( PgmObject *object )
{
	object->parent = NULL;
	object->child_list = NULL;
	object->name = NULL;
	object->stock_id = NULL;
	object->showed = FALSE;
	object->real_showed = FALSE;
	object->show_child_count = TRUE;
}

static void
pgm_object_unref_child( gpointer data, gpointer user_data )
{
	g_object_unref( G_OBJECT( data ) );
}

static void
pgm_object_dispose( GObject *object )
{
	PgmObject *self = PGM_OBJECT( object );

	g_list_foreach( self->child_list, pgm_object_unref_child, NULL );
	if( self->showed )
		gtk_tree_store_remove( pgm_main_window_get_tree_store(), &self->iter );
	if( self->parent )
		pgm_object_remove_child( self->parent, self );

	G_OBJECT_CLASS( pgm_object_parent_class )->dispose( object );
}

static void
pgm_object_finalize( GObject *object )
{
	PgmObject *self = PGM_OBJECT( object );

	if( self->name )
		g_free( self->name );
	if( self->stock_id )
		g_free( self->stock_id );

	g_list_free( self->child_list );

	G_OBJECT_CLASS( pgm_object_parent_class )->finalize( object );
}

static void
pgm_object_set_property( GObject                  *object,
						 guint                     param_id,
						 const GValue             *value,
						 GParamSpec               *pspec )
{
	PgmObject *pobject = PGM_OBJECT( object );

	switch( param_id )
	{
		case PROP_NAME:
			pgm_object_set_name( pobject, g_value_get_string( value ) );
			break;
		case PROP_ICON:
			pgm_object_set_icon( pobject, g_value_get_string( value ) );
			break;
		case PROP_PARENT:
			pgm_object_append_child( PGM_OBJECT( g_value_get_object( value ) ), pobject );
			break;
		case PROP_SHOW_CHILD_COUNT:
			pgm_object_set_show_child_count( pobject, g_value_get_boolean( value ) );
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
			break;
	}
}

static void
pgm_object_get_property( GObject                  *object,
						 guint                     param_id,
						 GValue                   *value,
						 GParamSpec               *pspec )
{
	PgmObject *pobject = PGM_OBJECT( object );

	switch( param_id )
	{
		case PROP_NAME:
			g_value_set_string( value, pgm_object_get_name( pobject ) );
			break;
		case PROP_ICON:
			g_value_set_string( value, pgm_object_get_icon( pobject ) );
			break;
		case PROP_PARENT:
			g_value_set_object( value, pgm_object_get_parent( pobject ) );
			break;
		case PROP_SHOW_CHILD_COUNT:
			g_value_set_boolean( value, pobject->show_child_count );
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
			break;
	}
}

void
pgm_object_append_child( PgmObject *object, PgmObject *child )
{
	g_return_if_fail( object != NULL );
	g_return_if_fail( child != NULL );
	g_return_if_fail( child->parent != object );

	if( child->parent )
		pgm_object_remove_child( child->parent, child );

	child->parent = object;
	object->child_list = g_list_append( object->child_list, child );

	g_object_ref_sink( G_OBJECT( child ) );

	if( object->showed && object->show_child_count )
	{
		GtkTreeStore *store;
		gchar *title;
		title = gen_title( object );
		store = pgm_main_window_get_tree_store();
		if( gtk_tree_store_iter_is_valid( store, &object->iter ) )
			gtk_tree_store_set( store, &object->iter, 2, title, -1 );
		g_free( title );
	}
}

static void
pgm_object_remove_child( PgmObject *object, PgmObject *child )
{
	g_return_if_fail( object != NULL );
	g_return_if_fail( child != NULL );
	g_return_if_fail( child->parent == object );

	child->parent = NULL;
	object->child_list = g_list_remove( object->child_list, child );
}

static void
pgm_object_set_show_child_count( PgmObject *object, gboolean show )
{
	object->show_child_count = show;
	if( object->showed )
	{
		GtkTreeStore *store;
		store = pgm_main_window_get_tree_store();

		if( show )
		{
			gchar *title;
			title = gen_title( object );
			gtk_tree_store_set( store, &object->iter, 2, title, -1 );
			g_free( title );
		}
		else
			gtk_tree_store_set( store, &object->iter, 2, NULL, -1 );
	}
}

void
pgm_object_set_parent ( PgmObject *object, PgmObject *parent )
{
	g_return_if_fail( object != NULL );

	if( object->parent )
		pgm_object_remove_child( object->parent, object );

	if( parent )
		pgm_object_append_child( parent, object );
}

void
pgm_object_set_name( PgmObject *object, const gchar *name )
{
	GtkTreeStore *store;

	g_return_if_fail( object != NULL );

	if( object->name )
		g_free( object->name );
	object->name = g_strdup( name );

	store = pgm_main_window_get_tree_store();
	if( gtk_tree_store_iter_is_valid( store, &object->iter ) )
		gtk_tree_store_set( store, &object->iter, 1, name, -1 );
}

void
pgm_object_set_icon( PgmObject *object, const gchar *stock_id )
{
	GtkTreeStore *store;

	g_return_if_fail( object != NULL );

	if( object->stock_id )
		g_free( object->stock_id );
	object->stock_id = g_strdup( stock_id );

	store = pgm_main_window_get_tree_store();
	if( gtk_tree_store_iter_is_valid( store, &object->iter ) )
		gtk_tree_store_set( store, &object->iter, 0, pgm_main_window_render_icon( stock_id ), -1 );
}

PgmObject *
pgm_object_get_parent( PgmObject *object )
{
	g_return_val_if_fail( object != NULL, NULL );

	return object->parent;
}

gchar*
pgm_object_get_name( PgmObject *object )
{
	g_return_val_if_fail( object != NULL, NULL );
	return object->name;
}

gchar*
pgm_object_get_icon( PgmObject *object )
{
	g_return_val_if_fail( object != NULL, NULL );
	return object->stock_id;
}

void
pgm_object_show( PgmObject *object )
{
	PgmObject *parent;
	GdkPixbuf *pixbuf;
	GtkTreeStore *store;
	gchar *title;

	g_return_if_fail( object != NULL );

	if( object->parent )
		parent = object->parent;
	else
		parent = NULL;

	if( object->showed )
		return;

	if( parent && !parent->showed )
		pgm_object_show( parent );

	store = pgm_main_window_get_tree_store();
	if( parent )
		gtk_tree_store_append( store, &object->iter, &parent->iter );
	else
		gtk_tree_store_append( store, &object->iter, NULL );

	if( object->stock_id )
	{
		pixbuf = pgm_main_window_render_icon( object->stock_id );
		if( !pixbuf )
			g_warning( "Couldn't render icon" );
		gtk_tree_store_set( store, &object->iter, 0, pixbuf, -1 );
	}

	gtk_tree_store_set( store, &object->iter, 1, object->name, -1 );
	if( object->show_child_count )
	{
		title = gen_title( object );
		gtk_tree_store_set( store, &object->iter, 2, title, -1 );
		g_free( title );
	}

	object->showed = TRUE;
}

GtkTreePath *
pgm_object_get_tree_path( PgmObject *object )
{
	GtkTreeStore *store;

	g_return_val_if_fail( object != NULL, NULL );

	store = pgm_main_window_get_tree_store();
	return gtk_tree_model_get_path( GTK_TREE_MODEL( store ), &object->iter );
}

gint
pgm_object_get_index( PgmObject *object )
{
	GtkTreePath *path;
	gint depth;
	gint *indices;

	g_return_val_if_fail( object != NULL, -1 );

	path = pgm_object_get_tree_path( object );
	depth = gtk_tree_path_get_depth( path );
	indices = gtk_tree_path_get_indices( path );
	if( depth == 0 )
	{
		g_debug( "name = %s, showed = %s, depth == 0", object->name, ( object->showed ? "TRUE" : "FALSE" ) );
		return -1;
	}

	return indices[ depth-1 ];
}

PgmObject *
pgm_object_get_nth_child( PgmObject *object, gint n )
{
	GList *child_node;
	PgmObject *child;
	gint index;

	g_return_val_if_fail( object != NULL, NULL );

	child_node = g_list_first( object->child_list );

	while( child_node != NULL )
	{
		child = PGM_OBJECT( child_node->data );
		index = pgm_object_get_index( child );
		if( index == n )
			return child;
		child_node = g_list_next( child_node );
	}
	return NULL;
}

gint
pgm_object_get_n_childs( PgmObject *object )
{
	g_return_val_if_fail( object != NULL, 0 );

	return g_list_length( object->child_list );
}

void
pgm_object_activate( PgmObject *object )
{
	PgmObjectClass *klass;

	g_return_if_fail( object != NULL );

	klass = PGM_OBJECT_GET_CLASS( object );

	if( klass->activate )
		klass->activate( object );
}

static void
pgm_object_on_activate( PgmObject *object )
{
	GtkTreePath *path;

	g_return_if_fail( object != NULL );

	path = pgm_object_get_tree_path( object );

	if( pgm_main_window_row_expanded( path ) )
		pgm_main_window_collapse_row( path );
	else
		pgm_main_window_expand_row( path, FALSE );
}

PgmObject *
pgm_object_get_database( PgmObject *object )
{
	PgmObjectClass *klass;

	g_return_val_if_fail( object != NULL, NULL );

	klass = PGM_OBJECT_GET_CLASS( object );

	if( klass->get_database )
		return klass->get_database( object );
	return NULL;
}

static PgmObject*
pgm_object_on_get_database( PgmObject *object )
{
	g_return_val_if_fail( object != NULL, NULL );

	if( object->parent )
		return pgm_object_get_database( object->parent );
	return NULL;
}

PgmObject *
pgm_object_get_schema( PgmObject *object )
{
	PgmObjectClass *klass;

	g_return_val_if_fail( object != NULL, NULL );

	klass = PGM_OBJECT_GET_CLASS( object );

	if( klass->get_schema )
		return klass->get_schema( object );
	return NULL;
}

static PgmObject*
pgm_object_on_get_schema( PgmObject *object )
{
	g_return_val_if_fail( object != NULL, NULL );

	if( object->parent )
		return pgm_object_get_schema( object->parent );
	return NULL;
}

void
pgm_object_expanded( PgmObject *object )
{
	/*GList *child_node;
	PgmObject *child;
	PgmObjectClass *child_class;*/

	g_return_if_fail( object != NULL );

	/*pgm_main_window_start_wait();*/

	g_signal_emit( object, signals[ EXPANDED ], 0 );

	/*child_node = g_list_first( object->child_list );
	while( child_node != NULL )
	{
		child = PGM_OBJECT( child_node->data );
		child_class = PGM_OBJECT_GET_CLASS( child );
		if( !child->real_showed && child_class->real_shown )
		{
			child_class->real_shown( child );
			child->real_showed = TRUE;
		}
		child_node = g_list_next( child_node );
	}
	pgm_main_window_stop_wait();*/
	g_idle_add( pgm_object_expanded_idle, object->child_list );
}

static GList *child_node = NULL;

static gboolean
pgm_object_expanded_idle( gpointer data )
{
	PgmObject *child;
	PgmObjectClass *child_class;

	if( data == NULL )
		return FALSE;

	if( child_node == NULL )
		child_node = data;

	child = PGM_OBJECT( child_node->data );
	child_class = PGM_OBJECT_GET_CLASS( child );
	if( !child->real_showed && child_class->real_shown )
	{
		child_class->real_shown( child );
		child->real_showed = TRUE;
	}
	child_node = g_list_next( child_node );
	return child_node != NULL;
}
