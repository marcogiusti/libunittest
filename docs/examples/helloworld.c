/**
 * Classic hello world example
 * ===========================
 *
 * Be sure you have libunittest installed and compile this file with the
 * following command:
 *
 *		gcc -ldl -rdynamic -lunittest -o helloworld helloworld.c
 *
 * If you run the executable you get:
 *
 *		$ ./helloworld
 *		1..1
 *		ok test_success # hello world
 *
 * One test case is executed: "test_success" and the test succeed "ok...".
 *
 * How to create your test cases
 * =============================
 *
 * A test case is a `void (*test_name)(TESTARGS, void *usrptr)` function. The
 * TESTARGS macro is needed by the assertions, and `usrptr` could be user by
 * the user to pass arbitrary to the test case. The SUCCESS macro in the body
 * is the simplest assertion, one that always succeed.
 *
 * The next step is to add that function to a test suite. The `load_test_suite`
 * create the suite and add our test. Note that the function declaration is
 * special, you cannot change it (not in this helloworld example).
 * For every new test you must add it to the suite.
 *
 * The main function call an utility function. The `test_main3`, I could change
 * the name in the future, accept the standard `argc` and `argv` arguments. You
 * could pass the main's `argc` and `argv` or you can set it manually just like
 * in this example. The `test_main3` (did I say that I could change the name?)
 * look for the `load_test_suite` function and run the resulting test suite.
 */

#include <unittest.h>


static void
test_success(TESTARGS, void *usrptr)
{
	SUCCESS("hello world");
}

struct test_suite*
load_test_suite(struct test_loader *loader)
{
	struct test_suite *suite;

	suite = test_suite_new();
	suite->add_test(suite, test_case_new(test_success));
	return suite;
}

int
main(int argc, char *argv[])
{
	char *args[] = { argv[0], NULL};

	return test_main3(1, args);
}
