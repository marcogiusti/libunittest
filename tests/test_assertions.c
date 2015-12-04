#include <stdlib.h>
#include <assert.h>
#include "unittest.h"
#include "unittest_priv.h"

#define loader_for_function(loader, func, skip, todo) \
	((struct test_loader_func *) loader)->name = #func; \
	((struct test_loader_func *) loader)->skip = skip; \
	((struct test_loader_func *) loader)->todo = todo; \
	((struct test_loader_func *) loader)->func = func;

struct _usrdata {
	struct test_runner *runner;
	struct test_loader *loader;
	struct test_suite *suite;
};
typedef void (*Function)(TESTARGS, void *);

static struct test_result *_run_tests(void *, void (*)(TESTARGS, void *),
		const char *, const char *);


static void
_test_success(TESTARGS, void *usrptr)
{
	SUCCESS("success");
}

static void
test_success(TESTARGS, void *usrptr)
{
	struct test_result *r;

	r = _run_tests(usrptr, _test_success, NULL, NULL);
	ASSERT_EQUAL(r->was_successful(r), 0, "0: success exit status");
}

static void
_test_fail(TESTARGS, void *usrptr)
{
	FAIL("fail");
}

static void
test_fail(TESTARGS, void *usrptr)
{
	struct test_result *r;

	r = _run_tests(usrptr, _test_fail, NULL, NULL);
	ASSERT_EQUAL(r->was_successful(r), 1, "1: fail exit status");
}

static void
_test_skip(TESTARGS, void *usrptr)
{
	FAIL("this test should not be run");
}

static void
test_skip(TESTARGS, void *usrptr)
{
	struct test_result *r;

	r = _run_tests(usrptr, _test_skip, "test skip", NULL);
	ASSERT_EQUAL(r->was_successful(r), 77, "77: there are skipped tests");
}

static void
_test_no_assertions(TESTARGS, void *usrptr)
{ }

static void
test_no_assertions(TESTARGS, void *usrptr)
{
	struct test_result *r;

	r = _run_tests(usrptr, _test_no_assertions, NULL, NULL);
	ASSERT_EQUAL(r->was_successful(r), 0, "0: no test, no fun");
}

static void
_test_todo(TESTARGS, void *usrptr)
{
	FAIL("this test should be fixed");
}

static void
test_todo(TESTARGS, void *usrptr)
{
	struct test_result *r;

	r = _run_tests(usrptr, _test_todo, NULL, "test todo");
	ASSERT_EQUAL(r->was_successful(r), 0, "A failure is expected");
}

static void
_setup(struct test_suite *suite)
{
	struct _usrdata *usrdata;

	usrdata = (struct _usrdata *) malloc(sizeof(struct _usrdata));
	usrdata->runner = tap_runner_new(0, false, false, NULL);
	usrdata->loader = func_loader_new();
	suite->usrptr = usrdata;
}

static void
_teardown(struct test_suite *suite)
{
	struct _usrdata *usrdata = (struct _usrdata *) suite->usrptr;

	usrdata->runner->free(usrdata->runner);
	usrdata->loader->free(usrdata->loader);
	usrdata->suite->free(usrdata->suite);
	free(usrdata);
}

static struct test_result *
_run_tests(void *usrptr, void (*func)(TESTARGS, void *), const char *skip,
		const char *todo)
{
	struct test_runner *runner = ((struct _usrdata *) usrptr)->runner;
	struct test_loader *loader = ((struct _usrdata *) usrptr)->loader;
	struct test_suite *suite;

	loader_for_function(loader, func, skip, todo);
	suite = loader->load_tests(loader, 0, (char **) {NULL});
	((struct _usrdata *) usrptr)->suite = suite;
	return runner->run(runner, suite);
}

struct test_suite*
load_test_suite(struct test_loader *loader)
{
	struct test_suite *suite;

	assert(loader != NULL);
	suite = test_suite_new();
	suite->name = "test_asserts";
	suite->doc = "Basic tests about the asserts";
	suite->add_test(suite, test_case_new(test_success));
	suite->add_test(suite, test_case_new(test_fail));
	suite->add_test(suite, test_case_new(test_skip));
	suite->add_test(suite, test_case_new(test_no_assertions));
	suite->add_test(suite, test_case_new(test_todo));
	suite->setup = _setup;
	suite->teardown = _teardown;
	return suite;
}

int
main(int argc, char *argv[])
{
	return test_main3(argc, argv);
	/* int i; */
	/* struct list *lp; */
	/* struct test_result result; */
	/* struct test_loader loader; */

	/* test_suite_run(load_test_suite(&loader), &result); */
}
