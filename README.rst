libunittest
===========

libunittest is a small C library for those who want to perform unit
test in C. The library is modelled on Python's unittest and inherit its
flexibility.

Installing
==========

.. highlight:: sh

The library uses the autotools to install itself. If you get the code
from github, run autoreconf::

   autoreconf --install

and then compile the code in the usual way::

   ./configure [options]
   make
   sudo make install

If you want to install it in you home directory::

   ./configure --prefix=$HOME/.local
   make
   make install

Of course the library has itself a test suite::

   make check

Tutorial
========

The following is the classic hello world example:

.. code-block:: c
   :linenos:

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
      char *args[] = {argv[0], NULL};

      return test_main3(1, args);
   }

Line 1: the header ``unittest.h`` import all the types and the
functions.

.. add a link to the documentation for the header

Lines 20-26: standard ``main()`` entry point. It invokes the
``test_main3()`` function, a short cut that initializes the test system,
runs the tests and return a suitable exit status value. Here we
explicitelly pass the function arguments to ``test_main3()``.

Lines 4-5: the ``test_success()`` function is the test case. The first
argument *must* be the :c:macro:`TESTARGS` marco. This, actually they
are more than one, argument is used by the :c:macro:`SUCCESS` macro and,
more generally, by all ``ASSERT_*`` macros. The second argument is a
``void *`` and the user could use it to pass arbitrary data the the test
case. More on this later.

.. warning::
   Do not forget the ``TESTARGS`` macro in the function declaration.
   It declares two variables whose names should not clash with your
   variables' names.

Line 7: the :c:macro:`SUCCESS` is an assertion that always succeed. It
accept a ``const char *`` as argument that is the description of the
test.

Lines 10-11: the ``load_test_suite()`` function is a global visible
(i.e.  non ``static``) function. It accepts a :c:type:`test_loader` and
returns a :c:type:`test_suite`. The default :c:type:`test_loader` invoke
automatically this function to load the test methods.

Line 15: create a :c:type:`test_suite`.

Line 16: add a :c:type:`test_case` to the suite.

Compile the code with the following comand::

   gcc -ldl -lunittest -rdynamic helloworld.c

The ``unittest`` is the testing library and ``dl`` is required to load
the dinamic libraries. The standard :c:type:`test_loader` uses the main
program as a library and try load it to search for the
``load_test_suite()`` function. ``-rdynamic`` is required to allow
backtraces from within the program.

If you run the program, you should see the following output::

   1..1
   ok test_success # hello world

This outout follow the `TAP`_ protocol. It just says that 1 test (1..1)
is run and it succeed (ok).

If we add the following function::

   static void
   test_fail(TESTARGS, void *usrptr)
   {
      FAIL("hello fail");
   }

and we add the following line in ``load_test_suite()``::

	suite->add_test(suite, test_case_new(test_fail));

and run the code, you will get the following output::

   1..2
   ok test_success # hello world
   not ok test_fail # hello fail

Two tests are run (1..2), the first succeed (ok) and the second fail
(not ok).

Integrate libunittest with autotools
====================================

libunittest supports the `TAP`_ protocol and can be easily integrated
with `automake`_. Look at the ``tests/Makefile.am`` file. Do not forget
to copy the file ``tap-driver.sh`` from the automake sources, then run::

   make check


.. _TAP: http://testanything.org/
.. _automake: https://www.gnu.org/software/automake/manual/automake.html#Tests

.. vim: tw=72
