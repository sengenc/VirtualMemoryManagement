#!/bin/bash
# showargs.sh
# Returns the parameters with numbers

showargs() {
    i=1
    for var in "$@"
    do
        echo $i. $var
        let i++
    done
}

showargs $@