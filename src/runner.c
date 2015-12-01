#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "unittest.h"
#include "unittest_priv.h"


struct tap_runner {
	RUNNER_HEAD
};


static struct test_result *
test_runner_run(struct test_runner *runner, struct test_suite *suite)
{
	assert(runner != NULL);
	assert(runner->result != NULL);
	assert(suite != NULL);
	assert(suite->run != NULL);

	if (suite->skip != NULL) {
		if (runner->result->stream != NULL)
			fprintf(runner->result->stream, "1..0 # SKIP %s\n", suite->skip);
	} else {
		if (runner->result->stream != NULL)
			fprintf(runner->result->stream, "1..%d\n", suite->len(suite));
		if (runner->result->start_run != NULL)
			runner->result->start_run(runner->result);
		suite->run(suite, runner->result);
		if (runner->result->stop_run != NULL)
			runner->result->stop_run(runner->result);
	}
	return runner->result;
}

static void
test_runner_free(struct test_runner *runner)
{
	runner->result->free(runner->result);
	free(runner);
}

struct test_runner *
tap_runner_new(int verbosity, bool failfast, bool buffer, FILE *stream)
{
	struct test_runner *runner;

	runner = (struct test_runner *) calloc(1, sizeof(struct tap_runner));
	if (runner == NULL)
		err_sys("malloc");
	runner->result = tap_result_new(failfast, stream);
	runner->run = test_runner_run;
	runner->free = test_runner_free;
	return runner;
}
