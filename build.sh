#!/bin/bash

plat_name="null"
plat_dir="./plat-$plat_name"
build_dir="./build"
src_dir="./src"

if [ $# -gt 0 ]
then
	plat_name=$1
fi
if [ -d $plat_dir ]
then
	echo "Build for platform $plat_name"
else
	echo "Error: Platform directory not found $plat_dir"
	exit 2
fi

echo "Prepare"
if [ -d $build_dir ]
then
	rm -rf $build_dir
fi
mkdir $build_dir
for f in `ls $src_dir`
do
	if [ -L $src_dir/$f ]
	then
		rm -f $src_dir/$f
	fi
done

for f in `ls $plat_dir`
do
	ln -s ../$plat_dir/$f $src_dir/$f
done

echo "Build bootconf"
make -f Makefile.bc 

echo "Build ZSBL"
make -f Makefile.zsbl-mc
make -f Makefile.zsbl-pc

exit 0