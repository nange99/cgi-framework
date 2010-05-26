#!/bin/bash

find . \( -iname "*.h" -or -iname "*.c" -or -iname "*.y" -or -iname "*.l" \) -exec wc -l {} \; | awk '{ sum += $1}; END { print "total source lines: " sum }'
