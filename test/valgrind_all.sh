#!/bin/bash

echo "=== TESTE 1 ==="

export REQUEST_METHOD="GET"
export PATH_INFO="/app/do.test"
export QUERY_STRING="user=Pedro%20Kiefer&asdf=pedro"
libtool --mode=execute valgrind --tool=memcheck -v --track-origins=yes --leak-check=full --show-reachable=yes ./test_cgi

echo "=== TESTE 2 ==="

export REQUEST_METHOD="POST"
export CONTENT_LENGTH=23
libtool --mode=execute valgrind --tool=memcheck -v --track-origins=yes --leak-check=full --show-reachable=yes ./test_cgi < post_data

echo "=== TESTE 3 ==="
export REQUEST_METHOD="GET"
export PATH_INFO="/app/do.list"
libtool --mode=execute valgrind --tool=memcheck -v --track-origins=yes --leak-check=full --show-reachable=yes ./test_cgi

echo "=== TESTE 4 ==="
export PATH_INFO="/app/do.expr"
libtool --mode=execute valgrind --tool=memcheck -v --track-origins=yes --leak-check=full --show-reachable=yes ./test_cgi


#libtool --mode=execute ./test_cgi
