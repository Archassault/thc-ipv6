#!/bin/bash
test -z "$1" -o "$1" = "-h" && {
  echo $0 FILE
  echo prints the networks found in FILE
#, specify - for stdin
  exit 1
}
test -e "$1" -o "$1" = "-" || {
  echo Error: File $1 not found
  exit 1
}

{
  test "$1" = "-" && {
    echo no
  } || {
    cat $1 | egrep :: | egrep -v '^[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}|[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}$' | sed 's/::.*/::/'
    cat $1 | egrep -v :: | awk -F: '{print $1":"$2":"$3":"$4"::"}'
    cat $1 | egrep :: | egrep '^[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}|[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}:[0-9a-fA-F]{1,4}$' | sed 's/::/:0:0:/' | awk -F: '{print $1":"$2":"$3":"$4"::"}'
  }
} | sort -n

exit 0
