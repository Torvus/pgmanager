#include "tests.h"

GList *test_list = NULL;

static void register_test( gpointer data, gpointer user_data );

void register_all_tests()
{
	CU_pTestRegistry registry;
	registry = CU_get_registry();
	if( !registry )
		return;
	g_list_foreach( test_list, register_test, registry );
}

static void
register_test( gpointer data, gpointer user_data )
{
	PgmTest *test = data;
	CU_pTestRegistry registry = user_data;
	CU_pSuite suite;

	suite = CU_get_suite_by_name( test->suite_name, registry );
	if( !suite )
		suite = CU_add_suite( test->suite_name, NULL, NULL );
	CU_add_test( suite, test->test_name, test->test_func );
}

static void __attribute__((destructor))
cleanup_tests ()
{
	PgmTest *test;
	GList *cur = g_list_first( test_list );

	while( cur )
	{
		test = (PgmTest*)cur->data;
		g_free( test->suite_name );
		g_free( test->test_name );
		g_free( test );
		cur = g_list_next( cur );
	}
	g_list_free( test_list );
}
