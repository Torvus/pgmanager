#ifndef PGMSTOCK_H
#define PGMSTOCK_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

void pgm_stock_init();
void pgm_stock_add_pixmap_directory( const gchar *path );

#define PGM_STOCK_SERVERS                      "pgm-servers"
#define PGM_STOCK_SERVER                       "pgm-server"
#define PGM_STOCK_DATABASE                     "pgm-database"
#define PGM_STOCK_DATABASE_CLOSED              "pgm-database-closed"
#define PGM_STOCK_FUNCTIONS                    "pgm-functions"
#define PGM_STOCK_FUNCTION                     "pgm-function"
#define PGM_STOCK_TABLES                       "pgm-tables"
#define PGM_STOCK_TABLE                        "pgm-table"
#define PGM_STOCK_VIEWS                        "pgm-views"
#define PGM_STOCK_VIEW                         "pgm-view"
#define PGM_STOCK_SCHEMAS                      "pgm-schemas"
#define PGM_STOCK_SCHEMA                       "pgm-schema"
#define PGM_STOCK_KEY                          "pgm-key"
#define PGM_STOCK_KEY_PRIMARY                  "pgm-key-primary"
#define PGM_STOCK_KEY_FOREIGN                  "pgm-key-foreign"
#define PGM_STOCK_LANGUAGES                    "pgm-languages"
#define PGM_STOCK_LANGUAGE                     "pgm-language"
#define PGM_STOCK_SQL_EDITOR                   "pgm-sql-editor"
#define PGM_STOCK_EXECUTE                      "pgm-execute"

G_END_DECLS

#endif /* PGMSTOCK_H */
