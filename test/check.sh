#!/bin/bash
set -euo pipefail

cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null

function check() {
	if [ -f "$1" ] ; then
		echo "Check		$1" ;
		if [ -x "$1" ] ; then
			"./$1"
		else
			diff -B -w -u "$1" -
		fi
	fi
}

if [ $# -eq 1 -a -f "$1.cpp" ] ; then
	test="../test-$1"

	# Build (if not exist)
	test -x "$test" || make -C ".." "test-$1"

	# Execute and capture stdout + stderr
	stdout=$(mktemp)
	stderr=$(mktemp)
	"$test" >"$stdout" 2>"$stderr"

	# Compare stdout + stderr with example
	check "$1.stdout" < "$stdout"
	check "$1.stderr" < "$stderr"

	# Remove files
	rm "$stdout" "$stderr"
else
	echo "Usage: $0 [TESTCASE]"
fi
