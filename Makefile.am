
EXTRA_DIST = \
	runtests.sh \
	test/data/example_2.json

TESTS = runtests.sh

bin_PROGRAMS = jup

jup_SOURCES = jup.cc utf8.h utilstrencodings.cpp utilstrencodings.h
jup_LDADD = @ARGP_LIBS@ @UNIVALUE_LIBS@

