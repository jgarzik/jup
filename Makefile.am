
EXTRA_DIST = \
	runtests.js \
	test/data/all-tests.json \
	test/data/array-1-out.json \
	test/data/array-1.cmd \
	test/data/example_2.json \
	test/data/pretty-out.json

TESTS = runtests.js

SUBDIRS = univalue

bin_PROGRAMS = jup

jup_SOURCES = \
	jup.cc \
	fileutil.cc \
	fileutil.h \
	utf8.h \
	utilstrencodings.cpp \
	utilstrencodings.h
jup_LDADD = @ARGP_LIBS@ univalue/.libs/libunivalue.a

