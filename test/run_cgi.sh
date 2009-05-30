#!/bin/bash
export REQUEST_METHOD="GET"
export PATH_INFO="/app/do.list"
glibtool --mode=execute ./test_cgi
