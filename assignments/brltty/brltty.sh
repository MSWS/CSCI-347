#!/bin/bash

echo -n "Number of braile drivers: " && grep -c "^#braille-driver" brltty.conf.rtf

echo "List of braile devices:"
grep "#braille-device" brltty.conf.rtf | grep -Eo "\s#.+" | sed -E "s/.+# /\t/g" | sed "s/\.\\\//g"
