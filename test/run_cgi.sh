#!/bin/bash
export REQUEST_METHOD="GET"
export PATH_INFO="/app/do.list"
export QUERY_STRING="user=Pedro%20Kiefer&asdf=pedro"
#libtool --mode=execute valgrind --tool=memcheck -v --track-origins=yes --leak-check=full --show-reachable=yes ./test_cgi
glibtool --mode=execute ./test_cgi
