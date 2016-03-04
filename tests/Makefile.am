LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/tap-driver.sh
AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CFLAGS = -Wall -Werror
AM_LDFLAGS = -Wl,--no-as-needed -ldl -rdynamic
LDADD = $(top_builddir)/src/libunittest.la

check_PROGRAMS = test_assertions test_suite
TESTS = $(check_PROGRAMS)
test_assertions_SOURCES = test_assertions.c
test_suite_SOURCES = test_suite.c
