#ifndef PGMLANGUAGE_H
#define PGMLANGUAGE_H

#include "pgmobject.h"
#include "pgmlanguagelist.h"

#define PGM_TYPE_LANGUAGE                  (pgm_language_get_type())
#define PGM_LANGUAGE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST( (obj), PGM_TYPE_LANGUAGE, PgmLanguage ))
#define PGM_IS_LANGUAGE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE( (obj), PGM_TYPE_LANGUAGE) )
#define PGM_LANGUAGE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST( (klass), PGM_TYPE_LANGUAGE, PgmLanguageClass ))
#define PGM_IS_LANGUAGE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE( (klass), PGM_TYPE_LANGUAGE ))
#define PGM_LANGUAGE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS( (obj), PGM_TYPE_LANGUAGE, PgmLanguageClass ))

G_BEGIN_DECLS

typedef struct _PgmLanguage PgmLanguage;
typedef struct _PgmLanguageClass PgmLanguageClass;

struct _PgmLanguage
{
	PgmObject parent;
};

struct _PgmLanguageClass
{
	PgmObjectClass parent_class;
};

GType pgm_language_get_type();
PgmLanguage* pgm_language_new( PgmLanguageList *list, const gchar *name );

G_END_DECLS

#endif /* PGMLANGUAGE_H */
