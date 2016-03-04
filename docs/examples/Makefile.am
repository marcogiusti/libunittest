AM_CPPFLAGS = -I$(top_srcdir)/src -rdynamic
AM_LDFLAGS = -Wl,--no-as-needed -ldl -rdynamic
LDADD = $(top_builddir)/src/libunittest.la

noinst_PROGRAMS = helloworld fail failfast firstfail
helloworld_SOURCES = helloworld.c
fail_SOURCES = fail.c
failfast_SOURCES = failfast.c
firstfail_SOURCES = firstfail.c
