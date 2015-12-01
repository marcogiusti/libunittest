#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "unittest.h"


static int _test_main1(struct test_runner *runner, struct test_loader *loader,
		bool verbosity, bool failfast, bool buffered, FILE *stream, int argc,
		char *argv[]);
static int _test_main2(struct test_runner *runner, struct test_loader *loader,
		int argc, char *argv[]);


static const char *usage =
	"Usage: %s [options] [tests]\n"
	"\n"
	"Options:\n"
	"  -h, --help       Show this message\n"
	"  -v, --verbose    Verbose output\n"
	"  -q, --quiet      Minimal output\n"
	"  -f, --failfast   Stop on first failure\n"
	"  -c, --catch      Catch control-C and display results\n"
	"  -b, --buffer     Buffer stdout and stderr during test runs\n";

static const char *version = "0.1";

struct unittest_opts {
	bool verbosity;
	bool failfast;
	bool buffered;
	FILE *stream;
	int argc;
	char **argv;
};

static void
print_usage(const char *prog, int exit_status)
{
	fprintf(stderr, usage, prog);
	exit(exit_status);
}

static void
print_version(const char *prog)
{
	fprintf(stderr, "%s %s\n", prog, version);
	exit(0);
}

void
unittest_parse_options(int argc, char *argv[], struct unittest_opts *options)
{
	const char *optstring;
	int opt;

	optstring = "fvqhVb";
	opterr = 0;
	while ((opt = getopt(argc, argv, optstring)) != -1) {
		switch (opt) {
			case 'f':
				options->failfast = true;
				break;
			case 'v':
				options->verbosity++;
				break;
			case 'q':
				options->verbosity--;
				break;
			case 'h':
				print_usage(argv[0], 0);
			case 'V':
				print_version(argv[0]);
			case 'b':
				options->buffered = true;
				break;
			default:
				print_usage(argv[0], 1);
		}
	}
	options->argc = argc - optind;
	options->argv = &argv[optind];
}

int
test_main(int argc, char *argv[],  struct test_runner *runner,
		struct test_loader *loader)
{
	struct unittest_opts options = {
		.verbosity = 0,
		.failfast = false,
		.buffered = false,
		.stream = stdout,
	};

	unittest_parse_options(argc, argv, &options);
	return _test_main1(runner, loader, options.verbosity, options.failfast,
			options.buffered, options.stream, options.argc, options.argv);
}

static int
_test_main1(struct test_runner *runner, struct test_loader *loader,
		bool verbosity, bool failfast, bool buffered, FILE *stream, int argc,
		char *argv[])
{
	int ret;
	bool mustfree = false;

	if (runner == NULL) {
		runner = tap_runner_new(verbosity, failfast, buffered, stream);
		mustfree = true;
	}
	ret = _test_main2(runner, loader, argc, argv);
	if (mustfree)
		runner->free(runner);
	return ret;
}

static int
_test_main2(struct test_runner *runner, struct test_loader *loader, int argc,
		char *argv[])
{
	int ret;
	bool mustfree = false;

	if (loader == NULL) {
		loader = test_loader_new();
		mustfree = true;
	}
	ret = run_tests(runner, loader, argc, argv);
	if (mustfree)
		loader->free(loader);
	return ret;
}

int
run_tests(struct test_runner *runner, struct test_loader *loader, int argc,
		char *argv[])
{
	struct test_result *result;
	struct test_suite *suite;
	int ret = 1;


	assert(runner != NULL);
	assert(loader != NULL);

	suite = loader->load_tests(loader, argc, argv);
	if (suite != NULL) {
		result = runner->run(runner, suite);
		if (result != NULL)
			ret = result->was_successful(result);
	}
	if (suite != NULL)
		suite->free(suite);
	return ret;
}
