#!/bin/bash
export REQUEST_METHOD="POST"
export PATH_INFO="/app/do.list"
export CONTENT_LENGTH=23
libtool --mode=execute valgrind --tool=memcheck -v --track-origins=yes --leak-check=full --show-reachable=yes ./test_cgi < post_data