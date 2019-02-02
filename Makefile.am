
EXTRA_DIST = \
	test/runtests.js \
	test/test-file-base64 \
	test/test-file-csv \
	test/test-file-hex \
	test/test-file-indent \
	test/test-file-text \
	test/test-file-json \
	test/data/random.dat \
	test/data/random.txt \
	test/data/test.csv \
	test/data/indent-3-out.json \
	test/data/file-json-1-out.json \
	test/data/file-csv-1-out.json \
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

TESTS = test/runtests.js \
	test/test-file-base64 \
	test/test-file-csv \
	test/test-file-hex \
	test/test-file-indent \
	test/test-file-json \
	test/test-file-text

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

