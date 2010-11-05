#ifndef TESTS_H
#define TESTS_H

#include <gtk/gtk.h>
#include <CUnit/CUnit.h>

typedef struct
{
	gchar *suite_name;
	gchar *test_name;
	CU_TestFunc test_func;
} PgmTest;

void register_all_tests();

#define PGM_TEST( _suite_name, _test_func ) \
	extern GList *test_list; \
	static void __attribute__((constructor)) _test_func##_register() \
	{ \
		PgmTest *test = g_new( PgmTest, 1 ); \
		test->suite_name = g_strdup( _suite_name ); \
		test->test_name = g_strdup( #_test_func ); \
		test->test_func = _test_func; \
		test_list = g_list_append( test_list, test ); \
	}

#endif /* TESTS_H */
