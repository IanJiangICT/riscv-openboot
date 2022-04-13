#!/bin/bash

plat_list="null"
plat_list="$plat_list apple"
plat_list="$plat_list banana"
plat_list="$plat_list grape"

echo "Build for all platforms: $plat_list"
for plat_name in $plat_list
do
	ts=`date +"%Y%m%d.%H%M%S"`
	./build.sh $plat_name
	mv build build-$plat_name-$ts
	ls -l build-$plat_name-$ts/*hex
done
