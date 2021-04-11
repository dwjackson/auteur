#!/bin/sh

if [ "$#" -lt 1 ]
then
	echo 'No screenplay file specified'
	exit 1
fi

# Convert the screenplay file to PostScript
file="$1"
tmp=`mktemp`
./auteur "$file" > "$tmp"

# Render the screenplay
out_file="$file.pdf"
ps2pdf "$tmp" "$out_file"

# Clean up
rm "$tmp"
