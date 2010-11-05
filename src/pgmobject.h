#ifndef PGMOBJECT_H
#define PGMOBJECT_H

#include <gtk/gtk.h>

#define PGM_TYPE_OBJECT                    (pgm_object_get_type())
#define PGM_OBJECT(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_OBJECT, PgmObject ))
#define PGM_IS_OBJECT(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_OBJECT) )
#define PGM_OBJECT_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_OBJECT, PgmObjectClass ))
#define PGM_IS_OBJECT_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_OBJECT ))
#define PGM_OBJECT_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_OBJECT, PgmObjectClass ))

typedef struct _PgmObject           PgmObject;
typedef struct _PgmObjectClass      PgmObjectClass;

G_BEGIN_DECLS

struct _PgmObject
{
	GInitiallyUnowned parent_instance;

	/* private */
	PgmObject *parent;
	GList *child_list;

	gchar *name;
	gchar *stock_id;
	gboolean showed;
	gboolean real_showed;
	gboolean show_child_count;
	GtkTreeIter iter;

};

struct _PgmObjectClass
{
	GInitiallyUnownedClass parent;

	/*< signals >*/
	void (*expanded) ( PgmObject *object );

	/*< virtual >*/
	void (*activate) ( PgmObject *object );
	PgmObject* (*get_database) ( PgmObject *object );
	PgmObject* (*get_schema) ( PgmObject *object );
	void (*real_shown) ( PgmObject *object );
};

GType         pgm_object_get_type      ( void );
void          pgm_object_set_parent    ( PgmObject *object, PgmObject *parent );
void          pgm_object_set_name      ( PgmObject *object, const gchar *name );
void          pgm_object_set_icon      ( PgmObject *object, const gchar *stock_id );
PgmObject *   pgm_object_get_parent    ( PgmObject *object );
gchar*        pgm_object_get_name      ( PgmObject *object );
gchar*        pgm_object_get_icon      ( PgmObject *object );
void          pgm_object_show          ( PgmObject *object );
void          pgm_object_append_child  ( PgmObject *object, PgmObject *child );
GtkTreePath * pgm_object_get_tree_path ( PgmObject *object );
gint          pgm_object_get_index     ( PgmObject *object );
PgmObject *   pgm_object_get_nth_child ( PgmObject *object, gint n );
gint          pgm_object_get_n_childs  ( PgmObject *object );
void          pgm_object_activate      ( PgmObject *object );
PgmObject *   pgm_object_get_database  ( PgmObject *object );
PgmObject *   pgm_object_get_schema    ( PgmObject *object );
void          pgm_object_expanded      ( PgmObject *object );

G_END_DECLS

#endif /* PGMOBJECT_H */
