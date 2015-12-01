#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "unittest.h"
#include "unittest_priv.h"


struct test_suite_impl {
	SUITE_HEAD
	struct list *tests;
	struct list *suites;
};


static void
test_suite_add_test(struct test_suite *suite, struct test_case *test)
{
	struct test_suite_impl *suiteimpl = (struct test_suite_impl *) suite;

	suiteimpl->tests = list_append(suiteimpl->tests, test);
}

static void
test_suite_add_suite(struct test_suite *suite, struct test_suite *suitec)
{
	struct test_suite_impl *suiteimpl = (struct test_suite_impl *) suite;

	suiteimpl->suites = list_append(suiteimpl->suites, suitec);
}

static void
test_suite_run(struct test_suite *suite, struct test_result *result)
{
	struct list *iter;
	struct test_case *test;
	struct test_suite_impl *suiteimpl = (struct test_suite_impl *) suite;
	struct test_suite *suitec;

	assert(suite != NULL);
	assert(result != NULL);

	for (iter = suiteimpl->tests; iter != NULL; iter = iter->next) {
		if (result->shouldstop)
			break;
		test = (struct test_case *) iter->data;
		assert(test != NULL);
		assert(test->run != NULL);
		test->run(test, suite, result);
	}
	for (iter = suiteimpl->suites; iter != NULL; iter = iter->next) {
		if (result->shouldstop)
			break;
		suitec = (struct test_suite *) iter->data;
		assert(suitec != NULL);
		assert(suitec->run != NULL);
		if (suitec->skip == NULL)
			/* TODO: add a skip to result */
			suitec->run(suitec, result);
	}
}

static unsigned int
test_suite_len(struct test_suite *suite)
{
	struct list *iter;
	unsigned int c;
	struct test_suite_impl *si = (struct test_suite_impl *) suite;
	struct test_suite *suitep;

	c = 0;
	for (iter = si->suites; iter != NULL; iter = iter->next) {
		suitep = (struct test_suite *) iter->data;
		if (suitep->skip == NULL)
			c += suitep->len(suitep);
	}
	return list_len(si->tests) + c;
}

static void
test_suite_free(struct test_suite *suite)
{
	list_free(((struct test_suite_impl *)suite)->tests, free);
	list_free(((struct test_suite_impl *)suite)->suites, free);
	free(suite);
}

struct test_suite *
test_suite_new(void)
{
	struct test_suite_impl *suite;

	suite = (struct test_suite_impl *) calloc(1,
			sizeof(struct test_suite_impl));
	if (suite == NULL)
		err_sys("malloc");
	suite->free = test_suite_free;
	suite->add_test = test_suite_add_test;
	suite->add_suite = test_suite_add_suite;
	suite->run = test_suite_run;
	suite->len = test_suite_len;
	return (struct test_suite *) suite;
}
