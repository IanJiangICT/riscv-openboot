#!/bin/bash

target=vcs
#target=zebu

if [ $# -gt 0 ]
then
	target=$1
fi

build_dir=./build-flash-$target
bin_dir=./build
work_dir=`pwd`

echo "== Build flash for $target under $build_dir =="

rm -rf $build_dir
mkdir -p $build_dir/flash_data

f0=$bin_dir/bc1-$target.elf.bin
f1=
f2=$bin_dir/fsbl.elf.bin
f3= #$bin_dir/ss.elf.bin

if [ ! -f $f0 ]
then
	echo "Error: Not found file 0 $f0"
	exit 1
else
	echo " - file 0 $f0"
	cp $f0 $build_dir/flash_data/0bc1
fi

if [ -z $f1 ]
then
	echo "Note: Use zero file 1"
	dd if=/dev/zero of=$build_dir/flash_data/1ddrconf bs=1024 count=2 > /dev/null 2>&1
else
	if [ ! -f $1 ]
	then
		echo "Error: Not found file 1 $f1"
		exit 1
	else
	echo " - file 1 $f1"
	cp $f1 $build_dir/flash_data/1ddrconf
	fi
fi

if [ ! -f $f2 ]
then
	echo "Error: Not found file 2 $f2"
	exit 1
else
	echo " - file 2 $f2"
	cp $f2 $build_dir/flash_data/2fsbl
fi

if [ -z $f3 ]
then
	echo "Note: Without file 3"
else
	if [ ! -f $3 ]
	then
		echo "Error: Not found file 3 $f3"
		exit 1
	else
	echo " - file 3 $f3"
	cp $f3 $build_dir/flash_data/3x
	fi
fi

echo "== File list to build in =="
ls -l $build_dir/flash_data/*

cd $build_dir

$work_dir/script/mkimg.sh -p flash_data flash.bin
> flash.hex
echo "@0000" >> flash.hex
xxd -c 4 -g 1 flash.bin | awk '{print $2,$3,$4,$5}' >> flash.hex 
ls -l flash.hex
wc -l flash.hex
head -n 10 flash.hex
echo "..."
tail -n 10 flash.hex

cd $work_dir

exit 0
