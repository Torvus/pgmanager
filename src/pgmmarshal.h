#ifndef PGMMARSHAL_H
#define PGMMARSHAL_H

#include <glib-object.h>

G_BEGIN_DECLS

extern void pgm_marshal_BOOL_INT_INT_POINTER( GClosure     *closure,
											  GValue       *return_value,
											  guint         n_param_values,
											  const GValue *param_values,
											  gpointer      invocation_hint,
											  gpointer      marshal_data );

gboolean pgm_boolean_handled_accumulator( GSignalInvocationHint *ihint,
										  GValue                *return_accu,
										  const GValue          *handler_return,
										  gpointer               dummy );
G_END_DECLS

#endif /* PGMMARSHAL_H */
