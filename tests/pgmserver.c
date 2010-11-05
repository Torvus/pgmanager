#include "../src/pgmserver.h"
#include "../src/pgmstock.h"
#include "tests.h"

static void
pgm_server_test()
{
	PgmServerList *list;
	PgmServer *server;

	list = pgm_server_list_new();
	CU_ASSERT_PTR_NOT_NULL_FATAL( list );
	CU_ASSERT( PGM_IS_OBJECT( list ) );
	CU_ASSERT( PGM_IS_SERVER_LIST( list ) );

	server = pgm_server_new( list, "localhost", 5432, "dummy_login", PGM_SSL_REQUIRE, TRUE );
	CU_ASSERT_PTR_NOT_NULL_FATAL( server );
	CU_ASSERT( PGM_IS_OBJECT( server ) );
	CU_ASSERT( PGM_IS_SERVER( server ) );
	CU_ASSERT_STRING_EQUAL( pgm_server_get_host( server ), "localhost" );
	CU_ASSERT( pgm_server_get_port( server ) == 5432 );
	CU_ASSERT_STRING_EQUAL( pgm_server_get_login( server ), "dummy_login" );
	CU_ASSERT( pgm_server_get_ssl_mode( server ) == PGM_SSL_REQUIRE );
	CU_ASSERT( pgm_object_get_n_childs( PGM_OBJECT( list ) ) == 1 );
	CU_ASSERT_STRING_EQUAL( pgm_object_get_icon( PGM_OBJECT( server ) ), PGM_STOCK_SERVER );
	/*CU_ASSERT_PTR_EQUAL( pgm_object_get_nth_child( PGM_OBJECT( list ), 0 ), PGM_OBJECT( server ) );*/

	g_object_unref( server );
	g_object_unref( list );
}

PGM_TEST( "Pgm", pgm_server_test )
