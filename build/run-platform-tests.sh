#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <platformtag>"
    exit 2
fi

platformtag="$1"

set -eu

ippdir=/opt/intel/ipp

echo
if [ -d "$ippdir" ]; then
    echo "Found IPP directory $ippdir, considering IPP as well as other options"
else
    echo "No IPP directory $ippdir, not testing with IPP"
fi

if valgrind --version >/dev/null 2>&1 ;
then
    have_valgrind=yes
else
    echo
    echo "No valgrind executable found, not using valgrind"
    have_valgrind=no
fi

tmpfile=$(mktemp "/tmp/test_XXXXXX")
trap "rm -f $tmpfile" 0

run() {
    successtext="$1"
    shift
    echo -n "Running \"$@\"..."
    if "$@" > "$tmpfile" 2>&1 ; then
	if [ -z "$successtext" ] || fgrep -q "$successtext" "$tmpfile" ; then
	    echo " OK"
	    return 0
	else
	    echo " Failed"
	    cat "$tmpfile"
	    return 1
	fi
    else
	echo " Failed"
	cat "$tmpfile"
	return 1
    fi
}

for mf in Makefile build/Makefile.$platformtag build/Makefile.$platformtag.* ; do

    case "$mf" in
	*~) continue;;
	*.bak) continue;;
	*ipp)
	    if [ ! -d "$ippdir" ]; then
		continue
	    fi;;
    esac

    if [ ! -f "$mf" ]; then
	continue
    fi
    
    echo
    echo "Building and testing with $mf:"
    echo

    if [ -f "../bqvec/$mf" ]; then
	echo "Similar Makefile exists in bqvec dir, building there first..."
	( cd ../bqvec ; run "" make -f "$mf" clean && run "" make -f "$mf" )
	echo "Build in bqvec done"
	echo
    fi
    
    run "" make -f "$mf" clean
    run "No errors detected" make -f "$mf" test

    if [ "$have_valgrind" = "yes" ]; then
	for t in test-* ; do
	    if [ -x "$t" ]; then
		run "no leaks are possible" valgrind --leak-check=full ./"$t"
	    fi
	done
    fi
done

echo
echo "All tests passed"
echo
