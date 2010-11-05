#include "pgmmarshal.h"

#define g_marshal_value_peek_boolean(v)  (v)->data[0].v_int
#define g_marshal_value_peek_char(v)     (v)->data[0].v_int
#define g_marshal_value_peek_uchar(v)    (v)->data[0].v_uint
#define g_marshal_value_peek_int(v)      (v)->data[0].v_int
#define g_marshal_value_peek_uint(v)     (v)->data[0].v_uint
#define g_marshal_value_peek_long(v)     (v)->data[0].v_long
#define g_marshal_value_peek_ulong(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_int64(v)    (v)->data[0].v_int64
#define g_marshal_value_peek_uint64(v)   (v)->data[0].v_uint64
#define g_marshal_value_peek_enum(v)     (v)->data[0].v_long
#define g_marshal_value_peek_flags(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_float(v)    (v)->data[0].v_float
#define g_marshal_value_peek_double(v)   (v)->data[0].v_double
#define g_marshal_value_peek_string(v)   (v)->data[0].v_pointer
#define g_marshal_value_peek_param(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_boxed(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_pointer(v)  (v)->data[0].v_pointer
#define g_marshal_value_peek_object(v)   (v)->data[0].v_pointer

void pgm_marshal_BOOL_INT_INT_POINTER( GClosure     *closure,
									   GValue       *return_value,
									   guint         n_param_values,
									   const GValue *param_values,
									   gpointer      invocation_hint,
									   gpointer      marshal_data )
{
	typedef gboolean (*GMarshalFunc_BOOL_INT_INT_POINTER)( gpointer data1,
												  gint     arg_1,
												  gint     arg_2,
												  gpointer arg_3,
												  gpointer data2 );

	register GMarshalFunc_BOOL_INT_INT_POINTER callback;
	register GCClosure *cc = (GCClosure*) closure;
	register gpointer data1, data2;
	gboolean v_return;

	g_return_if_fail (return_value != NULL);
	g_return_if_fail (n_param_values == 4);

	if (G_CCLOSURE_SWAP_DATA (closure))
    {
		data1 = closure->data;
		data2 = g_value_peek_pointer (param_values + 0);
    }
	else
    {
		data1 = g_value_peek_pointer (param_values + 0);
		data2 = closure->data;
    }
	callback = (GMarshalFunc_BOOL_INT_INT_POINTER) (marshal_data ? marshal_data : cc->callback);
	v_return = callback( data1,
						 g_marshal_value_peek_int( param_values + 1 ),
						 g_marshal_value_peek_int( param_values + 2 ),
						 g_marshal_value_peek_pointer( param_values + 3 ),
						 data2 );
	g_value_set_boolean (return_value, v_return);
}

gboolean
pgm_boolean_handled_accumulator( GSignalInvocationHint *ihint,
							     GValue                *return_accu,
								 const GValue          *handler_return,
								 gpointer               dummy )
{
  gboolean continue_emission;
  gboolean signal_handled;

  signal_handled = g_value_get_boolean (handler_return);
  g_value_set_boolean (return_accu, signal_handled);
  continue_emission = !signal_handled;

  return continue_emission;
}
