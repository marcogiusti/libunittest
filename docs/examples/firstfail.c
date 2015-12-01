#include <unittest.h>


static void
first_fail(TESTARGS, void *usrptr)
{
	FAIL("error");
	SUCCESS("this is not executed");
}

struct test_suite*
load_test_suite(struct test_loader *loader)
{
	struct test_suite *suite;

	suite = test_suite_new();
	suite->add_test(suite, test_case_new(first_fail));
	return suite;
}

int
main(int argc, char *argv[])
{
	char *args[] = { argv[0], NULL};

	return test_main3(1, args);
}
