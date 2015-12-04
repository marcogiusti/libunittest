#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>
#include "unittest.h"
#include "unittest_priv.h"


struct test_case_impl {
	CASE_HEAD
	jmp_buf *jmpbuffer;
};

enum assert_result {
	SUCCESS,
	FAILURE,
	XFAILURE,
	XSUCCESS,
	ERROR
};

static void
test_case_run(struct test_case *test, struct test_suite *suite,
		struct test_result *result)
{
	jmp_buf jmpbuffer;

	assert(test != NULL);
	assert(result != NULL);
	assert(result->add_skip != NULL);
	assert(result->add_success != NULL);
	assert(result->add_xsuccess != NULL);
	assert(result->add_failure != NULL);
	assert(result->add_xfailure != NULL);

	if (result->start_test != NULL)
		result->start_test(result, test);
	if (test->skip != NULL) {
		result->add_skip(result, test);
		if (result->stop_test != NULL)
			result->stop_test(result, test);
		return;
	}
	if (suite->setup != NULL)
		suite->setup(suite, result);
	((struct test_case_impl *) test)->jmpbuffer = &jmpbuffer;
	switch (setjmp(jmpbuffer)) {
		case SUCCESS:
			test->func(test, result, suite->usrptr);
			/* NOTE: Reaced only if the test terminate correctly. */
			/* NOTE: If there is no assertion, all the fields are NULL or 0. */
			if (test->todo != NULL)
				result->add_xsuccess(result, test);
			else
				result->add_success(result, test);
			break;
		case FAILURE:
			result->add_failure(result, test);
			break;
		case XFAILURE:
			result->add_xfailure(result, test);
			break;
		case ERROR:
			result->add_error(result, test);
		default:
			abort();  /* programming error */
	}
	((struct test_case_impl *) test)->jmpbuffer = NULL;
	if (suite->teardown != NULL)
		suite->teardown(suite, result);
	if (result->stop_test != NULL)
		result->stop_test(result, test);
}

static void
test_case_assert(struct test_case *test, struct test_result *result, bool pass,
		const char *condition, const char *msg, const char *filename, unsigned int lineno)
{
	assert(((struct test_case_impl *) test)->jmpbuffer != NULL);
	test->msg = msg;
	test->condition = condition;
	test->filename = filename;
	test->lineno = lineno;
	if (!pass) {
		if (test->todo == NULL)
			longjmp(*((struct test_case_impl *) test)->jmpbuffer, FAILURE);
		else
			longjmp(*((struct test_case_impl *) test)->jmpbuffer, XFAILURE);
	}
}

struct test_case *
test_case_new_impl(const char *name, const char *skip, const char *todo,
		void (*func)(struct test_case *, struct test_result *, void *))
{
	struct test_case *test;

	test = (struct test_case *) calloc(1, sizeof(struct test_case_impl));
	if (test == NULL)
		err_sys("malloc");
	test->name = name;
	test->skip = skip;
	test->todo = todo;
	test->func = func;
	test->run = test_case_run;
	test->assert_impl = test_case_assert;
	return test;
}
