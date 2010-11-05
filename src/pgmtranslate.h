#ifndef PGMTRANSLATE_H
#define PGMTRANSLATE_H

#include "config.h"

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else /* gettext_noop */
#    define N_(String) (String)
#  endif /* gettext_noop */
#else /* ENABLE_NLS */
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif /* ENABLE_NLS */

#endif /* PGMTRANSLATE_H */
