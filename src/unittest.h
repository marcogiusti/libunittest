/*
 * Copyright (C) 2015 Marco Giusti.
 */
/** @file
 * The header file to include.
 */

#include <stdbool.h>
#include <stdio.h>

#ifndef UNITTEST_H
#define UNITTEST_H

struct test_case;
struct test_suite;


/**
 * Define the common fields for the test_result types.
 */
#define RESULT_HEAD \
	/** Set to stop the runner to run more tests. */ \
	bool shouldstop; \
	/** Set to interrupt the tests at the first failure. */ \
	bool failfast; \
	/** The stream to use to print the results of the run. */ \
	FILE *stream; \
	/** Free the resources acquired by the result. */ \
	void (*free)(struct test_result *result); \
	/** Executed before a run. */ \
	void (*start_run)(struct test_result *result); \
	/** Executed after a run. */ \
	void (*stop_run)(struct test_result *result); \
	/** Executed before each test. */ \
	void (*start_test)(struct test_result *result, struct test_case *test); \
	/** Executed after each test. */ \
	void (*stop_test)(struct test_result *result, struct test_case *test); \
	/** Add a test to the list of the skipped ones.
	 * @note The result does *not* own the test and should not try to free it.
	 */ \
	void (*add_skip)(struct test_result *result, struct test_case *test); \
	/** Add a test to the list of the success.
	 * @note The result does *not* own the test and should not try to free it.
	 */ \
	void (*add_success)(struct test_result *result, struct test_case *test); \
	/** Add a test to the list of the unexpected success.
	 * @note The result does *not* own the test and should not try to free it.
	 */ \
	void (*add_xsuccess)(struct test_result *result, struct test_case *test); \
	/** Add a test to the list of the failures.
	 * @note The result does *not* own the test and should not try to free it.
	 */ \
	void (*add_failure)(struct test_result *result, struct test_case *test); \
	/** Add a test to the list of the expected failures.
	 * @note The result does *not* own the test and should not try to free it.
	 */ \
	void (*add_xfailure)(struct test_result *result, struct test_case *test); \
	/** Return a `int` suitable as parameter to exit(). */ \
	int (*was_successful)(struct test_result* result);

/**
 * Represent the result of the tests.
 * TODO: implement an error list.
 */
struct test_result {
	RESULT_HEAD
};

/**
 * Create a new tap_result, an implementation of test_result.
 * @note If the memory allocation fails, the program aborts.
 * @param stream A stream the use to print the output.
 * @param failfast If the tests should stop at the first failure.
 */
struct test_result *tap_result_new(bool failfast, FILE *stream);

/**
 * Groups the required test arguments in a macro to hide the implementation
 * requirements.
 */
#define _TESTARG __test__
#define _RESULTARG __result__
#define TESTARGS struct test_case *_TESTARG, struct test_result *_RESULTARG

/**
 * Define the common fields for the test_case types.
 */
#define CASE_HEAD \
	/** The name of the test. Useful if the test fail to track the failure */ \
	const char *name; \
	/** The message to use in case the test fails. */ \
	const char *msg; \
	/** If not NULL the test is skipped. */ \
	const char *skip; \
	/** If not NULL the reason why test (should) fail. */ \
	const char *todo; \
	/** The condition actually tested. */ \
	const char *condition; \
	/** The name of the file where the test is located. */ \
	const char *filename; \
	/** The line number in the file. */ \
	unsigned int lineno; \
	void (*func)(struct test_case *test, struct test_result *result, \
			void *usrptr); \
	/** Run the test. All the arguments must be not NULL. */ \
	void (*run)(struct test_case *test, struct test_suite *suite, \
			struct test_result *result); \
	/**
	 * Check that `condition` is true and update the result.
	 * Don't use it directly but one of the ASSERT_ macros instead.
	 * @param result The result to update.
	 * @param pass If the test passed.
	 * @param condition The condition tested.
	 * @param msg The message associated with this assertion.
	 * @param filename The file where the test is located.
	 * @param lineno the line number in the file.
	 */ \
	void (*assert_impl)(struct test_case *test, struct test_result *result, \
			bool pass, const char *condition, const char *msg, \
			const char *filename, unsigned int lineno);

/**
 * Represent the smallest unit of testing.
 */
struct test_case {
	CASE_HEAD
};

/**
 * Create a new test case for the function `func`.
 * @note If the memory allocation fails, the program aborts.
 * @param func The function to run as part of the test.
 */
