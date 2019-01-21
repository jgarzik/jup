
EXTRA_DIST = \
	runtests.sh \
	test/data/example_2.json

TESTS = runtests.sh

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

