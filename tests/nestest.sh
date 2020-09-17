#! /bin/bash -eu

expected="tests/nestest-output.txt"
actual="tests/nestest.log"

./build/tests > "${actual}" || true
line=$(cmp "${expected}" "${actual}" | awk '{print $NF}')
exitcode=0

if [ -n "$line" ]; then
    echo "Files differ at line: $line"
    awk -v file="${expected}" -v line=$line 'NR==line-1,NR==line+1 {print "Expected: "$0;}' "${expected}"
    awk -v file="${actual}"   -v line=$line 'NR==line-1,NR==line+1 {print "Actual:   "$0;}' "${actual}"
    exitcode=1
fi

rm "${actual}"

exit $exitcode
