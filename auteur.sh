#!/bin/sh

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

# Copyright (c) 2021 David Jackson

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
