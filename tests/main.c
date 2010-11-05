#include <stdlib.h>
#include <gtk/gtk.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Console.h>

#include "tests.h"

int main( int argc, char *argv[] )
{
	int failed = 0;
	gtk_set_locale ();
	gtk_init (&argc, &argv);

	if( CU_initialize_registry() )
	{
		g_critical( "Initialization of Test Registry failed." );
		return EXIT_FAILURE;
	}

	register_all_tests();
	CU_set_output_filename( "TestsResult" );
	CU_list_tests_to_file();
	CU_automated_run_tests();
	/*CU_console_run_tests();*/
	failed = CU_get_number_of_tests_failed();
	CU_cleanup_registry();

	if( failed == 0 )
		return EXIT_SUCCESS;
	return EXIT_FAILURE;
}
