#!/bin/bash
# anzahlsubdirs.sh
# Return number of subdirectories of directory, given as parameter

anzahlsubdirs() {
	echo $((ls -l $1)| grep ^d | wc -l)
}

anzahlsubdirs $1