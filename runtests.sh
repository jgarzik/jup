#!/bin/sh

testdir=$srcdir/test
datadir=$testdir/data

testfn=example_2.json
if ./jup < $datadir/$testfn > /dev/null
then
	echo ""
else
	exit 1
fi

