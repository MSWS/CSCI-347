#!/bin/bash

echo -n "Number of braile drivers: " && grep -c "^#braille-driver" $1

echo "List of braile devices:"
grep "#braille-device" $1| grep -Eo "\s#.+" | sed -E "s/.+# /\t/g" | sed "s/\.\\\//g" 
echo "Number of text tables: $(grep -c "^#text-table" $1)"
echo "Number of contraction tables: $(grep -c "^#contraction-table" $1)"
