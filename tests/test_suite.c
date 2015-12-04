#include <stdlib.h>
#include <assert.h>
#include "unittest.h"
#include "unittest_priv.h"


static void
test_new_suite(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr;

	ASSERT_PTR_NULL(suite->name, "name must be NULL");
	ASSERT_PTR_NULL(suite->doc, "doc must be NULL");
	ASSERT_PTR_NULL(suite->skip, "_skip_ must be NULL");
	ASSERT_PTR_NULL(suite->usrptr, "usrptr must be NULL");
	ASSERT_PTR_NOT_NULL(suite->free, "free cannot be NULL");
	ASSERT_PTR_NOT_NULL(suite->add_test, "add_test cannot be NULL");
	ASSERT_PTR_NOT_NULL(suite->add_suite, "add_suite cannot be NULL");
	ASSERT_PTR_NOT_NULL(suite->run, "run cannot be NULL");
	ASSERT_PTR_NOT_NULL(suite->len, "len cannot be NULL");
}

static void
test_len0(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr;

	ASSERT_EQUAL(suite->len(suite), 0, "No tests are added to the suite.");
}

static void
test_add_test(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr;

	suite->add_test(suite, test_case_new(NULL));
	ASSERT_EQUAL(suite->len(suite), 1, "Just added one test.");
}

static void
test_add_suite0(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr;

	suite->add_suite(suite, test_suite_new());
	ASSERT_EQUAL(suite->len(suite), 0, "The new suite has no tests.");
}

static void
test_add_suite1(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr,
					  *suite_c;

	suite_c = test_suite_new();
	suite_c->add_test(suite_c, test_case_new(NULL));
	suite->add_suite(suite, suite_c);
	ASSERT_EQUAL(suite->len(suite), 1, "The new suite has just one test.");
}

static void
test_len1(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr,
					  *suite1,
					  *suite2;


	suite1 = test_suite_new();
	suite1->add_test(suite1, test_case_new(NULL));
	suite->add_suite(suite, suite1);
	suite2 = test_suite_new();
	suite2->skip = "test skip";
	suite2->add_test(suite2, test_case_new(NULL));
	suite->add_suite(suite, suite2);
	ASSERT_EQUAL(suite->len(suite), 1,
			"Test from a skipped suite should not be counted");
}

static void
test_run_tests0(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr;
	struct test_result *myres;

	myres = tap_result_new(false, NULL);
	suite->run(suite, myres);
	ASSERT_EQUAL(myres->was_successful(myres), 0, "No tests no failures");
	myres->free(myres);
}

static void
_test_success(TESTARGS, void *usrptr)
{
	SUCCESS("urra'");
}

static void
test_run_tests1(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr;
	struct test_result *myres;

	myres = tap_result_new(false, NULL);
	suite->add_test(suite, test_case_new(_test_success));
	suite->run(suite, myres);
	ASSERT_EQUAL(myres->was_successful(myres), 0, "The test succeed.");
	myres->free(myres);
}

static void
_test_fail(TESTARGS, void *usrptr)
{
	FAIL("buu");
}

static void
test_run_tests2(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr,
					  *suite1,
					  *suite2;
	struct test_result *myres;

	/* Add two suites: one fails and one succeed. */
	suite1 = test_suite_new();
	suite1->add_test(suite1, test_case_new(_test_fail));
	suite->add_suite(suite, suite1);
	suite2 = test_suite_new();
	suite2->add_test(suite2, test_case_new(_test_success));
	suite->add_suite(suite, suite2);
	myres = tap_result_new(false, NULL);
	suite->run(suite, myres);
	ASSERT_EQUAL(myres->was_successful(myres), 1, "One test fails.");
	myres->free(myres);
}

static void
_test_abort(TESTARGS, void *usrptr)
{
	abort();
}

static void
test_run_tests3(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr,
					  *suite1,
					  *suite2;
	struct test_result *myres;

	/* Add two suites, one fails and the second is not run. */
	/* It the second test is executed, the whole program abort. */
	suite1 = test_suite_new();
	suite1->add_test(suite1, test_case_new(_test_fail));
	suite->add_suite(suite, suite1);
	suite2 = test_suite_new();
	suite2->add_test(suite2, test_case_new(_test_abort));
	suite->add_suite(suite, suite2);
	myres = tap_result_new(true, NULL);
	suite->run(suite, myres);
	ASSERT_EQUAL(myres->was_successful(myres), 1, "One test fails.");
	myres->free(myres);
}

static void
test_skip_suite(TESTARGS, void *usrptr)
{
	struct test_suite *suite = (struct test_suite *) usrptr,
					  *suite1,
					  *suite2;
	struct test_result *myres;

	/* Run only the first suites, the second is skipped. */
	/* It the second test is executed, the whole program abort. */
	suite1 = test_suite_new();
	suite1->add_test(suite1, test_case_new(_test_success));
	suite->add_suite(suite, suite1);
	suite2 = test_suite_new();
	suite2->skip = "test skip";
	suite2->add_test(suite2, test_case_new(_test_abort));
	suite->add_suite(suite, suite2);
	myres = tap_result_new(false, NULL);
	suite->run(suite, myres);
	ASSERT_EQUAL(myres->was_successful(myres), 0, "Only one test executed.");
	myres->free(myres);
}

static void
_setup(struct test_suite *suite)
{
	suite->usrptr = test_suite_new();
}

static void
_teardown(struct test_suite *suite)
{
	struct test_suite *s = (struct test_suite *) suite->usrptr;
	s->free(s);
}

struct test_suite*
load_test_suite(struct test_loader *loader)
{
	struct test_suite *suite;

	assert(loader != NULL);
	suite = test_suite_new();
	suite->name = "test_suite";
	suite->doc = "Test the test_suite structure";
	suite->add_test(suite, test_case_new(test_new_suite));
	suite->add_test(suite, test_case_new(test_len0));
	suite->add_test(suite, test_case_new(test_add_test));
	suite->add_test(suite, test_case_new(test_add_suite0));
	suite->add_test(suite, test_case_new(test_add_suite1));
	suite->add_test(suite, test_case_new(test_len1));
	suite->add_test(suite, test_case_new(test_run_tests0));
	suite->add_test(suite, test_case_new(test_run_tests1));
	suite->add_test(suite, test_case_new(test_run_tests2));
	suite->add_test(suite, test_case_new(test_run_tests3));
	suite->add_test(suite, test_case_new(test_skip_suite));
	suite->setup = _setup;
	suite->teardown = _teardown;
	return suite;
}

int
main(int argc, char *argv[])
{
	return test_main3(0, (char **) {NULL});
}
