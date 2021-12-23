#!/bin/bash
ipcs -q | sed 1,3d | sed '$d' | sed 's/^..//' | tr 'a-z' 'A-Z' | awk '{print "ibase=16; "$1}' | while read line; do echo "$line" | bc; done;
