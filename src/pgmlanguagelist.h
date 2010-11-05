#ifndef PGMLANGUAGELIST_H
#define PGMLANGUAGELIST_H

#include "pgmobject.h"
#include "pgmdatabase.h"

#define PGM_TYPE_LANGUAGE_LIST                    (pgm_language_list_get_type())
#define PGM_LANGUAGE_LIST(obj)                    (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_LANGUAGE_LIST, PgmLanguageList ))
#define PGM_IS_LANGUAGE_LIST(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_LANGUAGE_LIST) )
#define PGM_LANGUAGE_LIST_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_LANGUAGE_LIST, PgmLanguageListClass ))
#define PGM_IS_LANGUAGE_LIST_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_LANGUAGE_LIST ))
#define PGM_LANGUAGE_LIST_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_LANGUAGE_LIST, PgmLanguageListClass ))

G_BEGIN_DECLS

typedef struct _PgmLanguageList PgmLanguageList;
typedef struct _PgmLanguageListClass PgmLanguageListClass;

struct _PgmLanguageList
{
	PgmObject parent;
};

struct _PgmLanguageListClass
{
	PgmObjectClass parent;
};

GType pgm_language_list_get_type();
PgmLanguageList* pgm_language_list_new( PgmDatabase *db );

G_END_DECLS

#endif /* PGMLANGUAGELIST_H */
