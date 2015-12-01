#include <dlfcn.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "unittest.h"
#include "unittest_priv.h"
#include <stdio.h>

#define LOAD_TEST_SUITE "load_test_suite"

static void
suite_error(TESTARGS, void *usrptr)
{
	FAIL((char *) usrptr);
}

static struct test_suite *
suite_error_new(const char *error)
{
	struct test_suite *suite;

	suite = test_suite_new();
	suite->usrptr = (void *) error;
	suite->add_test(suite, test_case_new(suite_error));
	return suite;
}

struct test_suite *
load_test_from_dyn_library(struct test_loader *loader, const char *filename)
{
	void *handle;
	struct test_suite *suite;
	typedef struct test_suite* (_Loaderhook)(struct test_loader*);
	_Loaderhook *load_suite;

	assert(loader != NULL);

	if ((handle = dlopen(filename, RTLD_LAZY | RTLD_LOCAL)) == NULL)
		return suite_error_new(dlerror());
	load_suite = (_Loaderhook *) dlsym(handle, LOAD_TEST_SUITE);
	if (load_suite == NULL)
		suite = suite_error_new("no suite found");
	else if ((suite = load_suite(loader)) == NULL )
		suite = suite_error_new("error while loading suite");
	dlclose(handle);
	return suite;
}

static struct test_suite *
test_loader_discover_tests(struct test_loader *loader, int argc, char *argv[])
{
	struct test_suite *suite;
	int c;

	suite = test_suite_new();
	for (c = 0; c < argc; c++)
		suite->add_suite(suite, load_test_from_dyn_library(loader, argv[c]));
	suite->add_suite(suite, load_test_from_dyn_library(loader, NULL));
	return suite;
}

static void
test_loader_free(struct test_loader *loader)
{
	free(loader);
}

struct test_loader *
test_loader_new(void)
{
	struct test_loader *loader;

	loader = (struct test_loader *) calloc(1, sizeof(struct test_loader));
	if (loader == NULL)
		err_sys("malloc");
	loader->free = test_loader_free;
	loader->load_tests = test_loader_discover_tests;
	return loader;
}

static struct test_suite *
test_loader_load_tests_function(struct test_loader *loader, int argc,
		char *argv[])
{
	struct test_suite *suite;
	struct test_case *test;
	struct test_loader_func *loaderf = (struct test_loader_func *) loader;

	suite = test_suite_new();
	test = test_case_new_impl(loaderf->name, loaderf->skip, loaderf->todo,
			loaderf->func);
	suite->add_test(suite, test);
	return suite;
}

struct test_loader *
func_loader_new(void)
{
	struct test_loader *loader;

	loader = (struct test_loader *) calloc(1, sizeof(struct test_loader_func));
	if (loader == NULL)
		err_sys("malloc");
	loader->free = test_loader_free;
	loader->load_tests = test_loader_load_tests_function;
	return loader;
}
