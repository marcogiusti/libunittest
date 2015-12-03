/**
 * Be sure you read the helloworld.c, fail.c and firstfail.c examples.
 *
 * This example shows how the failfast options works, here passed directly to
 * `test_main3()` as a -f flag. There are two tests, the first one fails, all
 * the others are skipped and the program terminate.
 *
 * Here is the output:
 *
 *		1..2
 *		not ok test_fail # hello fail
 *
 * As you can see only one of the two tests are executed.
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
	suite->add_test(suite, test_case_new(test_fail));
	suite->add_test(suite, test_case_new(test_success));
	return suite;
}

int
main(int argc, char *argv[])
{
	char *args[] = { argv[0], "-f", NULL};

	return test_main3(2, args);
}
