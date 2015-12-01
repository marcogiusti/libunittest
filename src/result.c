#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "unittest.h"
#include "unittest_priv.h"


struct tap_result {
	RESULT_HEAD
	struct list *failures;
	struct list *xfailures;
	struct list *successes;
	struct list *xsuccesses;
	struct list *skipped;
};

static void
tap_result_start_run(struct test_result *result)
{ }

static void
tap_result_stop_run(struct test_result *result)
{ }

static void
tap_result_add_skip(struct test_result *result, struct test_case *test)
{
	struct list **skipped = &((struct tap_result *) result)->skipped;

	assert(test->name != NULL);
	assert(test->skip != NULL);
	*skipped = list_append(*skipped, test);
	if (result->stream != NULL)
		fprintf(result->stream, "ok %s # SKIP %s\n", test->name, test->skip);
}

static void
tap_result_add_success(struct test_result *result, struct test_case *test)
{
	struct list **successes = &((struct tap_result *) result)->successes;

	assert(test->name != NULL);
	*successes = list_append(*successes, test);
	if (result->stream != NULL) {
		if (test->msg != NULL)
			fprintf(result->stream, "ok %s # %s\n", test->name, test->msg);
		else
			fprintf(result->stream, "ok %s\n", test->name);
	}
}

static void
tap_result_add_xsuccess(struct test_result *result, struct test_case *test)
{
	struct list **xsuccesses = &((struct tap_result *) result)->xsuccesses;

	assert(test->name != NULL);
	assert(test->todo != NULL);
	*xsuccesses = list_append(*xsuccesses, test);
	if (result->stream != NULL)
		fprintf(result->stream, "ok %s # TODO %s\n", test->name, test->todo);
	if (result->failfast)
		result->shouldstop = true;
}

static void
tap_result_add_failure(struct test_result *result, struct test_case *test)
{
	struct list **failures = &((struct tap_result *) result)->failures;

	assert(test->name != NULL);
	*failures = list_append(*failures, test);
	if (result->stream != NULL) {
		if (test->msg != NULL)
			fprintf(result->stream, "not ok %s # %s\n", test->name, test->msg);
		else
			fprintf(result->stream, "not ok %s\n", test->name);
	}
	if (result->failfast)
		result->shouldstop = true;
}

static void
tap_result_add_xfailure(struct test_result *result, struct test_case *test)
{
	struct list **xfailures = &((struct tap_result *) result)->xfailures;

	assert(test->name != NULL);
	assert(test->todo != NULL);
	*xfailures = list_append(*xfailures, test);
	if (result->stream != NULL)
		fprintf(result->stream, "not ok %s # TODO %s\n", test->name,
			test->todo);
}

static int
tap_result_was_successful(struct test_result *_result)
{
	struct tap_result *result = (struct tap_result *) _result;

	if (result->failures != NULL)
		return 1;
	if (result->successes == NULL && result->skipped)
		return 77;
	return 0;
}

static void
tap_result_free(struct test_result *result)
{
	struct tap_result *tapresult = (struct tap_result *) result;

	assert(result != NULL);
	list_free(tapresult->failures, NULL);
	list_free(tapresult->xfailures, NULL);
	list_free(tapresult->successes, NULL);
	list_free(tapresult->xsuccesses, NULL);
	list_free(tapresult->skipped, NULL);
	free(result);
}

struct test_result *
tap_result_new(bool failfast, FILE *stream)
{
	struct test_result *result;
	struct tap_result *tapresult;

	result = (struct test_result *) malloc(sizeof(struct tap_result));
	if (result == NULL)
		err_sys("malloc");
	tapresult = (struct tap_result *) result;
	tapresult->failures = NULL;
	tapresult->xfailures = NULL;
	tapresult->successes = NULL;
	tapresult->xsuccesses = NULL;
	tapresult->skipped = NULL;
	result->shouldstop = false;
	result->failfast = failfast;
	result->stream = stream;
	result->free = tap_result_free;
	result->start_run = tap_result_start_run;
	result->stop_run = tap_result_stop_run;
	result->add_skip = tap_result_add_skip;
	result->add_success = tap_result_add_success;
	result->add_xsuccess = tap_result_add_xsuccess;
	result->add_failure = tap_result_add_failure;
	result->add_xfailure = tap_result_add_xfailure;
	result->was_successful = tap_result_was_successful;
	return result;
}