#define test_case_new(func) \
	test_case_new_impl(#func, NULL, NULL, func)

/**
 * Create a new test case for the function `func` but skip it.
 * @note If the memory allocation fails, the program aborts.
 * @param func The function to run as part of the test.
 * @param reason The reason why the test must be skipped.
 */
#define test_case_skip_new(func, reason) \
	test_case_new_impl(#func, reason, NULL, func)

/**
 * Create a new test case for the function `func` and mark it as failing.
 * @note If the memory allocation fails, the program aborts.
 * @param func The function to run as part of the test.
 * @param reason The reason why the test fails.
 */
#define test_case_todo_new(func, reason) \
	test_case_new_impl(#func, NULL, reason, func)

/**
 * Create a new test case.
 * @note Don't use this function but one of the test_case_*_new macros.
 */
struct test_case *test_case_new_impl(const char *name, const char *skip,
		const char *todo,
		void (*func)(struct test_case *, struct test_result *, void *));

/**
 * Define the common fields for the test_suite types.
 */
#define SUITE_HEAD \
	/** The name of the suite. */ \
	char *name; \
	/** A documentation string. */ \
	char *doc; \
	/** If not NULL the suite is skipped. */ \
	char *skip; \
	/** A generic pointer that the user could use in a test case. */ \
	void *usrptr; \
	/** Free the resources acquired by this suite. */ \
	void (*free)(struct test_suite *suite); \
	/** Called to set up the preconditions that the test needs.
	 * @note It must not fail. */ \
	void (*setup)(struct test_suite *suite, struct test_result *result); \
	/** Tear down the test fixture. @note It must not fail. */ \
	void (*teardown)(struct test_suite *suite, struct test_result *result); \
	/** Add a test to the current suite. */ \
	void (*add_test)(struct test_suite *suite, struct test_case *test); \
	/** Add a child suite to the current suite. */ \
	void (*add_suite)(struct test_suite *suite, struct test_suite *suitec); \
	/** Run all the tests of the suite. */ \
	void (*run)(struct test_suite *suite, struct test_result *result); \
	/** Return the number of the tests in the suite. */ \
	unsigned int (*len)(struct test_suite *suite);

/**
 * A collection of test cases.
 * TODO: make it also a collection of suites.
 */
struct test_suite {
	SUITE_HEAD
};

/**
 * Create a new test suite.
 * @note If the memory allocation fails, the program aborts.
 */
struct test_suite *test_suite_new(void);

/**
 * Define the common fields for the test_runner types.
 */
#define RUNNER_HEAD	\
	/** A result of the tests run. */ \
	struct test_result *result; \
	/** Free the resources acquired by the runner. */ \
	void (*free)(struct test_runner *runner); \
	/** Run the test suite. */ \
	struct test_result *(*run)(struct test_runner *runner, \
			struct test_suite *suite);

/**
 * The test runner run the suite and return a result.
 */
struct test_runner {
	RUNNER_HEAD
};

/**
 * Create a new runner that is compatible with the
 * [TAP specification](http://testanything.org/tap-specification.html).
 * @note If the memory allocation fails, the program aborts.
 * @param verbosity Indicate the verbosity level of the runner.
 * @param failfast If true the runner stop at the first test failed.
 * @param buffered If true the output is buffered.
 * @param stream The stream where to print the output.
 */
struct test_runner *tap_runner_new(int verbosity, bool failfast, bool buffered,
		FILE *stream);

/**
 * Define the common fields for the test_loader types.
 */
#define LOADER_HEAD	\
	/** Free the resources acquired by the loader. */ \
	void (*free)(struct test_loader *loader); \
	/** Load the tests based on the parameters passed. */ \
	struct test_suite *(*load_tests)(struct test_loader *loader, int argc, \
			char *argv[]);

/**
 * The loader loads the tests to run.
 * The default implementation search in the main program for the function
 * `load_test_suite()` and, if found, run it.
 */
struct test_loader {
	LOADER_HEAD
};

/**
 * Create a new test_loader.
 * @note If the memory allocation fails, the program aborts.
 */
struct test_loader *test_loader_new(void);

/**
 * A loader implementation that load a single function as test case.
 */
struct test_loader_func {
	LOADER_HEAD
	/** The name of the function. */
	const char *name;
	/** If not `NULL` the test should be skipped, otherwise the reason of the
	 * skip. */
	const char *skip;
	/** If not `NULL` the reason why the test should fail. */
	const char *todo;
	/** The pointer to the test case. */
	void (*func)(struct test_case *test, struct test_result *result,
			void *usrptr);
};

/**
 * Create a new test_loader_func.
 * @note If the memory allocation fails, the program aborts.
 */
struct test_loader *func_loader_new(void);

/**
 * Main entry point.
 * Parse the options, if needed create the runner and the loader, load the
 * tests and run them.
 * @param argc Lenght of argv with the ending `NULL`.
 * @param argv Array of parameters passed to the loader. argv[0] is the name of
 * the program.
 * @param runner The runner that will run the tests.
 * @param loader The loader that will load the tests.
 * @return 0 if all the tests pass, a suitable exit status otherwise.
 */
int test_main(int argc, char *argv[], struct test_runner *runner,
		struct test_loader *loader);

/**
 * Create a default loader, load the tests and run them.
 * Look test_main for a description of the parameters with the exception of
 * `loader` which is `NULL`.
 */
#define test_main2(argc, argv, runner) test_main(argc, argv, runner, NULL)

/**
 * Create a default loader and runner, load the tests and run them.
 * Look test_main for a description of the parameters with the exception of
 * `loader` and `runner` which are `NULL`.
 */
#define test_main3(argc, argv) test_main(argc, argv, NULL, NULL)

/**
 * Load the tests and run them.
 * @param runner The test_runner to use. Must not be NULL.
 * @param loader The test_loader to use. Must not be NULL.
 * @param argc Lenght of argv with the ending `NULL`.
 * @param argv Array of parameters passed to the loader. argv[0] is **not** the
 * name of the program.
 * @return 0 if all the tests pass, a suitable exit status otherwise.
 */
int run_tests(struct test_runner *runner, struct test_loader *loader, int argc,
		char *argv[]);

/**
 * A test that always pass.
 * @param msg A message to print.
 */
#define SUCCESS(msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			true, \
			"SUCCESS", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * A test that always fails.
 * @note It does not return.
 * @param msg A message to print.
 */
#define FAIL(msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			false, \
			"FAIL", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that the `first` and `second` parameters are equal.
 * @note If it fails, it does not return.
 * @param first The first member of the comparition.
 * @param second The second member of the comparition.
 * @param msg A message to print.
 */
#define ASSERT_EQUAL(first, second, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			(first) == (second), \
			"(" #first ") == (" #second ")", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that the `first` and `second` parameters are not equal.
 * @note If it fails, it does not return.
 * @param first The first member of the comparition.
 * @param second The second member of the comparition.
 * @param msg A message to print.
 */
#define ASSERT_NOT_EQUAL(first, second, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			(first) != (second), \
			"(" #first ") != (" #second ")", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that the two pointers are equal.
 * @note If it fails, it does not return.
 * @param first The first pointer.
 * @param second The second pointer.
 * @param msg A message to print.
 */
#define ASSERT_PTR_EQUAL(first, second, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			(const void *)(first) == (const void *)(second), \
			"(const void *)(" #first ") == (const void *)(" #second ")", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that the two pointers are not equal.
 * @note If it fails, it does not return.
 * @param first The first pointer.
 * @param second The second pointer.
 * @param msg A message to print.
 */
#define ASSERT_PTR_NOT_EQUAL(first, second, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			(const void *)(first) != (const void *)(second), \
			"(const void *)(" #first ") != (const void *)(" #second ")", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that the pointers is `NULL`.
 * @note If it fails, it does not return.
 * @param ptr The pointer to test.
 * @param msg A message to print.
 */
#define ASSERT_PTR_NULL(ptr, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			(const void *)(ptr) == NULL, \
			"(const void *)(" #ptr ") == NULL", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that the pointers is not `NULL`.
 * @note If it fails, it does not return.
 * @param ptr The pointer to test.
 * @param msg A message to print.
 */
#define ASSERT_PTR_NOT_NULL(ptr, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			(const void *)(ptr) != NULL, \
			"(const void *)(" #ptr ") != NULL", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that the two strings are equal.
 * @note If it fails, it does not return.
 * @param first The first string.
 * @param second The second string.
 * @param msg A message to print.
 */
#define ASSERT_STRING_EQUAL(first, second, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			strcmp((const char *)(first) == (const char *)(second)), \
			"strcmp(" #first " == " #second ")", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that the two strings are not equal.
 * @note If it fails, it does not return.
 * @param first The first string.
 * @param second The second string.
 * @param msg A message to print.
 */
#define ASSERT_STRING_NOT_EQUAL(first, second, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			strcmp((const char *)(first) != (const char *)(second)), \
			"strcmp(" #first " != " #second ")", \
			msg, \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that first and second are approximately equal.
 * Do the test by computing the difference and comparing to zero.
 * @note If it fails, it does not return.
 * @param first The first number.
 * @param second The second number.
 * @param msg A message to print.
 */
#define ASSERT_ALMOST_EQUAL(first, second, delta, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			fabs((double) (first) - (double) (second)) <= \
				fabs((double) delta), \
			"fabs((double) (" #first ") - (double) (" #second ")) <= " \
				"fabs((double) " #delta ")", \
			__FILE__, \
			__LINE__); \
} while(0)

/**
 * Test that first and second are not approximately equal.
 * Do the test by computing the difference and comparing to zero.
 * @note If it fails, it does not return.
 * @param first The first number.
 * @param second The second number.
 * @param msg A message to print.
 */
#define ASSERT_NOT_ALMOST_EQUAL(first, second, delta, msg) do { \
	_TESTARG->assert_impl(_TESTARG, \
			_RESULTARG, \
			fabs((double) (first) - (double) (second)) > \
				fabs((double) delta), \
			"fabs((double) (" #first ") - (double) (" #second ")) > " \
				"fabs((double) " #delta ")", \
			__FILE__, \
			__LINE__); \
} while(0)

#endif /* UNITTEST_H */
