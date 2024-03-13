#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: ./run_program.sh <directory>"
    exit 1
fi

image_dir=$1
image_files=()

for file in "$image_dir"/*.ppm; do
		image_files+=("$file")
done

if [ ${#image_files[@]} -eq 0 ]; then
	echo "No PPM files found in $image_dir"
	exit 1
fi

./edge_detector "${image_files[@]}"
