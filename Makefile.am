
EXTRA_DIST = \
	runtests.js \
	test/data/all-tests.json \
	test/data/array-1-out.json \
	test/data/array-1.cmd \
	test/data/example_2.json \
	test/data/false-1-out.json \
	test/data/false-1.cmd \
	test/data/get-1-out.json \
	test/data/get-1.cmd \
	test/data/int-1-out.json \
	test/data/int-1.cmd \
	test/data/new-1-out.json \
	test/data/new-1.cmd \
	test/data/newarray-1-out.json \
	test/data/newarray-1.cmd \
	test/data/null-1-out.json \
	test/data/null-1.cmd \
	test/data/num-1-out.json \
	test/data/num-1.cmd \
	test/data/object-1-out.json \
	test/data/object-1.cmd \
	test/data/pretty-out.json \
	test/data/str-1-out.json \
	test/data/str-1.cmd \
	test/data/true-1-out.json \
	test/data/true-1.cmd

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

