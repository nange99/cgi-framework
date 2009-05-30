#!/bin/bash
export REQUEST_METHOD="GET"
export PATH_INFO="/app/do.list"
libtool --mode=execute valgrind --tool=memcheck -v --leak-check=full --show-reachable=yes ./test_cgi
