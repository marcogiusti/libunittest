/**
 * This is the natuaral evolution of the helloworld.c example. Be sure you read
 * it. This second example simply add a second test that, alas, fails.
 * Interesting here is the exit status of the program: 1. This indicate a
 * generic failure.
 */

#include <unittest.h>


static void
test_success(TESTARGS, void *usrptr)
{
	SUCCESS("hello world");
}

static void
test_fail(TESTARGS, void *usrptr)
{
	FAIL("hello fail");
}

struct test_suite*
load_test_suite(struct test_loader *loader)
{
	struct test_suite *suite;

	suite = test_suite_new();
	suite->add_test(suite, test_case_new(test_success));
	suite->add_test(suite, test_case_new(test_fail));
	return suite;
}

int
main(int argc, char *argv[])
{
	char *args[] = { argv[0], NULL};

	return test_main3(1, args);
}
