#!/bin/bash
export REQUEST_METHOD="GET"
export PATH_INFO="/app/do.table"
export QUERY_STRING="user=Pedro%20Kiefer&asdf=pedro"
export HTTP_COOKIE="sid=a831bc800f842f1c5f4c59e66babbec554a98e50; "
libtool --mode=execute valgrind --tool=memcheck -v --track-origins=yes --leak-check=full --show-reachable=yes ./test_cgi
#libtool --mode=execute valgrind --tool=callgrind ./test_cgi
#libtool --mode=execute ./test_cgi
